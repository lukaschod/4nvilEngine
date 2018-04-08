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
        MemoryModule* memoryModule;
        const Storage* perAllRendererStorage;
    };
}