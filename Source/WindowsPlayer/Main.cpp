#include <Modules\ModuleManager.h>
#include <Modules\ConcurrentModuleExecuter.h>
#include <Modules\StaticModulePlanner.h>

#include <Tools\Math\Vector.h>
#include <Tools\Math\Matrix.h>
#include <Tools\StopWatch.h>

#include <Foundation\UnitModule.h>
#include <Foundation\TimeModule.h>
#include <Foundation\LogModule.h>
#include <Foundation\TransformModule.h>
#include <Rendering\CameraModule.h>
#include <Rendering\ImageModule.h>
#include <Rendering\MaterialModule.h>
#include <Rendering\MeshModule.h>
#include <Rendering\MeshRendererModule.h>
#include <Rendering\UnlitRenderingLoop\UnlitRenderingLoopModule.h>
#include <Rendering\ShaderModule.h>
#include <Rendering\StorageModule.h>
#include <Rendering\SurfaceModule.h>
#include <Rendering\SamplerModule.h>

#include <Windows\Graphics\D12\D12GraphicsModule.h>
#include <Windows\Views\WinViewModule.h>

class TestModule : public Module
{
public:
	TestModule() : frame(0) {}

	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
	{
		viewModule = ExecuteBefore<IViewModule>(moduleManager);
		materialModule = ExecuteBefore<MaterialModule>(moduleManager);
		shaderModule = ExecuteBefore<ShaderModule>(moduleManager);
		meshRendererModule = ExecuteBefore<MeshRendererModule>(moduleManager);
		meshModule = ExecuteBefore<MeshModule>(moduleManager);
		cameraModule = ExecuteBefore<CameraModule>(moduleManager);
		surfaceModule = ExecuteBefore<SurfaceModule>(moduleManager);
		graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
		unitModule = ExecuteBefore<UnitModule>(moduleManager);
		transformModuke = ExecuteBefore<TransformModule>(moduleManager);
		logModule = ExecuteBefore<LogModule>(moduleManager);
	}

	const Shader* CreateShader(const ExecutionContext& context)
	{
		auto source =
			R"(

cbuffer _perCameraData : register(b0)
{
	float4x4 _worldToCamera;
};

cbuffer _perMeshData : register(b1)
{
	float4x4 _objectToWorld;
};

struct AppData
{
	float4 position : POSITION;
};

struct VertData
{
	float4 position : SV_POSITION;
};

VertData VertMain(AppData i)
{
	VertData o;
	o.position = mul(_worldToCamera, mul(_objectToWorld, i.position));
	return o;
}

float4 FragMain(VertData i) : SV_TARGET
{
	return float4(1, 1, 1, 1);
}
			)";

		VertexLayout vertexLayout;
		vertexLayout.attributes.push_back(VertexAttributeLayout(VertexAttributeTypePosition, ColorFormatRGBA32));

		auto shaderDesc = new ShaderPipelineDesc();
		shaderDesc->name = "Test";
		shaderDesc->source = (const uint8_t*) source;
		shaderDesc->sourceSize = strlen(source);
		shaderDesc->states.zTest = ZTestLEqual;
		shaderDesc->states.zWrite = ZWriteOn;
		shaderDesc->varation = 0;
		shaderDesc->vertexLayout = vertexLayout;
		shaderDesc->parameters.push_back(ShaderParameter("_perCameraData", ShaderParameterTypeConstantBuffer));
		shaderDesc->parameters.push_back(ShaderParameter("_perMeshData", ShaderParameterTypeConstantBuffer));

		auto shader = shaderModule->RecCreateShader(context);
		shaderModule->RecSetShaderPipeline(context, shader, 0, shaderDesc);
		return shader;
	}

	const Unit* CreateQuad(const ExecutionContext& context, const Shader* shader, const Mesh* mesh, Vector3f position)
	{
		auto triangle = unitModule->RecCreateUnit(context);

		// Create transform
		auto transform = transformModuke->RecCreateTransform(context);
		transformModuke->RecSetPosition(context, transform, position);
		unitModule->RecAddComponent(context, triangle, transform);

		auto material = materialModule->RecCreateMaterial(context);
		materialModule->RecSetShader(context, material, shader);

		auto meshRenderer = meshRendererModule->RecCreateMeshRenderer(context);
		meshRendererModule->RecSetMaterial(context, meshRenderer, material);
		meshRendererModule->RecSetMesh(context, meshRenderer, mesh);
		unitModule->RecAddComponent(context, triangle, meshRenderer);
		return triangle;
	}

	const Mesh* CreateMesh(const ExecutionContext& context)
	{
		static float vertices[] = {
			-0.5f, -0.5f, 0, 1,
			-0.5f, 0.5f, 0, 1,
			0.5f, 0.5f, 0, 1,

			-0.5f, -0.5f, 0, 1,
			0.5f, 0.5f, 0, 1,
			0.5f, -0.5f, 0, 1,
		};

		VertexLayout vertexLayout;
		vertexLayout.attributes.push_back(VertexAttributeLayout(VertexAttributeTypePosition, ColorFormatRGBA32));

		auto mesh = meshModule->RecCreateMesh(context, vertexLayout);
		meshModule->RecSetVertices(context, mesh, Range<uint8_t>((uint8_t*) vertices, sizeof(vertices)));
		meshModule->RecSetSubMesh(context, mesh, 0, SubMesh(6));
		return mesh;
	}

	void Initialize(const ExecutionContext& context)
	{
		static const Unit* movingCamera;

		if (frame != 0)
		{
			auto transform = unitModule->GetComponent<Transform>(movingCamera);
			transformModuke->RecSetPosition(context, transform, transform->localPosition + Vector3f(Math::Sin(frame / 10.0f), 0, 0));
			return;
		}

		logModule->RecWriteFmt(context, "Initializing test scene %d\n", 1);

		// Create window
		//auto view = viewModule->RecCreateIView(context);
		//auto view2 = viewModule->RecCreateIView(context);

		/*{
			auto mainCamera = unitModule->RecCreateUnit(context);

			// Create transform
			auto transform = transformModuke->RecCreateTransform(context);
			transformModuke->RecSetPosition(context, transform, Vector3f(0, 0, -1));
			unitModule->RecAddComponent(context, mainCamera, transform);

			// Create camera with window as target
			auto surface = surfaceModule->RecCreateSurface(context);
			surfaceModule->RecSetColor(context, surface, 0, SurfaceColor(view->renderTarget, LoadActionClear, StoreActionStore));
			surfaceModule->RecSetViewport(context, surface, Viewport(Rectf(0.5f, 0, 0.5f, 1)));
			auto camera = cameraModule->RecCreateCamera(context);
			cameraModule->RecSetSurface(context, camera, surface);
			unitModule->RecAddComponent(context, mainCamera, camera);
		}

		{
			auto mainCamera = unitModule->RecCreateUnit(context);

			// Create transform
			auto transform = transformModuke->RecCreateTransform(context);
			transformModuke->RecSetPosition(context, transform, Vector3f(0, 0, -10));
			unitModule->RecAddComponent(context, mainCamera, transform);

			// Create camera with window as target
			auto surface = surfaceModule->RecCreateSurface(context);
			surfaceModule->RecSetColor(context, surface, 0, SurfaceColor(view->renderTarget, LoadActionLoad, StoreActionStore));
			surfaceModule->RecSetViewport(context, surface, Viewport(Rectf(0, 0, 0.5f, 1)));
			auto camera = cameraModule->RecCreateCamera(context);
			cameraModule->RecSetSurface(context, camera, surface);
			unitModule->RecAddComponent(context, mainCamera, camera);
		}*/

		for (int i = 0; i < 1; i++)
		{
			auto view = viewModule->RecCreateIView(context);

			auto mainCamera = unitModule->RecCreateUnit(context);
			movingCamera = mainCamera;

			// Create transform
			auto transform = transformModuke->RecCreateTransform(context);
			transformModuke->RecSetPosition(context, transform, Vector3f(0, 0, -10));
			unitModule->RecAddComponent(context, mainCamera, transform);

			// Create camera with window as target
			auto surface = surfaceModule->RecCreateSurface(context);
			surfaceModule->RecSetColor(context, surface, 0, SurfaceColor(view->renderTarget, LoadActionClear, StoreActionStore));
			surfaceModule->RecSetViewport(context, surface, Viewport(Rectf(0, 0, 1, 1)));
			auto camera = cameraModule->RecCreateCamera(context);
			cameraModule->RecSetSurface(context, camera, surface);
			unitModule->RecAddComponent(context, mainCamera, camera);
		}

		auto testShader = CreateShader(context);
		auto mesh = CreateMesh(context);
		for (int i = 0; i < 200; i++)
		{
			for (int j = 0; j < 200; j++)
			{
				auto quad = CreateQuad(context, testShader, mesh, Vector3f(i*2, j*2, 0));
			}
		}
	}

	virtual void Execute(const ExecutionContext& context) override
	{
		Initialize(context);
		frame++;
	}

	UnitModule* unitModule;
	TransformModule* transformModuke;
	IViewModule* viewModule;
	MaterialModule* materialModule;
	ShaderModule* shaderModule;
	MeshRendererModule* meshRendererModule;
	MeshModule* meshModule;
	CameraModule* cameraModule;
	SurfaceModule* surfaceModule;
	IGraphicsModule* graphicsModule;
	LogModule* logModule;
	uint32_t frame;
};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	auto planner = new StaticModulePlanner();
	auto executer = new ConcurrentModuleExecuter(planner, 4);
	auto moduleManager = new ModuleManager(planner, executer);

	moduleManager->AddModule(new LogModule());
	moduleManager->AddModule(new UnitModule());
	moduleManager->AddModule(new D12GraphicsModule());
	moduleManager->AddModule(new TransformModule());
	moduleManager->AddModule(new SamplerModule());
	moduleManager->AddModule(new ImageModule());
	moduleManager->AddModule(new WinViewModule(hInst));
	moduleManager->AddModule(new StorageModule());
	moduleManager->AddModule(new ShaderModule());
	moduleManager->AddModule(new MaterialModule());
	moduleManager->AddModule(new MeshModule());
	moduleManager->AddModule(new MeshRendererModule());
	moduleManager->AddModule(new SurfaceModule());
	moduleManager->AddModule(new CameraModule());
	moduleManager->AddModule(new UnlitRenderingLoopModule());
	moduleManager->AddModule(new TestModule());
	moduleManager->AddModule(new TimeModule());
	moduleManager->AddModule(new MemoryModule());

	moduleManager->Start();
	while (moduleManager->IsRunning())
	{
		EXT_TRACE("----------FRAME--------------");
		moduleManager->NewFrame();
		moduleManager->WaitForFrame();
	}

	delete moduleManager;
	delete executer;
	delete planner;

	return 0;
}