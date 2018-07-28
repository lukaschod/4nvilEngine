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

#include <Core/Foundation/TransformModule.hpp>
#include <Core/Foundation/MemoryModule.hpp>
#include <Core/Graphics/IGraphicsModule.hpp>
#include <Core/Rendering/MeshRendererModule.hpp>
#include <Core/Rendering/MeshModule.hpp>
#include <Core/Rendering/MaterialModule.hpp>
#include <Core/Rendering/StorageModule.hpp>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;

static const Char* memoryLabelMeshRenderer = "Core::MeshRenderer";

Void MeshRenderer::Transfer(ITransfer* transfer)
{
    TRANSFER(unit);
    TRANSFER(mesh);
    if (transfer->IsReading())
        created = false;
}

Void MeshRendererModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    meshModule = ExecuteBefore<MeshModule>(moduleManager);
    materialModule = ExecuteBefore<MaterialModule>(moduleManager);
    storageModule = ExecuteBefore<StorageModule>(moduleManager);
    transformModule = ExecuteBefore<TransformModule>(moduleManager);
    memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
    memoryModule->SetAllocator(memoryLabelMeshRenderer, new FixedBlockHeap(sizeof(MeshRenderer)));
    ExecuteBefore<IGraphicsModule>(moduleManager);
    perAllRendererStorage = nullptr;
}

Void MeshRendererModule::Execute(const ExecutionContext& context)
{
    MARK_FUNCTION;

    if (perAllRendererStorage == nullptr)
    {
        perAllRendererStorage = storageModule->AllocateStorage();
        storageModule->RecSetSize(context, perAllRendererStorage, sizeof(Matrix4x4f));
        storageModule->RecSetUsage(context, perAllRendererStorage, BufferUsageFlags::Shader | BufferUsageFlags::GpuOnly); // Only updated by cameras
        storageModule->RecCreateStorage(context, perAllRendererStorage);
    }

    base::Execute(context);

    for (auto meshRenderer : meshRenderers)
    {
        auto transform = unitModule->GetComponent<Transform>(meshRenderer);
        if (Enum::Contains(transform->flags, TransformStateFlags::LocalObjectToWorldChanged))
            storageModule->RecUpdateStorage(context, meshRenderer->perMeshStorage, 0, Range<Void>(&transform->objectToWorld, sizeof(Matrix4x4f)));
    }
}

const MeshRenderer* MeshRendererModule::AllocateMeshRenderer()
{
    auto target = memoryModule->New<MeshRenderer>(memoryLabelMeshRenderer);
    target->perMeshStorage = storageModule->AllocateStorage();
    return target;
}

SERIALIZE_METHOD_ARG1(MeshRendererModule, Destroy, const Component*);
SERIALIZE_METHOD_ARG2(MeshRendererModule, SetMesh, const MeshRenderer*, const Mesh*);
SERIALIZE_METHOD_ARG2(MeshRendererModule, SetMaterial, const MeshRenderer*, const Material*);
SERIALIZE_METHOD_ARG1(MeshRendererModule, CreateMeshRenderer, const MeshRenderer*);

Bool MeshRendererModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateMeshRenderer, MeshRenderer*, target);
        ASSERT(!target->created);
        target->created = true;
        storageModule->RecSetSize(context, target->perMeshStorage, sizeof(Matrix4x4f));
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
