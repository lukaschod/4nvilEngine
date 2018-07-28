/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <Core/Tools/Common.hpp>
#include <Core/Foundation/UnitModule.hpp>

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
    struct MeshRenderer : Component
    {
        IMPLEMENT_TRANSFERABLE(Core, MeshRenderer);

        MeshRenderer()
            : mesh(nullptr)
            , material(nullptr)
            , created(false)
        {
        }
        const Mesh* mesh;
        const Material* material;
        const Storage* perMeshStorage;
        Bool created;
    };

    class MeshRendererModule : public ComponentModule
    {
    public:
        IMPLEMENT_TRANSFERER(Core, MeshRenderer);
        BASE_IS(ComponentModule);

        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        CORE_API virtual Void Execute(const ExecutionContext& context) override;
        CORE_API const MeshRenderer* AllocateMeshRenderer();

        // Returns all mesh renderers that currently allocated
        inline const List<MeshRenderer*>& GetMeshRenderers() const { return meshRenderers; }

        // Returns storage that will have camera data
        inline const Storage* GetPerAllRendererStorage() const { return perAllRendererStorage; }

    public:
        CORE_API virtual Void RecDestroy(const ExecutionContext& context, const Component* target) override;

        CORE_API Void RecCreateMeshRenderer(const ExecutionContext& context, const MeshRenderer*);

        // Set mesh that will be used for rendering
        CORE_API Void RecSetMesh(const ExecutionContext& context, const MeshRenderer* target, const Mesh* mesh);

        // Set material that will be used for rendering
        CORE_API Void RecSetMaterial(const ExecutionContext& context, const MeshRenderer* target, const Material* material);

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        List<MeshRenderer*> meshRenderers;
        MaterialModule* materialModule;
        MeshModule* meshModule;
        StorageModule* storageModule;
        TransformModule* transformModule;
        MemoryModule* memoryModule;
        const Storage* perAllRendererStorage;
    };
}