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

#include <Core/Tools/Character.hpp>
#include <Core/Input/InputModule.hpp>

using namespace Core;

Void InputModule::Execute(const ExecutionContext& context)
{
    MARK_FUNCTION;
    Reset();
    base::Execute(context);
}

const InputDevice* InputModule::TryFindInputDevice(const Char* typeName) const
{
    for (auto device : devices)
        if (Character::Equals(device->desc.typeName, typeName))
            return device;
    return nullptr;
}

DECLARE_COMMAND_CODE(Input);
Void InputModule::RecInput(const ExecutionContext& context, const InputDevice* device, InputType inputType, UInt8* data, UInt size)
{
    auto buffer = GetRecordingBuffer(context);
    auto& stream = buffer->stream;
    stream.Write(TO_COMMAND_CODE(Input));
    stream.Write(device);
    stream.Write(inputType);
    stream.Write(size);
    if (size != 0)
        stream.Write(data, size);
    stream.Align();
    buffer->commandCount++;
}

DECLARE_COMMAND_CODE(CreateInputDevice);
const InputDevice* InputModule::RecCreateInputDevice(const ExecutionContext& context, const InputDeviceDesc& desc)
{
    auto buffer = GetRecordingBuffer(context);
    auto& stream = buffer->stream;
    stream.Write(TO_COMMAND_CODE(CreateInputDevice));
    auto device = new InputDevice();
    device->desc = desc;
    stream.Write(device);
    stream.Align();
    buffer->commandCount++;
    return device;
}

Bool InputModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_START(Input);
        auto device = stream.FastRead<InputDevice*>();
        auto inputType = stream.FastRead<InputType>();
        auto size = stream.FastRead<UInt>();
        auto pointer = stream.data;
        stream.data += size;
        device->inputStream.Write(inputType);
        if (size != 0)
            device->inputStream.Write(pointer, size);
        device->inputCount++;
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(CreateInputDevice, InputDevice*, device);
        devices.push_back(device);
        DESERIALIZE_METHOD_END;
    }
    return false;
}

Void InputModule::Reset()
{
    for (auto device : devices)
    {
        device->inputStream.Reset();
        device->inputCount = 0;
    }
}