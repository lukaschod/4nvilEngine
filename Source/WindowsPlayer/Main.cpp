#include <Tools\Math\Vector.h>
#include <Tools\Math\Matrix.h>
#include <Tools\StopWatch.h>
#include <Modules\ModuleManager.h>
#include <Modules\ConcurrentModuleExecutor.h>
#include <Modules\StaticModulePlanner.h>
#include <Foundation\UnitModule.h>
#include <Foundation\TimeModule.h>
#include <Foundation\LogModule.h>
#include <Foundation\TransformModule.h>
#include <Foundation\ProfilerModule.h>
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
#include <Input\InputModule.h>
#include <Input\MouseModule.h>
#include <Windows\Graphics\Directx12\GraphicsModule.h>
#include <Windows\Views\ViewModule.h>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;
using namespace Core;
using namespace Windows;

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

/*class AgentModule : public ComponentModule
{
public:
	DECLARE_COMMAND_CODE(CreateAgent);
	DECLARE_COMMAND_CODE(SetDestination);

	const Agent* AllocateAgent() const
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
		ComponentModule::SetupExecuteOrder(moduleManager);
		transformModule = ExecuteAfter<TransformModule>(moduleManager);
		unitModule = ExecuteAfter<UnitModule>(moduleManager);
		memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
		timeModule = ExecuteAfter<TimeModule>(moduleManager);
		memoryModule->SetAllocator("AgentModule", new FixedBlockHeap(sizeof(Agent)));
	}

	virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override
	{
		switch (commandCode)
		{
			DESERIALIZE_METHOD_ARG1_START(CreateAgent, Agent*, target);
			target->acceleration = 1.0f;
			target->maxSpeed = 2.0f;
			target->velocity = Vector3f(0, 0, 0);
			target->radius = 1.2f;
			target->transform = unitModule->GetComponent<Transform>(target->unit);
			agents.push_back(target);
			DESERIALIZE_METHOD_END;
		}
		return false;
	}

	virtual void Execute(const ExecutionContext& context) override
	{
		MARK_FUNCTION;
		ComponentModule::Execute(context);

		for (auto agent : agents)
		{
			auto agentTransform = agent->transform;
			Vector3f impact(0, 0, 0);
			impact += GetSeek(agent, Vector3f(0, 0, 0));
			impact += GetSeparation(agent) * 2.2;

			agent->velocity += impact * timeModule->GetDeltaTime();

			// Limit the speed
			auto speed = agent->velocity.Magnitude();
			if (speed > agent->maxSpeed)
				agent->velocity *= agent->maxSpeed / speed;

			transformModule->RecAddPosition(context, agentTransform, agent->velocity);
		}

		for (auto agent : agents)
		{
			auto agentTransform = agent->transform;
			transformModule->RecAddPosition(context, agentTransform, GetDistinguish(agent));
		}
	}

	Vector3f GetSeek(Agent* agent, Vector3f destination)
	{
		auto position = agent->transform->localPosition;

		if (position == destination)
			return Vector3f(0, 0, 0);

		auto desired = destination - position;

		desired *= agent->maxSpeed / desired.Magnitude();

		auto velocityChange = desired - agent->velocity;

		velocityChange *= agent->acceleration / agent->maxSpeed;

		return velocityChange;
	}

	Vector3f GetSeparation(Agent* targetAgent)
	{
		Vector3f totalForce(0, 0, 0);
		int neighboursCount = 0;
		float seperation = 3;

		auto targetTransform = targetAgent->transform;
		for (auto agent : agents)
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
			return Vector3f(0, 0, 0);

		return totalForce * (targetAgent->maxSpeed/neighboursCount);
	}

	Vector3f GetDistinguish(Agent* targetAgent)
	{
		Vector3f totalForce(0, 0, 0);
		auto targetPosition = targetAgent->transform->localPosition;
		for (auto agent : agents)
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

	List<Agent*>* GetAgents() { return &agents; }

private:
	TransformModule* transformModule;
	UnitModule* unitModule;
	MemoryModule* memoryModule;
	TimeModule* timeModule;
	List<Agent*> agents;
};*/

class AgentModule : public ComponentModule
{
public:
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
		ComponentModule::SetupExecuteOrder(moduleManager);
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
			target->velocity = Vector3f(0, 0, 0);
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
		auto screenPosition3 = Vector3f(screenPosition.x, screenPosition.y, 0.9f);
		auto worldPosition = cameraModule->CalculateScreenToWorld(cameraModule->GetCameras()[0], screenPosition3);

		TRACE("%f %f %f", worldPosition.x, worldPosition.y, worldPosition.z);
		for (auto agent : agents)
		{
			agent->seekDestination = mouseModule->GetButtonState(MouseButtonType::Left) == MouseButtonState::Click;
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
			Vector3f impact(0, 0, 0);
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
			return Vector3f(0, 0, 0);

		auto desired = destination - position;

		desired *= agent->maxSpeed / desired.Magnitude();

		auto velocityChange = desired - agent->velocity;

		velocityChange *= agent->acceleration / agent->maxSpeed;

		return velocityChange;
	}

	Vector3f GetSeparation(Agent* targetAgent)
	{
		Vector3f totalForce(0, 0, 0);
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
			return Vector3f(0, 0, 0);

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
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
	{
		base::SetupExecuteOrder(moduleManager);
		transformModule = ExecuteAfter<TransformModule>(moduleManager);
		agentModule = ExecuteAfter<AgentModule>(moduleManager);
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
			transformModule->RecAddPosition(context, agentTransform, GetDistinguish(agent));
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
	AgentModule* agentModule;
	List<Agent*>* agents;
};

class ShutdownModule : public ComputeModule
{
public:
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
	TestModule() : frame(0) {}

	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
	{
		ComputeModule::SetupExecuteOrder(moduleManager);
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
			auto view = viewModule->RecCreateIView(context);

			auto mainCamera = unitModule->RecCreateUnit(context);
			movingCamera = mainCamera;

			// Create transform
			auto transform = transformModuke->RecCreateTransform(context);
			transformModuke->RecSetPosition(context, transform, Vector3f(0, 0, -400));
			unitModule->RecAddComponent(context, mainCamera, transform);

			// Create camera with window as target
			auto surface = surfaceModule->RecCreateSurface(context);
			surfaceModule->RecSetColor(context, surface, 0, SurfaceColor(view->renderTarget, LoadAction::Clear, StoreAction::Store));
			surfaceModule->RecSetViewport(context, surface, Viewport(Rectf(0, 0, 1, 1)));
			auto camera = cameraModule->RecCreateCamera(context);
			cameraModule->RecSetSurface(context, camera, surface);
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
	Test2Module() : frame(0) {}

	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
	{
		ComputeModule::SetupExecuteOrder(moduleManager);
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
		surfaceModule->RecSetColor(context, surface, 0, SurfaceColor(view->renderTarget, LoadAction::Clear, StoreAction::Store));
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
		surfaceModule->RecSetColor(context, surface, 0, SurfaceColor(view->renderTarget, LoadAction::Load, StoreAction::Store));
		surfaceModule->RecSetViewport(context, surface, Viewport(Rectf(0, 0, 0.5f, 1)));
		auto camera = cameraModule->RecCreateCamera(context);
		cameraModule->RecSetSurface(context, camera, surface);
		unitModule->RecAddComponent(context, mainCamera, camera);
		}*/

		int count = 30;
		auto offset = count * 1;

		for (int i = 0; i < 1; i++)
		{
			auto view = viewModule->RecCreateIView(context);

			auto mainCamera = unitModule->RecCreateUnit(context);
			movingCamera = mainCamera;

			// Create transform
			auto transform = transformModuke->RecCreateTransform(context);
			transformModuke->RecSetPosition(context, transform, Vector3f(0, 0, -400));
			unitModule->RecAddComponent(context, mainCamera, transform);

			// Create camera with window as target
			auto surface = surfaceModule->RecCreateSurface(context);
			surfaceModule->RecSetColor(context, surface, 0, SurfaceColor(view->renderTarget, LoadAction::Clear, StoreAction::Store));
			surfaceModule->RecSetViewport(context, surface, Viewport(Rectf(0, 0, 1, 1)));
			auto camera = cameraModule->RecCreateCamera(context);
			cameraModule->RecSetSurface(context, camera, surface);
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

void AddCoreModules(ModuleManager* manager)
{

}

void AddProjectModules(ModuleManager* manager)
{

}

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
	moduleManager->AddModule(new UnlitRenderingLoopModule());
	moduleManager->AddModule(new TimeModule());
	moduleManager->AddModule(new MemoryModule());
	moduleManager->AddModule(new ProfilerModule());
	moduleManager->AddModule(new InputModule());
	moduleManager->AddModule(new MouseModule());

	// Player
	moduleManager->AddModule(new Windows::ViewModule(hInst));
	moduleManager->AddModule(new Windows::Directx12::GraphicsModule());

	/*// Test project 1
	moduleManager->AddModule(new TestModule());
	moduleManager->AddModule(new FpsLoggerModule());
	moduleManager->AddModule(new ShutdownModule(moduleManager));*/

	// Test project 2
	//moduleManager->AddModule(new AgentModule());
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