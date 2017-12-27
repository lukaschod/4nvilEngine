#pragma once

#include <Tools\Common.h>
#include <Foundation\UnitModule.h>
#include <Rendering\MeshModule.h>
#include <Rendering\MaterialModule.h>

class TransformModule; struct Transform;
class StorageModule; struct Storage;
class MeshRendererModule;
class MemoryModule;

struct MeshRenderer : public Component
{
	MeshRenderer(MeshRendererModule* module) : 
		Component((ComponentModule*)module), 
		mesh(nullptr), 
		material(nullptr) 
	{}
	const Mesh* mesh;
	const Material* material;
	const Storage* perMeshStorage;
};

class MeshRendererModule : public ComponentModule
{
public:
	MeshRendererModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual void Execute(const ExecutionContext& context) override;
	virtual void RecDestroy(const ExecutionContext& context, const Component* target) override;
	const MeshRenderer* RecCreateMeshRenderer(const ExecutionContext& context);
	void RecSetMesh(const ExecutionContext& context, const MeshRenderer* target, const Mesh* mesh);
	void RecSetMaterial(const ExecutionContext& context, const MeshRenderer* target, const Material* material);

public:
	const List<MeshRenderer*>& GetMeshRenderers() const;
	const Storage* GetPerAllRendererStorage() const;

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode) override;

private:
	List<MeshRenderer*> meshRenderers;
	MaterialModule* materialModule;
	MeshModule* meshModule;
	StorageModule* storageModule;
	TransformModule* transformModule;
	UnitModule* unitModule;
	MemoryModule* memoryModule;
	const Storage* perAllRendererStorage;
};