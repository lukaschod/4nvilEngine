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

Void Storage::Transfer(ITransfer* transfer)
{
    TRANSFER(usage);
    TRANSFER(size);
    if (transfer->IsReading())
        data = new UInt8[size];
    transfer->Transfer(data, size);
}

Void StorageModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

const Storage* StorageModule::AllocateStorage() const
{
    auto storage = new Storage();
    return storage;
}

SERIALIZE_METHOD_ARG1(StorageModule, CreateStorage, const Storage*);
SERIALIZE_METHOD_ARG1(StorageModule, Destroy, const Storage*);
SERIALIZE_METHOD_ARG2(StorageModule, SetUsage, const Storage*, BufferUsageFlags);
SERIALIZE_METHOD_ARG2(StorageModule, SetSize, const Storage*, UInt);
SERIALIZE_METHOD_ARG3(StorageModule, UpdateStorage, const Storage*, UInt32, Range<Void>&);
SERIALIZE_METHOD_ARG3(StorageModule, CopyStorage, const Storage*, const Storage*, UInt);

Bool StorageModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateStorage, Storage*, target);
        ASSERT(!target->created);
        ASSERT(target->size != 0);
        if (target->data == nullptr)
            target->data = new UInt8[target->size];
        target->buffer = graphicsModule->AllocateBuffer(target->size);
        graphicsModule->RecSetBufferUsage(context, target->buffer, target->usage);
        graphicsModule->RecCreateBuffer(context, target->buffer);
        storages.push_back(target);
        target->created = true;
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(Destroy, Storage*, target);
        NOT_IMPLEMENTED();
        DESERIALIZE_METHOD_END

        DESERIALIZE_METHOD_ARG2_START(SetUsage, Storage*, target, BufferUsageFlags, usage);
        ASSERT(!target->created);
        target->usage = usage;
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetSize, Storage*, target, UInt, size);
        ASSERT(!target->created);
        target->size = size;
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(UpdateStorage, Storage*, target, UInt32, targetOffset, Range<Void>, data);
        ASSERT(target->created);
        memcpy(target->data, data.pointer, data.size);
        graphicsModule->RecUpdateBuffer(context, target->buffer, (Void*)data.pointer, data.size);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(CopyStorage, Storage*, src, Storage*, dst, UInt, size);
        ASSERT(src->created && dst->created);
        memcpy(dst->data, src->data, size);
        graphicsModule->RecCopyBuffer(context, src->buffer, dst->buffer, size);
        DESERIALIZE_METHOD_END;
    }
    return false;
}