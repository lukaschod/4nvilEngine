#include <Core\Foundation\TransformModule.h>
#include <Core\Foundation\MemoryModule.h>
#include <Core\Graphics\IGraphicsModule.h>
#include <Core\Rendering\MeshRendererModule.h>
#include <Core\Rendering\MeshModule.h>
#include <Core\Rendering\MaterialModule.h>
#include <Core\Rendering\StorageModule.h>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;

static const char* memoryLabelMeshRenderer = "Core::MeshRenderer";

MeshRendererModule::MeshRendererModule()
	: perAllRendererStorage(nullptr)
{
}

void MeshRendererModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	meshModule = ExecuteBefore<MeshModule>(moduleManager);
	materialModule = ExecuteBefore<MaterialModule>(moduleManager);
	storageModule = ExecuteBefore<StorageModule>(moduleManager);
	transformModule = ExecuteBefore<TransformModule>(moduleManager);
	unitModule = ExecuteAfter<UnitModule>(moduleManager);
	memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
	memoryModule->SetAllocator(memoryLabelMeshRenderer, new FixedBlockHeap(sizeof(MeshRenderer)));
	ExecuteBefore<IGraphicsModule>(moduleManager);
}

void MeshRendererModule::Execute(const ExecutionContext& context)
{
	MARK_FUNCTION;

	if (perAllRendererStorage == nullptr)
	{
		perAllRendererStorage = storageModule->AllocateStorage(sizeof(Matrix4x4f));
		storageModule->RecSetUsage(context, perAllRendererStorage, BufferUsageFlags::Shader | BufferUsageFlags::GpuOnly); // Only updated by cameras
		storageModule->RecCreateStorage(context, perAllRendererStorage);
	}

	base::Execute(context);

	for (auto meshRenderer : meshRenderers)
	{
		auto transform = unitModule->GetComponent<Transform>(meshRenderer);
		if (transform->flags.Contains(TransformStateFlags::LocalObjectToWorldChanged))
			storageModule->RecUpdateStorage(context, meshRenderer->perMeshStorage, 0, Range<void>(&transform->objectToWorld, sizeof(Matrix4x4f)));
	}
}

const MeshRenderer* MeshRendererModule::AllocateMeshRenderer()
{
	auto target = memoryModule->New<MeshRenderer>(memoryLabelMeshRenderer, this);
	target->perMeshStorage = storageModule->AllocateStorage(sizeof(Matrix4x4f));
	return target;
}

const List<MeshRenderer*>& MeshRendererModule::GetMeshRenderers() const { return meshRenderers; }
const Storage* MeshRendererModule::GetPerAllRendererStorage() const { return perAllRendererStorage; }

SERIALIZE_METHOD_ARG1(MeshRendererModule, Destroy, const Component*);
SERIALIZE_METHOD_ARG2(MeshRendererModule, SetMesh, const MeshRenderer*, const Mesh*);
SERIALIZE_METHOD_ARG2(MeshRendererModule, SetMaterial, const MeshRenderer*, const Material*);
SERIALIZE_METHOD_ARG1(MeshRendererModule, CreateMeshRenderer, const MeshRenderer*);

bool MeshRendererModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateMeshRenderer, MeshRenderer*, target);
		target->created = true;
		storageModule->RecCreateStorage(context, target->perMeshStorage);
		meshRenderers.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetMesh, MeshRenderer*, target, const Mesh*, mesh);
		target->mesh = mesh;
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetMaterial, MeshRenderer*, target, const Material*, material);
		target->material = material;
		materialModule->RecSetStorage(context, target->material, "_perCameraData", perAllRendererStorage);
		materialModule->RecSetStorage(context, target->material, "_perMeshData", target->perMeshStorage);
		DESERIALIZE_METHOD_END;
	}
	return false;
}
