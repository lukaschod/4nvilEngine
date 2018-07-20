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

#include <Core/Graphics/IBuffer.hpp>
#include <Core/Foundation/TransfererUtility.hpp>
#include <Core/Rendering/MeshModule.hpp>
#include <Core/Rendering/StorageModule.hpp>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;

template<> inline Void Core::TransferValue(ITransfer* transfer, const Char* name, Graphics::VertexLayout*& vertexLayout)
{
    TRANSFER(vertexLayout->attributes);
    TRANSFER(vertexLayout->stride);
}

Void Mesh::Transfer(ITransfer* transfer)
{
    TRANSFER(vertexBuffer);
    TRANSFER(subMeshes);
}

Void MeshModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    storageModule = ExecuteBefore<StorageModule>(moduleManager);
}

const Mesh* MeshModule::AllocateMesh() const
{
    return new Mesh();
}

SERIALIZE_METHOD_ARG1(MeshModule, CreateMesh, const Mesh*);
SERIALIZE_METHOD_ARG1(MeshModule, Destroy, const Mesh*);
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

        DESERIALIZE_METHOD_ARG1_START(Destroy, Mesh*, target);
        NOT_IMPLEMENTED();
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetVertices, Mesh*, target, Range<UInt8>, vertices);
        ASSERT(target->created);

        target->vertexBuffer = storageModule->AllocateStorage();
        storageModule->RecSetSize(context, target->vertexBuffer, vertices.size);
        storageModule->RecCreateStorage(context, target->vertexBuffer);
        storageModule->RecUpdateStorage(context, target->vertexBuffer, 0, Range<Void>((Void*) vertices.pointer, vertices.size));
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(SetSubMesh, Mesh*, target, UInt32, index, SubMesh, submesh);
        ASSERT(target->created);
        target->subMeshes.safe_set(index, submesh);
        DESERIALIZE_METHOD_END;
    }
    return false;
}