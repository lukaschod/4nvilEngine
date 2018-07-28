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
#include <Core/Tools/IO/MemoryStream.hpp>
#include <Core/Tools/Collections/List.hpp>

namespace Core
{
    typedef UInt32 InputType;

    struct InputDeviceDesc
    {
        const Char* typeName;
        const Char* vendorName;
    };

    struct InputDevice
    {
        InputDeviceDesc desc;
        IO::MemoryStream inputStream;
        UInt32 inputCount;
    };

    class InputModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        CORE_API virtual Void Execute(const ExecutionContext& context) override;

        // Find input devices by the type name
        CORE_API const InputDevice* TryFindInputDevice(const Char* typeName) const;

        // Record abstract input
        CORE_API Void RecInput(const ExecutionContext& context, const InputDevice* device, InputType inputType, UInt8* data, UInt size);

        // Create new input device, type name must be unique
        CORE_API const InputDevice* RecCreateInputDevice(const ExecutionContext& context, const InputDeviceDesc& desc);

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        // Remove all previous inputs
        Void Reset();

    private:
        List<InputDevice*> devices;
    };
}