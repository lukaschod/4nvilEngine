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

namespace Core::Graphics
{
    struct ITexture;
    struct ISwapChain;
}

namespace Core
{
    class IRenderLoopModule : public PipeModule 
    {
    public:
        virtual Void RecRender(const ExecutionContext& context, const Graphics::ISwapChain* swapChain, const Graphics::ITexture* renderTarget) pure;
    };
}