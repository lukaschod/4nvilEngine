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

#include <Core/Input/MouseModule.hpp>
#include <Core/Input/InputModule.hpp>

using namespace Core;

Void MouseModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    inputModule = ExecuteAfter<InputModule>(moduleManager);
    device = nullptr;
}

Void MouseModule::Execute(const ExecutionContext& context)
{
    MARK_FUNCTION; 

    device = inputModule->TryFindInputDevice("Mouse");

    // If device is still not available simply skip everything
    if (device == nullptr)
        return;

    for (auto& state : buttonsState)
    {
        state.down = false;
        state.up = false;
    }

    // Examine all inputs and update the state of mouse
    auto& stream = device->inputStream;
    auto inputCount = device->inputCount;
    UInt offset = 0;
    for (UInt32 i = 0; i < inputCount; i++)
    {
        auto inputType = stream.FastRead<MouseInputType>(offset);
        switch (inputType)
        {

        case MouseInputType::Move:
        {
            auto& desc = stream.FastRead<MousePositionDesc>(offset);
            position = desc.position;
            break;
        }

        case MouseInputType::Button:
        {
            auto& desc = stream.FastRead<MouseButtonDesc>(offset);
            auto& state = buttonsState[Enum::ToUnderlying(desc.type)];
            auto isDown = desc.isDown;
            state.down = isDown;
            state.up = !isDown;
            state.click = isDown;
            break;
        }

        default:
            ERROR("Unknown input type");
        }
    }
}
