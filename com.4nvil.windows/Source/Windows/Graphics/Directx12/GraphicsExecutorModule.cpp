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

#include <algorithm>
#include <Windows/Graphics/Directx12/GraphicsExecutorModule.hpp>
#include <Windows/Graphics/Directx12/CmdQueue.hpp>

using namespace Windows;
using namespace Windows::Directx12;

GraphicsExecutorModule::GraphicsExecutorModule() : completedBufferIndex(0) {}

void GraphicsExecutorModule::RecCmdBuffer(const ExecutionContext& context, const Directx12::CmdBuffer* buffer)
{
    auto& executerContext = executerContexts[context.workerIndex];
    executerContext.recordedCmds.push_back(buffer);
}

void GraphicsExecutorModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    executerContexts.resize(moduleManager->GetWorkerCount());
}

void GraphicsExecutorModule::Execute(const ExecutionContext& context)
{
    MARK_FUNCTION;
    // TODO: Make sure no allocation happens
    cmdBuffersToExecute.clear();
    for (auto& executerContext : executerContexts)
    {
        cmdBuffersToExecute.push_back_list(executerContext.recordedCmds);
        executerContext.recordedCmds.clear();
    }

    std::sort(cmdBuffersToExecute.begin(), cmdBuffersToExecute.end(), 
        [](const Directx12::CmdBuffer* first, const Directx12::CmdBuffer* second) { return first->index < second->index; });

    auto back = cmdBuffersToExecute.back();
    for (auto cmdBuffer : cmdBuffersToExecute)
    {
        cmdBuffer->queue->Execute((Directx12::CmdBuffer*)cmdBuffer, cmdBuffer == back); // TODO: Fix this const shit
    }

    // TODO: remove once we have double/triple buffering
    if (!cmdBuffersToExecute.empty())
        cmdBuffersToExecute.back()->queue->WaitForBufferIndexToComplete(back->index);
    completedBufferIndex = back->index;
}