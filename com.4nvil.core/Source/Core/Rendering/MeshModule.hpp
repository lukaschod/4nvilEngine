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
#include <Core/Foundation/PipeModule.hpp>
#include <Core/Graphics/VertexLayout.hpp>

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
        SubMesh(UInt32 offset, UInt32 size, MeshTopology topology) : offset(offset), size(size), topology(topology) {}
        SubMesh(UInt32 size) : SubMesh(0, size, MeshTopology::Triangles) {}
        UInt32 offset;
        UInt32 size;
        MeshTopology topology;
    };

    struct Mesh
    {
        Mesh(const Graphics::VertexLayout& vertexLayout)
            : vertexLayout(vertexLayout)
            , vertexBuffer(nullptr)
            , created(false)
        {
        }
        const Graphics::VertexLayout vertexLayout;
        const Graphics::IBuffer* vertexBuffer;
        List<SubMesh> subMeshes;
        Range<UInt8> vertices;
        Bool created;
    };

    class MeshModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        virtual Void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        const Mesh* AllocateMesh(const Graphics::VertexLayout& vertexLayout) const;

    public:
        Void RecCreateMesh(const ExecutionContext& context, const Mesh* target);
        Void RecSetVertices(const ExecutionContext& context, const Mesh* target, const Range<UInt8>& vertices);
        Void RecSetSubMesh(const ExecutionContext& context, const Mesh* target, UInt32 index, const SubMesh& submesh);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        List<Mesh*> meshes;
        Graphics::IGraphicsModule* graphicsModule;
    };
}