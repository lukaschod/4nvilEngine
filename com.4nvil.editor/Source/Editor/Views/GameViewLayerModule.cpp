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

#include <Editor/Views/GameViewLayerModule.hpp>
#include <Core/Rendering/RenderLoop/UnlitRenderLoopModule.hpp>
#include <Core/Rendering/CameraModule.hpp>

using namespace Editor;

IRenderLoopModule* GameViewLayerModule::GetRenderLoop(ModuleManager * moduleManager)
{
    ExecuteBefore<CameraModule>(moduleManager); // TODO: Temp fix remove it
    return ExecuteBefore<UnlitRenderLoopModule>(moduleManager);
}
