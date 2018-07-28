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
#include <Core/Foundation/TransfererModule.hpp>
#include <Core/Graphics/IBuffer.hpp>

namespace Core::Graphics
{
    class IGraphicsModule;
}

namespace Core
{
    struct Storage : Transferable
    {
        IMPLEMENT_TRANSFERABLE(Core, Storage);
        Storage()
            : buffer(nullptr)
            , usage(Graphics::BufferUsageFlags::CpuToGpu)
            , data(nullptr)
            , size(128)
            , created(false)
        {
        }
        const Graphics::IBuffer* buffer;
        Graphics::BufferUsageFlags usage;
        UInt8* data;
        UInt size;
        Bool created;
    };

    class StorageModule : public TransfererModule
    {
    public:
        IMPLEMENT_TRANSFERER(Core, Storage);
        BASE_IS(PipeModule);

        CORE_API virtual Void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        CORE_API const Storage* AllocateStorage() const;

    public:
        CORE_API Void RecCreateStorage(const ExecutionContext& context, const Storage* storage);
        CORE_API Void RecDestroy(const ExecutionContext& context, const Storage* storage);
        CORE_API Void RecSetSize(const ExecutionContext& context, const Storage* target, UInt size);
        CORE_API Void RecSetUsage(const ExecutionContext& context, const Storage* target, Graphics::BufferUsageFlags usage);
        CORE_API Void RecUpdateStorage(const ExecutionContext& context, const Storage* target, UInt32 targetOffset, Range<Void>& data);
        CORE_API Void RecCopyStorage(const ExecutionContext& context, const Storage* src, const Storage* dst, UInt size);

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        List<Storage*> storages;
        Graphics::IGraphicsModule* graphicsModule;
    };
}