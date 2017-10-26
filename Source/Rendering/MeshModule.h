#pragma once

#include <Common\EngineCommon.h>
#include <Modules\CmdModule.h>
#include <Common\Collections\List.h>
#include <Graphics\IGraphicsModule.h>
#include <Graphics\VertexLayout.h>
#include <Graphics\IBuffer.h>

enum MeshTopology
{
	MeshTopologyTriangles,
};

struct SubMesh
{
	SubMesh() {}
	SubMesh(uint32_t offset, uint32_t size, MeshTopology topology) : offset(offset), size(size), topology(topology) {}
	SubMesh(uint32_t size) : SubMesh(0, size, MeshTopologyTriangles) {}
	uint32_t offset;
	uint32_t size;
	MeshTopology topology;
};

struct Mesh
{
	Mesh(const VertexLayout& vertexLayout) : 
		vertexLayout(vertexLayout), 
		vertexBuffer(nullptr) 
	{}
	const VertexLayout vertexLayout;
	const IBuffer* vertexBuffer;
	List<SubMesh> subMeshes;
	Range<uint8_t> vertices;
};

class MeshModule : public CmdModule
{
public:
	MeshModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	const Mesh* RecCreateMesh(const ExecutionContext& context, const VertexLayout& vertexLayout);
	void RecSetVertices(const ExecutionContext& context, const Mesh* target, const Range<uint8_t>& vertices);
	void RecSetSubMesh(const ExecutionContext& context, const Mesh* target, uint32_t index, const SubMesh& submesh);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) override;

private:
	List<Mesh*> meshes;
	IGraphicsModule* graphicsModule;
};