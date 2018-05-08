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
#include <Core/Foundation/PipeModule.hpp>
#include <Core/Graphics/IBuffer.hpp>

namespace Core::Graphics
{
    class IGraphicsModule;
}

namespace Core
{
    struct Storage
    {
        Storage(const Graphics::IBuffer* buffer)
            : buffer(buffer)
            , size(buffer->GetSize())
            , created(false)
        {
        }
        const Graphics::IBuffer* buffer;
        const UInt size;
        Bool created;
    };

    class StorageModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        virtual Void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        const Storage* AllocateStorage(UInt size) const;

        Void RecCreateStorage(const ExecutionContext& context, const Storage* storage);
        Void RecSetUsage(const ExecutionContext& context, const Storage* target, Graphics::BufferUsageFlags usage);
        Void RecUpdateStorage(const ExecutionContext& context, const Storage* target, UInt32 targetOffset, Range<Void>& data);
        Void RecCopyStorage(const ExecutionContext& context, const Storage* src, const Storage* dst, UInt size);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        List<Storage*> storages;
        Graphics::IGraphicsModule* graphicsModule;
    };
}