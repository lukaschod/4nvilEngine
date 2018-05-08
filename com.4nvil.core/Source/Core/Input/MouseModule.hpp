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
#include <Core/Tools/Math/Vector.hpp>
#include <Core/Tools/Enum.hpp>
#include <Core/Foundation/ComputeModule.hpp>
#include <Core/Input/MouseInputs.hpp>

namespace Core
{
    class InputModule;
    struct InputDevice;

    class MouseModule : public ComputeModule
    {
    public:
        BASE_IS(ComputeModule);

        MouseModule();
        virtual Void Execute(const ExecutionContext& context) override;
        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;

        // Checks if mouse is detected
        Bool IsPresent() const { return device != nullptr; }

        // Returns the specific button state
        Bool GetButton(MouseButtonType type) const { return buttonsState[Enum::ToUnderlying(type)].click; }

        // Returns the specific button state
        Bool GetButtonUp(MouseButtonType type) const { return buttonsState[Enum::ToUnderlying(type)].up; }

        // Returns the specific button state
        Bool GetButtonDown(MouseButtonType type) const { return buttonsState[Enum::ToUnderlying(type)].down; }

        // Returns mouse position in screen space
        Math::Vector2f GetPosition() const { return position; }

    private:
        InputModule* inputModule;
        const InputDevice* device;
        MouseButtonState buttonsState[Enum::ToUnderlying(MouseButtonType::Count)];

        Math::Vector2f position;
    };
}