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
#include <Core/Tools/Collections/List.hpp>
#include <Core/Foundation/TransfererModule.hpp>
#include <Core/Graphics/VertexLayout.hpp>

namespace Core
{
    struct Storage;
    class StorageModule;
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
        SubMesh(UInt32 offset, UInt32 size, MeshTopology topology) : offset(offset), size(size), topology(topology) {}
        SubMesh(UInt32 size) : SubMesh(0, size, MeshTopology::Triangles) {}
        UInt32 offset;
        UInt32 size;
        MeshTopology topology;
    };

    struct Mesh : Transferable
    {
        IMPLEMENT_TRANSFERABLE(Core, Mesh);
        Mesh()
            : vertexBuffer(nullptr)
            , created(false)
        {
        }
        const Storage* vertexBuffer;
        List<SubMesh> subMeshes;
        Bool created;
    };

    class MeshModule : public TransfererModule
    {
    public:
        IMPLEMENT_TRANSFERER(Core, Mesh);
        BASE_IS(PipeModule);

        CORE_API virtual Void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        CORE_API const Mesh* AllocateMesh() const;

    public:
        CORE_API Void RecCreateMesh(const ExecutionContext& context, const Mesh* target);
        CORE_API Void RecDestroy(const ExecutionContext& context, const Mesh* target);
        CORE_API Void RecSetVertices(const ExecutionContext& context, const Mesh* target, const Range<UInt8>& vertices);
        CORE_API Void RecSetSubMesh(const ExecutionContext& context, const Mesh* target, UInt32 index, const SubMesh& submesh);

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        List<Mesh*> meshes;
        StorageModule* storageModule;
    };
}