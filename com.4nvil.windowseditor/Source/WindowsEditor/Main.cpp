#include <Core\Tools\Math\Vector.hpp>
#include <Core\Tools\Math\Matrix.hpp>
#include <Core\Tools\StopWatch.hpp>
#include <Core\Modules\ModuleManager.hpp>
#include <Core\Modules\ConcurrentModuleExecutor.hpp>
#include <Core\Modules\StaticModulePlanner.hpp>
#include <Core\Foundation\UnitModule.hpp>
#include <Core\Foundation\TimeModule.hpp>
#include <Core\Foundation\LogModule.hpp>
#include <Core\Foundation\TransformModule.hpp>
#include <Core\Foundation\ProfilerModule.hpp>
#include <Core\Rendering\CameraModule.hpp>
#include <Core\Rendering\ImageModule.hpp>
#include <Core\Rendering\MaterialModule.hpp>
#include <Core\Rendering\MeshModule.hpp>
#include <Core\Rendering\MeshRendererModule.hpp>
#include <Core\Rendering\RenderLoop\UnlitRenderLoopModule.hpp>
#include <Core\Rendering\ShaderModule.hpp>
#include <Core\Rendering\StorageModule.hpp>
#include <Core\Rendering\SurfaceModule.hpp>
#include <Core\Rendering\SamplerModule.hpp>
#include <Core\Input\InputModule.hpp>
#include <Core\Input\MouseModule.hpp>
#include <Windows\Graphics\Directx12\GraphicsModule.hpp>
#include <Windows\Views\ViewModule.hpp>
#include <Editor\Views\GameViewLayerModule.hpp>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;
using namespace Windows;
using namespace Editor;

struct Agent : public Component
{
	Agent(ComponentModule* module) : Component(module) {}
	const Transform* transform;
	Vector3f velocity;
	Vector3f destination;
	bool seekDestination;
	float acceleration;
	float maxSpeed;
	float radius;
};

class AgentModule : public ComponentModule
{
public:
	BASE_IS(ComponentModule);
	DECLARE_COMMAND_CODE(CreateAgent);
	DECLARE_COMMAND_CODE(SetDestination);

	const Agent* AllocateAgent()
	{
		return memoryModule->New<Agent>("AgentModule", this);
	}

	const Agent* RecCreateAgent(const ExecutionContext& context, const Agent* target = nullptr)
	{
		auto buffer = GetRecordingBuffer(context);
		auto& stream = buffer->stream;
		target = target == nullptr ? AllocateAgent() : target;
		stream.Write(TO_COMMAND_CODE(CreateAgent));
		stream.Write(target);
		stream.Align();
		buffer->commandCount++;
		return target;
	}

	void RecSetDestination(const ExecutionContext& context, const Agent* target, const Transform* destination)
	{
		auto buffer = GetRecordingBuffer(context);
		auto& stream = buffer->stream;
		stream.Write(TO_COMMAND_CODE(SetDestination));
		stream.Write(target);
		stream.Write(destination);
		stream.Align();
		buffer->commandCount++;
	}

	virtual void RecDestroy(const ExecutionContext& context, const Component* unit) override {}

	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
	{
		base::SetupExecuteOrder(moduleManager);
		unitModule = ExecuteAfter<UnitModule>(moduleManager);
		memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
		memoryModule->SetAllocator("AgentModule", new FixedBlockHeap(sizeof(Agent)));
		mouseModule = ExecuteAfter<MouseModule>(moduleManager);
		cameraModule = ExecuteAfter<CameraModule>(moduleManager);
	}

	virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override
	{
		switch (commandCode)
		{
			DESERIALIZE_METHOD_ARG1_START(CreateAgent, Agent*, target);
			target->acceleration = 1.0f;
			target->maxSpeed = 2.0f;
			target->velocity = 0.0f;
			target->radius = 1.2f;
			target->transform = unitModule->GetComponent<Transform>(target->unit);
			target->seekDestination = nullptr;
			agents.push_back(target);
			DESERIALIZE_METHOD_END;
		}
		return false;
	}

	virtual void Execute(const ExecutionContext& context) override
	{
		MARK_FUNCTION;
		base::Execute(context);

		auto screenPosition = mouseModule->GetPosition();
		auto screenPosition3 = Vector3f(screenPosition.x, screenPosition.y, 0.999549866f);
		auto worldPosition = cameraModule->ScreenToWorld(cameraModule->GetCameras()[0], screenPosition3);

		for (auto agent : agents)
		{
			agent->seekDestination = mouseModule->GetButton(MouseButtonType::Left);
			agent->destination = worldPosition;
		}
	}

	List<Agent*>* GetAgents() { return &agents; }

private:
	UnitModule* unitModule;
	MemoryModule* memoryModule;
	MouseModule* mouseModule;
	CameraModule* cameraModule;
	List<Agent*> agents;
};

class AgentForceModule : public ComputeModule
{
public:
	BASE_IS(ComputeModule);

	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
	{
		base::SetupExecuteOrder(moduleManager);
		transformModule = ExecuteAfter<TransformModule>(moduleManager);
		agentModule = ExecuteAfter<AgentModule>(moduleManager);
		timeModule = ExecuteAfter<TimeModule>(moduleManager);
		agents = agentModule->GetAgents();
	}

	virtual size_t GetExecutionSize() override { return agents->size(); }
	virtual size_t GetSplitExecutionSize() override { return Math::SplitJobs(GetExecutionSize(), 4, 10); }

	virtual void Execute(const ExecutionContext& context) override
	{
		MARK_FUNCTION;

		for (auto i = context.start; i < context.end; i++)
		{
			auto agent = agents->at(i);
			auto agentTransform = agent->transform;
			Vector3f impact = 0;
			if (agent->seekDestination)
				impact += GetSeek(agent, agent->destination);
			impact += GetSeparation(agent) * 2.2f;

			agent->velocity += impact * timeModule->GetDeltaTime();

			// Limit the speed
			auto speed = agent->velocity.Magnitude();
			if (speed > agent->maxSpeed)
				agent->velocity *= agent->maxSpeed / speed;

			transformModule->RecAddPosition(context, agentTransform, agent->velocity);
		}
	}

	Vector3f GetSeek(Agent* agent, Vector3f destination)
	{
		auto position = agent->transform->localPosition;

		if (position == destination)
			return Vector3f::zero;

		auto desired = destination - position;

		desired *= agent->maxSpeed / desired.Magnitude();

		auto velocityChange = desired - agent->velocity;

		velocityChange *= agent->acceleration / agent->maxSpeed;

		return velocityChange;
	}

	Vector3f GetSeparation(Agent* targetAgent)
	{
		Vector3f totalForce = 0;
		int neighboursCount = 0;
		float seperation = 3;

		auto targetTransform = targetAgent->transform;
		for (auto agent : *agents)
		{
			if (targetAgent == agent)
				continue;

			auto agentTransform = agent->transform;
			auto pushForce = agentTransform->localPosition - targetTransform->localPosition;
			auto distance = pushForce.Magnitude();

			if (distance <= seperation)
			{
				float r = (agent->radius + targetAgent->radius);
				totalForce += pushForce*(1.0f - ((distance - r) / (seperation - r)));
				neighboursCount++;
			}
		}

		if (neighboursCount == 0)
			return Vector3f::zero;

		return totalForce * (targetAgent->maxSpeed / neighboursCount);
	}

private:
	TransformModule* transformModule;
	AgentModule* agentModule;
	TimeModule* timeModule;
	List<Agent*>* agents;
};

class AgentDistModule : public ComputeModule
{
public:
	BASE_IS(ComputeModule);

	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
	{
		base::SetupExecuteOrder(moduleManager);
		transformModule = ExecuteAfter<TransformModule>(moduleManager);
		agentModule = ExecuteAfter<AgentModule>(moduleManager);
		timeModule = ExecuteAfter<TimeModule>(moduleManager);
		ExecuteAfter<AgentForceModule>(moduleManager);
		agents = agentModule->GetAgents();
		totalPassedTime = 0;
	}

	virtual size_t GetExecutionSize() override { return agents->size(); }
	virtual size_t GetSplitExecutionSize() override { return Math::SplitJobs(GetExecutionSize(), 4, 10); }

	virtual void Execute(const ExecutionContext& context) override
	{
		MARK_FUNCTION;

		totalPassedTime += timeModule->GetDeltaTime() / 4.0f;

		for (auto i = context.start; i < context.end; i++)
		{
			auto agent = agents->at(i);
			auto agentTransform = agent->transform;
			transformModule->RecAddPosition(context, agentTransform, GetDistinguish(agent));
			transformModule->RecSetRotation(context, agentTransform, Vector3f(0, 0, totalPassedTime));
		}
	}

	Vector3f GetDistinguish(Agent* targetAgent)
	{
		Vector3f totalForce(0, 0, 0);
		auto targetPosition = targetAgent->transform->localPosition;
		for (auto agent : *agents)
		{
			if (targetAgent == agent)
				continue;

			auto position = agent->transform->localPosition;
			auto direction = position - targetPosition;
			auto directionMagnitude = direction.Magnitude();
			auto distance = agent->radius + targetAgent->radius - directionMagnitude;
			auto seperate = direction / directionMagnitude*distance;

			if (distance > 0 && directionMagnitude > 0)
			{
				totalForce += Vector3f(seperate.x, 0, seperate.y);
			}
		}

		return totalForce;
	}

private:
	TransformModule* transformModule;
	TimeModule* timeModule;
	AgentModule* agentModule;
	List<Agent*>* agents;
	float totalPassedTime;
};

class ShutdownModule : public ComputeModule
{
public:
	BASE_IS(ComputeModule);

	ShutdownModule(ModuleManager* moduleManager)
		: moduleManager(moduleManager)
	{
	}

	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
	{
		base::SetupExecuteOrder(moduleManager);
		viewModule = ExecuteAfter<IViewModule>(moduleManager);
	}

	virtual void Execute(const ExecutionContext& context) override
	{
		if (viewModule->GetViews().size() == 0)
			moduleManager->RequestStop();
	}

private:
	IViewModule* viewModule;
	ModuleManager* moduleManager;
};

class FpsLoggerModule : public ComputeModule
{
public:
	BASE_IS(ComputeModule);

	FpsLoggerModule()
		: passedFrameCount(0)
	{
	}

	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
	{
		base::SetupExecuteOrder(moduleManager);
		logModule = ExecuteBefore<LogModule>(moduleManager);
		stopWatch.Start();
	}

	virtual void Execute(const ExecutionContext& context) override
	{
		MARK_FUNCTION;
		
		auto dt = stopWatch.GetElapsedMiliseconds();
		if (dt >= 8000)
		{
			// Hystory of all ms changes
			// 78 58 59
			// 69 59 61
			// 67 57 57
			// 36 33 33
			// 22 21 20
			// 16 15 15

			auto msPerFrame = (float) dt / passedFrameCount;
			TRACE("Frame took ms %f", msPerFrame);
			logModule->RecWriteFmt(context, "Frame took ms %f\n", msPerFrame);

			stopWatch.Restart();
			passedFrameCount = 0;
		}
		passedFrameCount++;
	}

private:
	StopWatch stopWatch;
	LogModule* logModule;
	uint64 passedFrameCount;
};

class TestModule : public ComputeModule
{
public:
	BASE_IS(ComputeModule);

	TestModule() : frame(0) {}

	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
	{
		base::SetupExecuteOrder(moduleManager);
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
		gameViewLayerModule = ExecuteBefore<GameViewLayerModule>(moduleManager);
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
	return float4(1, 1, 0, 1);
}
			)";

		VertexLayout vertexLayout;
		vertexLayout.attributes.push_back(VertexAttributeLayout(VertexAttributeType::Position, ColorFormat::RGBA32));

		auto shaderDesc = new ShaderPipelineDesc();
		shaderDesc->name = "Test";
		shaderDesc->source = (const uint8*) source;
		shaderDesc->sourceSize = strlen(source);
		shaderDesc->states.zTest = ZTest::LEqual;
		shaderDesc->states.zWrite = ZWrite::On;
		shaderDesc->varation = 0;
		shaderDesc->vertexLayout = vertexLayout;
		shaderDesc->parameters.push_back(ShaderParameter("_perCameraData", ShaderParameterType::ConstantBuffer));
		shaderDesc->parameters.push_back(ShaderParameter("_perMeshData", ShaderParameterType::ConstantBuffer));

		auto shader = shaderModule->RecCreateShader(context);
		shaderModule->RecSetShaderPipeline(context, shader, 0, shaderDesc);
		return shader;
	}

	const Unit* CreateQuad(const ExecutionContext& context, const Shader* shader, const Mesh* mesh, Vector3f position)
	{
		auto triangle = unitModule->AllocateUnit();
		unitModule->RecCreateUnit(context, triangle);

		// Create transform
		auto transform = transformModuke->AllocateTransform();
		transformModuke->RecSetPosition(context, transform, position);
		transformModuke->RecCreateTransform(context, transform);
		unitModule->RecAddComponent(context, triangle, transform);

		auto material = materialModule->AllocateMaterial();
		materialModule->RecCreateMaterial(context, material);
		materialModule->RecSetShader(context, material, shader);

		auto meshRenderer = meshRendererModule->AllocateMeshRenderer();
		meshRendererModule->RecSetMaterial(context, meshRenderer, material);
		meshRendererModule->RecSetMesh(context, meshRenderer, mesh);
		meshRendererModule->RecCreateMeshRenderer(context, meshRenderer);
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
		vertexLayout.attributes.push_back(VertexAttributeLayout(VertexAttributeType::Position, ColorFormat::RGBA32));

		auto mesh = meshModule->RecCreateMesh(context, vertexLayout);
		meshModule->RecSetVertices(context, mesh, Range<uint8>((uint8*) vertices, sizeof(vertices)));
		meshModule->RecSetSubMesh(context, mesh, 0, SubMesh(6));
		return mesh;
	}

	void Initialize(const ExecutionContext& context)
	{
		MARK_FUNCTION;

		static const Unit* movingCamera;

		if (frame != 0)
		{
			auto transform = unitModule->GetComponent<Transform>(movingCamera);
			transformModuke->RecSetPosition(context, transform, transform->localPosition + Vector3f(Math::Sin(frame / 10.0f), 0, 0));
			return;
		}

		logModule->RecWriteFmt(context, "Initializing test scene %d\n", 1);

		int count = 200;
		auto offset = count * 1;

		for (int i = 0; i < 1; i++)
		{
			auto view = viewModule->AllocateView();
			viewModule->RecSetName(context, view, "Performance Test with 40000 objects");
			viewModule->RecCreateIView(context, view);

			gameViewLayerModule->RecShow(context, view);

			auto mainCamera = unitModule->AllocateUnit();
			unitModule->RecCreateUnit(context, mainCamera);
			movingCamera = mainCamera;

			// Create transform
			auto transform = transformModuke->AllocateTransform();
			transformModuke->RecSetPosition(context, transform, Vector3f(0, 0, -400));
			transformModuke->RecCreateTransform(context, transform);
			unitModule->RecAddComponent(context, mainCamera, transform);

			// Create camera with window as target
			auto camera = cameraModule->AllocateCamera();
			cameraModule->RecCreateCamera(context, camera);
			cameraModule->RecSetSurface(context, camera, gameViewLayerModule->GetSurface());
			unitModule->RecAddComponent(context, mainCamera, camera);
		}

		auto testShader = CreateShader(context);
		auto mesh = CreateMesh(context);
		for (int i = 0; i < count; i++)
		{
			for (int j = 0; j < count; j++)
			{
				auto quad = CreateQuad(context, testShader, mesh, Vector3f(i*2.0f - offset, j*2.0f - offset, 0.0f));
			}
		}
	}

	virtual void Execute(const ExecutionContext& context) override
	{
		Initialize(context);
		frame++;
	}

	UnitModule* unitModule;
	GameViewLayerModule* gameViewLayerModule;
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
	uint32 frame;
};

class Test2Module : public ComputeModule
{
public:
	BASE_IS(ComputeModule);

	Test2Module() : frame(0) {}

	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
	{
		base::SetupExecuteOrder(moduleManager);
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
		agentModule = ExecuteBefore<AgentModule>(moduleManager);
		gameViewLayerModule = ExecuteBefore<GameViewLayerModule>(moduleManager);
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
	return float4(1, 1, 0, 1);
}
			)";

		VertexLayout vertexLayout;
		vertexLayout.attributes.push_back(VertexAttributeLayout(VertexAttributeType::Position, ColorFormat::RGBA32));

		auto shaderDesc = new ShaderPipelineDesc();
		shaderDesc->name = "Test";
		shaderDesc->source = (const uint8*) source;
		shaderDesc->sourceSize = strlen(source);
		shaderDesc->states.zTest = ZTest::LEqual;
		shaderDesc->states.zWrite = ZWrite::On;
		shaderDesc->varation = 0;
		shaderDesc->vertexLayout = vertexLayout;
		shaderDesc->parameters.push_back(ShaderParameter("_perCameraData", ShaderParameterType::ConstantBuffer));
		shaderDesc->parameters.push_back(ShaderParameter("_perMeshData", ShaderParameterType::ConstantBuffer));

		auto shader = shaderModule->RecCreateShader(context);
		shaderModule->RecSetShaderPipeline(context, shader, 0, shaderDesc);
		return shader;
	}

	const Unit* CreateQuad(const ExecutionContext& context, const Shader* shader, const Mesh* mesh, Vector3f position)
	{
		auto triangle = unitModule->AllocateUnit();
		unitModule->RecCreateUnit(context, triangle);

		// Create transform
		auto transform = transformModuke->AllocateTransform();
		transformModuke->RecSetPosition(context, transform, position);
		transformModuke->RecCreateTransform(context, transform);
		unitModule->RecAddComponent(context, triangle, transform);

		auto material = materialModule->AllocateMaterial();
		materialModule->RecCreateMaterial(context, material);
		materialModule->RecSetShader(context, material, shader);

		auto meshRenderer = meshRendererModule->AllocateMeshRenderer();
		meshRendererModule->RecSetMaterial(context, meshRenderer, material);
		meshRendererModule->RecSetMesh(context, meshRenderer, mesh);
		meshRendererModule->RecCreateMeshRenderer(context, meshRenderer);
		unitModule->RecAddComponent(context, triangle, meshRenderer);

		auto agent = agentModule->RecCreateAgent(context);
		unitModule->RecAddComponent(context, triangle, agent);

		return triangle;
	}

	const Mesh* CreateMesh(const ExecutionContext& context)
	{
		volatile static float vertices[] = {
			-0.5f, -0.5f, 0, 1,
			-0.5f, 0.5f, 0, 1,
			0.5f, 0.5f, 0, 1,

			-0.5f, -0.5f, 0, 1,
			0.5f, 0.5f, 0, 1,
			0.5f, -0.5f, 0, 1,
		};

		VertexLayout vertexLayout;
		vertexLayout.attributes.push_back(VertexAttributeLayout(VertexAttributeType::Position, ColorFormat::RGBA32));

		auto mesh = meshModule->RecCreateMesh(context, vertexLayout);
		meshModule->RecSetVertices(context, mesh, Range<uint8>((uint8*) vertices, sizeof(vertices)));
		meshModule->RecSetSubMesh(context, mesh, 0, SubMesh(6));
		return mesh;
	}

	void Initialize(const ExecutionContext& context)
	{
		MARK_FUNCTION;
		static const Unit* movingCamera;

		if (frame != 0)
		{
			return;
		}

		logModule->RecWriteFmt(context, "Initializing test scene %d\n", 1);

		int count = 30;
		auto offset = count * 1;

		for (int i = 0; i < 1; i++)
		{
			auto view = viewModule->AllocateView();
			viewModule->RecCreateIView(context, view);

			gameViewLayerModule->RecShow(context, view);

			auto mainCamera = unitModule->AllocateUnit();
			unitModule->RecCreateUnit(context, mainCamera);
			movingCamera = mainCamera;

			// Create transform
			auto transform = transformModuke->AllocateTransform();
			transformModuke->RecSetPosition(context, transform, Vector3f(0, 0, -400));
			transformModuke->RecCreateTransform(context, transform);
			unitModule->RecAddComponent(context, mainCamera, transform);

			// Create camera with window as target
			auto camera = cameraModule->AllocateCamera();
			cameraModule->RecSetSurface(context, camera, gameViewLayerModule->GetSurface());
			cameraModule->RecCreateCamera(context, camera);
			unitModule->RecAddComponent(context, mainCamera, camera);
		}

		auto testShader = CreateShader(context);
		auto mesh = CreateMesh(context);
		for (int i = 0; i < count; i++)
		{
			for (int j = 0; j < count; j++)
			{
				auto quad = CreateQuad(context, testShader, mesh, Vector3f(i * 2.0f - offset, j * 2.0f - offset, 0));
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
	GameViewLayerModule* gameViewLayerModule;
	MaterialModule* materialModule;
	ShaderModule* shaderModule;
	MeshRendererModule* meshRendererModule;
	MeshModule* meshModule;
	CameraModule* cameraModule;
	SurfaceModule* surfaceModule;
	IGraphicsModule* graphicsModule;
	LogModule* logModule;
	AgentModule* agentModule;
	uint32 frame;
};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	auto planner = new StaticModulePlanner();
	auto executor = new ConcurrentModuleExecutor(planner, 4);
	auto moduleManager = new ModuleManager(planner, executor);
	
	// Core
	moduleManager->AddModule(new LogModule());
	moduleManager->AddModule(new UnitModule());
	moduleManager->AddModule(new TransformModule());
	moduleManager->AddModule(new SamplerModule());
	moduleManager->AddModule(new ImageModule());
	moduleManager->AddModule(new StorageModule()); 
	moduleManager->AddModule(new ShaderModule());
	moduleManager->AddModule(new MaterialModule());
	moduleManager->AddModule(new MeshModule());
	moduleManager->AddModule(new MeshRendererModule());
	moduleManager->AddModule(new SurfaceModule());
	moduleManager->AddModule(new CameraModule());
	moduleManager->AddModule(new UnlitRenderLoopModule());
	moduleManager->AddModule(new TimeModule());
	moduleManager->AddModule(new MemoryModule());
	moduleManager->AddModule(new ProfilerModule());
	moduleManager->AddModule(new InputModule());
	moduleManager->AddModule(new MouseModule());

	// Editor
	moduleManager->AddModule(new Editor::GameViewLayerModule());

	// Windows bound
	moduleManager->AddModule(new Windows::ViewModule(hInst));
	moduleManager->AddModule(new Windows::Directx12::GraphicsModule());

	/*// Test project 1
	moduleManager->AddModule(new TestModule());
	moduleManager->AddModule(new FpsLoggerModule());
	moduleManager->AddModule(new ShutdownModule(moduleManager));*/

	// Test project 2
	moduleManager->AddModule(new AgentModule());
	moduleManager->AddModule(new AgentForceModule());
	moduleManager->AddModule(new AgentDistModule());
	moduleManager->AddModule(new Test2Module());
	moduleManager->AddModule(new FpsLoggerModule());
	moduleManager->AddModule(new ShutdownModule(moduleManager));

	moduleManager->Start();
	while (moduleManager->IsRunning())
	{
		moduleManager->NewFrame();
		moduleManager->WaitForFrame();
	}
	moduleManager->Stop();

	delete moduleManager;
	delete executor;
	delete planner;

	return 0;
}