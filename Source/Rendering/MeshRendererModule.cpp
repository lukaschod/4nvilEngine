#include <Rendering\MeshRendererModule.h>
#include <Foundation\TransformModule.h>
#include <Foundation\MemoryModule.h>
#include <Graphics\IGraphicsModule.h>

MeshRendererModule::MeshRendererModule()
	: perAllRendererStorage(nullptr)
{
}

void MeshRendererModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	PipeModule::SetupExecuteOrder(moduleManager);
	meshModule = ExecuteBefore<MeshModule>(moduleManager);
	materialModule = ExecuteBefore<MaterialModule>(moduleManager);
	storageModule = ExecuteBefore<StorageModule>(moduleManager);
	transformModule = ExecuteBefore<TransformModule>(moduleManager);
	unitModule = ExecuteAfter<UnitModule>(moduleManager);
	memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
	memoryModule->SetAllocator(3, new FixedBlockHeap(sizeof(MeshRenderer)));
	ExecuteBefore<IGraphicsModule>(moduleManager);
}

void MeshRendererModule::Execute(const ExecutionContext& context)
{
	PROFILE_FUNCTION;
	if (perAllRendererStorage == nullptr)
		perAllRendererStorage = storageModule->RecCreateStorage(context, sizeof(Matrix4x4f));

	PipeModule::Execute(context);

	for (auto meshRenderer : meshRenderers)
	{
		auto transform = unitModule->GetComponent<Transform>(meshRenderer);
		storageModule->RecUpdateStorage(context, meshRenderer->perMeshStorage, 0, Range<void>(&transform->objectToWorld, sizeof(Matrix4x4f)));
	}
}

const List<MeshRenderer*>& MeshRendererModule::GetMeshRenderers() const { return meshRenderers; }
const Storage * MeshRendererModule::GetPerAllRendererStorage() const { return perAllRendererStorage; }

SERIALIZE_METHOD_ARG2(MeshRendererModule, SetMesh, const MeshRenderer*, const Mesh*);
SERIALIZE_METHOD_ARG2(MeshRendererModule, SetMaterial, const MeshRenderer*, const Material*);
SERIALIZE_METHOD_ARG1(MeshRendererModule, Destroy, const Component*);

DECLARE_COMMAND_CODE(CreateMeshRenderer);
const MeshRenderer* MeshRendererModule::RecCreateMeshRenderer(const ExecutionContext& context)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto target = memoryModule->New<MeshRenderer>(3, this);
	stream.Write(CommandCodeCreateMeshRenderer);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}
//SERIALIZE_METHOD_CREATECMP(MeshRendererModule, MeshRenderer);

bool MeshRendererModule::ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateMeshRenderer, MeshRenderer*, target);
		target->perMeshStorage = storageModule->RecCreateStorage(context, sizeof(Matrix4x4f));
		meshRenderers.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetMesh, MeshRenderer*, target, const Mesh*, mesh);
		target->mesh = mesh;
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetMaterial, MeshRenderer*, target, const Material*, material);
		target->material = material;
		materialModule->RecSetStorage(context, target->material, "_perCameraData", perAllRendererStorage);
		DESERIALIZE_METHOD_END;
	}
	return false;
}
