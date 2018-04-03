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

#include <Core/Foundation/ComputeModule.hpp>
#include <Windows/Graphics/Directx12/Common.hpp>
#include <Windows/Graphics/Directx12/CmdQueue.hpp>

namespace Windows::Directx12
{
    struct CmdBuffer;
}

namespace Windows::Directx12
{
    class GraphicsExecutorModule : public ComputeModule
    {
    public:
        BASE_IS(ComputeModule);

        GraphicsExecutorModule();
        virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
        virtual void Execute(const ExecutionContext& context) override;
        virtual const char* GetName() override { return "GraphicsExecutorModule"; }

        void RecCmdBuffer(const ExecutionContext& context, const Directx12::CmdBuffer* buffer);

    private:
        struct Context
        {
            List<const Directx12::CmdBuffer*> recordedCmds;
        };
        List<Context> executerContexts;

        List<const Directx12::CmdBuffer*> cmdBuffersToExecute;
        List<ID3D12GraphicsCommandList*> batchedCommandLists;
        AUTOMATED_PROPERTY_GET(uint64, completedBufferIndex);
    };
}