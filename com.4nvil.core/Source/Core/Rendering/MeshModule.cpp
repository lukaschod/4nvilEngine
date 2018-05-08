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

#include <Core/Graphics/IGraphicsModule.hpp>
#include <Core/Graphics/IBuffer.hpp>
#include <Core/Rendering/MeshModule.hpp>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;

Void MeshModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

const Mesh* MeshModule::AllocateMesh(const Graphics::VertexLayout& vertexLayout) const
{
    return new Mesh(vertexLayout);
}

DECLARE_COMMAND_CODE(CreateMesh);
const Mesh* MeshModule::RecCreateMesh(const ExecutionContext& context, const VertexLayout& vertexLayout)
{
    auto buffer = GetRecordingBuffer(context);
    auto& stream = buffer->stream;
    auto target = new Mesh(vertexLayout);
    stream.Write(TO_COMMAND_CODE(CreateMesh));
    stream.Write(target);
    stream.Align();
    buffer->commandCount++;
    return target;
}

SERIALIZE_METHOD_ARG2(MeshModule, SetVertices, const Mesh*, const Range<UInt8>&);
SERIALIZE_METHOD_ARG3(MeshModule, SetSubMesh, const Mesh*, UInt32, const SubMesh&);

Bool MeshModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateMesh, Mesh*, target);
        target->created = true;
        meshes.push_back(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetVertices, Mesh*, target, Range<UInt8>, vertices);
        ASSERT(target->created);
        target->vertices = vertices;

        target->vertexBuffer = graphicsModule->AllocateBuffer(vertices.size);
        graphicsModule->RecCreateIBuffer(context, target->vertexBuffer);
        graphicsModule->RecUpdateBuffer(context, target->vertexBuffer, (Void*) target->vertices.pointer, target->vertices.size);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(SetSubMesh, Mesh*, target, UInt32, index, SubMesh, submesh);
        ASSERT(target->created);
        target->subMeshes.safe_set(index, submesh);
        DESERIALIZE_METHOD_END;
    }
    return false;
}