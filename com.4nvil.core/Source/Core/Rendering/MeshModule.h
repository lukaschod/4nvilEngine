#pragma once

#include <Core\Tools\Common.h>
#include <Core\Tools\Collections\List.h>
#include <Core\Foundation\PipeModule.h>
#include <Core\Graphics\VertexLayout.h>

namespace Core::Graphics
{
	struct IBuffer;
	class IGraphicsModule;
}

namespace Core
{
	enum class MeshTopology
	{
		Triangles,
	};

	struct SubMesh
	{
		SubMesh() {}
		SubMesh(uint32 offset, uint32 size, MeshTopology topology) : offset(offset), size(size), topology(topology) {}
		SubMesh(uint32 size) : SubMesh(0, size, MeshTopology::Triangles) {}
		uint32 offset;
		uint32 size;
		MeshTopology topology;
	};

	struct Mesh
	{
		Mesh(const Graphics::VertexLayout& vertexLayout)
			: vertexLayout(vertexLayout)
			, vertexBuffer(nullptr)
		{
		}
		const Graphics::VertexLayout vertexLayout;
		const Graphics::IBuffer* vertexBuffer;
		List<SubMesh> subMeshes;
		Range<uint8> vertices;
	};

	class MeshModule : public PipeModule
	{
	public:
		virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;

		const Mesh* RecCreateMesh(const ExecutionContext& context, const Graphics::VertexLayout& vertexLayout);
		void RecSetVertices(const ExecutionContext& context, const Mesh* target, const Range<uint8>& vertices);
		void RecSetSubMesh(const ExecutionContext& context, const Mesh* target, uint32 index, const SubMesh& submesh);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<Mesh*> meshes;
		Graphics::IGraphicsModule* graphicsModule;
	};
}