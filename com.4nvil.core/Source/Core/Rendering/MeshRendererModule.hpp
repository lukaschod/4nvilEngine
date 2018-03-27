#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Foundation\UnitModule.hpp>

namespace Core
{
	struct Transform;
	struct Storage;
	struct Material;
	struct Mesh;
	class TransformModule;
	class MemoryModule;
	class StorageModule;
	class MaterialModule;
	class MeshModule; 
}

namespace Core
{
	struct MeshRenderer : public Component
	{
		MeshRenderer(ComponentModule* module)
			: Component(module)
			, mesh(nullptr)
			, material(nullptr)
			, created(false)
		{
		}
		const Mesh* mesh;
		const Material* material;
		const Storage* perMeshStorage;
		bool created;
	};

	class MeshRendererModule : public ComponentModule
	{
	public:
		BASE_IS(ComponentModule);

		MeshRendererModule();
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual void Execute(const ExecutionContext& context) override;
		const MeshRenderer* AllocateMeshRenderer();

		// Returns all mesh renderers that currently allocated
		const List<MeshRenderer*>& GetMeshRenderers() const;

		// Returns storage that will have camera data
		const Storage* GetPerAllRendererStorage() const;

	public:
		virtual void RecDestroy(const ExecutionContext& context, const Component* target) override;

		void RecCreateMeshRenderer(const ExecutionContext& context, const MeshRenderer*);

		// Set mesh that will be used for rendering
		void RecSetMesh(const ExecutionContext& context, const MeshRenderer* target, const Mesh* mesh);

		// Set material that will be used for rendering
		void RecSetMaterial(const ExecutionContext& context, const MeshRenderer* target, const Material* material);

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