#include <Core\Graphics\IGraphicsModule.h>
#include <Core\Graphics\IBuffer.h>
#include <Core\Rendering\MeshModule.h>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;

void MeshModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

const Mesh* MeshModule::AllocateMesh(const Graphics::VertexLayout& vertexLayout) const
{
	return new Mesh(vertexLayout);
}

DECLARE_COMMAND_CODE(CreateMesh);
const Mesh* MeshModule::RecCreateMesh(const ExecutionContext& context, const VertexLayout& vertexLayout)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto target = new Mesh(vertexLayout);
	stream.Write(TO_COMMAND_CODE(CreateMesh));
	stream.Write(target);
	stream.Align();
	buffer->commandCount++;
	return target;
}

SERIALIZE_METHOD_ARG2(MeshModule, SetVertices, const Mesh*, const Range<uint8>&);
SERIALIZE_METHOD_ARG3(MeshModule, SetSubMesh, const Mesh*, uint32, const SubMesh&);

bool MeshModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateMesh, Mesh*, target);
		target->created = true;
		meshes.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetVertices, Mesh*, target, Range<uint8>, vertices);
		ASSERT(target->created);
		target->vertices = vertices;

		target->vertexBuffer = graphicsModule->AllocateBuffer(vertices.size);
		graphicsModule->RecCreateIBuffer(context, target->vertexBuffer);
		graphicsModule->RecUpdateBuffer(context, target->vertexBuffer, (void*) target->vertices.pointer, target->vertices.size);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetSubMesh, Mesh*, target, uint32, index, SubMesh, submesh);
		ASSERT(target->created);
		target->subMeshes.safe_set(index, submesh);
		DESERIALIZE_METHOD_END;
	}
	return false;
}