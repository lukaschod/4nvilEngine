#pragma once

#include <Tools\Common.h>
#include <Foundation\UnitModule.h>

namespace Core
{
	class TransformModule; struct Transform;
	class MemoryModule;
}

namespace Core
{
	class StorageModule; struct Storage;
	class MaterialModule; struct Material;
	class MeshModule; struct Mesh;

	struct MeshRenderer : public Component
	{
		MeshRenderer(ComponentModule* module)
			: Component(module)
			, mesh(nullptr)
			, material(nullptr)
		{
		}
		const Mesh* mesh;
		const Material* material;
		const Storage* perMeshStorage;
	};

	class MeshRendererModule : public ComponentModule
	{
	public:
		MeshRendererModule();
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
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

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
}