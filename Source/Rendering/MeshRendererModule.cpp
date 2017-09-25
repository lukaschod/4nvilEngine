#include <Rendering\MeshRendererModule.h>
#include <Transforms\TransformModule.h>
#include <Graphics\IGraphicsModule.h>

MeshRendererModule::MeshRendererModule(uint32_t bufferCount, uint32_t bufferIndexStep) :
	ComponentModule(bufferCount, bufferIndexStep)
{
}

void MeshRendererModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	CmdModule::SetupExecuteOrder(moduleManager);
	meshModule = ExecuteBefore<MeshModule>(moduleManager);
	materialModule = ExecuteBefore<MaterialModule>(moduleManager);
	storageModule = ExecuteBefore<StorageModule>(moduleManager);
	transformModule = ExecuteBefore<TransformModule>(moduleManager);
	unitModule = ExecuteAfter<UnitModule>(moduleManager);
}

void MeshRendererModule::Execute(const ExecutionContext& context)
{
	CmdModule::Execute(context);

	for (auto meshRenderer : meshRenderers)
	{
		auto transform = unitModule->GetComponent<Transform>(meshRenderer);
		storageModule->RecordUpdateStorage(context, meshRenderer->perMeshStorage, 0, Range<void>(&transform->objectToWorld, sizeof(Matrix4x4f)));
	}
}

const List<MeshRenderer*>& MeshRendererModule::GetMeshRenderers() { return meshRenderers; }

SERIALIZE_METHOD_CREATECMP(MeshRendererModule, MeshRenderer);
SERIALIZE_METHOD_ARG2(MeshRendererModule, SetMesh, const MeshRenderer*, const Mesh*);
SERIALIZE_METHOD_ARG2(MeshRendererModule, SetMaterial, const MeshRenderer*, const Material*);
SERIALIZE_METHOD_ARG1(MeshRendererModule, Destroy, const Component*);

bool MeshRendererModule::ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateMeshRenderer, MeshRenderer*, target);
		target->perMeshStorage = storageModule->RecordCreateStorage(context, sizeof(Matrix4x4f));
		meshRenderers.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetMesh, MeshRenderer*, target, const Mesh*, mesh);
		target->mesh = mesh;
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetMaterial, MeshRenderer*, target, const Material*, material);
		target->material = material;
		DESERIALIZE_METHOD_END;
	}
	return false;
}
