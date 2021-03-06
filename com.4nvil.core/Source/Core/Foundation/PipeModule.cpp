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
#include <Core/Tools/Math/Math.hpp>
#include <Core/Foundation/PipeModule.hpp>
#include <Core/Foundation/ProfilerModule.hpp>

using namespace Core;

Void PipeModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    profilerModule = ExecuteAfter<ProfilerModule>(moduleManager);
    cachedCmdBuffers.resize(moduleManager->GetWorkerCount());
    AddPipe(moduleManager, this); // Allow adding commends on itself
    isPipesSorted = false;
}

Void PipeModule::Connect(ModuleManager* moduleManager, Module* module)
{
    base::Connect(moduleManager, module);
    AddPipe(moduleManager, module);
}

Void PipeModule::SortPipes()
{
    // Sort the pipies according the dependencies
    std::function<Bool(Module*, Module*)> recusrive = [&recusrive](Module* first, Module* second)
    {
        if (first == second)
            return false;

        for (auto module : second->GetDependencies())
            if (!recusrive(first, module))
                return false;

        return true;
    };

    std::sort(pipes.begin(), pipes.end(),
        [&recusrive](Pipe* first, Pipe* second) { return !recusrive(first->source, second->source); });
    isPipesSorted = true;
}

Void PipeModule::AddPipe(ModuleManager* moduleManager, Module* module)
{
    // It is possible it might try to add duplicate
    if (pipeMap.find(module) != pipeMap.end())
        return;

    // Once dependancy is added, we should create corresponding Pipe for it, where all communication will be stored
    auto pipe = new Pipe(module, module->IsSplittable() ? moduleManager->GetWorkerCount() : 1);
    pipes.push_back(pipe);
    pipeMap[module] = pipe;
    isPipesSorted = false;
}

/*CmdBuffer* PipeModule::GetRecordingBuffer(const ExecutionContext& context)
{
    // As long as for the worker executing Module doesn't change, we can actaully cache Pipe for faster access
    auto& cachedCmdBuffer = cachedCmdBuffers[context.workerIndex];
    if (cachedCmdBuffer.source == context.executingModule)
        return cachedCmdBuffer.currentBuffer;

    // If cache miss happens, we should find the Pipe corresponding to the executing Module
    auto pipe = pipeMap.find(context.executingModule);
    ASSERT(pipe != pipeMap.end());
    auto buffer = &pipe->second->buffers[context.executingModule->IsSplittable() ? 0 : context.workerIndex];

    // Update cache with new executing module
    cachedCmdBuffer.source = context.executingModule;
    cachedCmdBuffer.currentBuffer = buffer;

    return buffer;
}*/

Void PipeModule::Execute(const ExecutionContext& context)
{
    // TODO: Move it to some initialization
    // We can sort the pipes only once, as dependencies between the Modules are static
    // NOTE: Of course branch prediction migh eleminate this performance cost
    if (!isPipesSorted)
        SortPipes();

    for (auto pipe : pipes)
    {
        for (auto& buffer : pipe->buffers)
        {
            if (buffer.commandCount == 0)
                continue;

            // Reset buffer for executing
            auto& stream = buffer.stream;
            buffer.stream.Reset();
            buffer.executingModule = context.executingModule;

            // Execute each command one by one
            for (UInt i = 0; i < buffer.commandCount; i++)
            {
                auto& commandCode = stream.FastRead<CommandCode>();
                CHECK(ExecuteCommand(context, stream, commandCode));
                stream.Align();
            }

            // Prepare buffer for next frame recording
            buffer.commandCount = 0;
            buffer.stream.Reset();
        }
    }

    // Clear cache
    for (auto& cachedCmdBuffer : cachedCmdBuffers)
        cachedCmdBuffer.source = nullptr;
}
