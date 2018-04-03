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
#include <Core/Rendering/StorageModule.hpp>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;

void StorageModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

const Storage* StorageModule::AllocateStorage(size_t size) const
{
    auto buffer = graphicsModule->AllocateBuffer(size);
    return new Storage(buffer);
}

SERIALIZE_METHOD_ARG1(StorageModule, CreateStorage, const Storage*);
SERIALIZE_METHOD_ARG2(StorageModule, SetUsage, const Storage*, BufferUsageFlags);
SERIALIZE_METHOD_ARG3(StorageModule, UpdateStorage, const Storage*, uint32, Range<void>&);
SERIALIZE_METHOD_ARG3(StorageModule, CopyStorage, const Storage*, const Storage*, size_t);

bool StorageModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateStorage, Storage*, target);
        graphicsModule->RecCreateIBuffer(context, target->buffer);
        storages.push_back(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetUsage, Storage*, target, BufferUsageFlags, usage);
        graphicsModule->RecSetBufferUsage(context, target->buffer, usage);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(UpdateStorage, Storage*, target, uint32, targetOffset, Range<void>, data);
        graphicsModule->RecUpdateBuffer(context, target->buffer, (void*)data.pointer, data.size);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(CopyStorage, Storage*, src, Storage*, dst, size_t, size);
        graphicsModule->RecCopyBuffer(context, src->buffer, dst->buffer, size);
        DESERIALIZE_METHOD_END;
    }
    return false;
}