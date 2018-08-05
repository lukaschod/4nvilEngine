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

#include <map>
#include <Core/Tools/Common.hpp>
#include <Core/Tools/IO/MemoryStream.hpp>
#include <Core/Modules/Module.hpp>
#include <Core/Foundation/ProfilerModule.hpp>
#include <Core/Foundation/PipeModuleHelper.hpp>

namespace Core
{
    typedef UInt32 CommandCode;
    typedef IO::MemoryStream CommandStream;

    struct CmdBuffer
    {
        CmdBuffer() : commandCount(0) {}

        // Recorded commands with all its data
        CommandStream stream;

        // Total count of commands recorded in this buffer
        UInt commandCount;

        // On which worker the cmdbuffer was created
        UInt32 workerIndex;

        // Module that records this buffer
        Module* executingModule;
    };

    class PipeModule : public Module
    {
    public:
        BASE_IS(Module);

        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        CORE_API virtual Void Execute(const ExecutionContext& context) override;

    protected:
        // This is where each PipeModule implementation will add its commands
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) pure;

        CORE_API virtual Void Connect(ModuleManager* moduleManager, Module* module) override;

        // Returns buffer that should be used for recording the commands
        inline CmdBuffer* GetRecordingBuffer(const ExecutionContext& context);

    private:
        // Sort the pipes according the order of Module execution, deducted from the dependancy tree
        Void SortPipes();
        Void AddPipe(ModuleManager* moduleManager, Module* module);

    protected:
        ProfilerModule* profilerModule;

        // Pipe is simple one directiona connection between Modules
        struct Pipe
        {
            Pipe(Module* source, UInt size) : buffers(size), source(source) {}
            Module* source;
            List<CmdBuffer> buffers;
        };

        std::map<Module*, Pipe*> pipeMap; // Map between the Module and Pipe
        List<Pipe*> pipes; // All the Pipes that belong to this Module
        Bool isPipesSorted;

        struct CachedCmdBuffer
        {
            CachedCmdBuffer() : source(nullptr), currentBuffer(nullptr) {}
            Module* source;
            CmdBuffer* currentBuffer;
        };
        List<CachedCmdBuffer> cachedCmdBuffers;
    };

    CmdBuffer* PipeModule::PipeModule::GetRecordingBuffer(const ExecutionContext& context)
    {
        // As long as for the worker executing Module doesn't change, we can actaully cache Pipe for faster access
        auto& cachedCmdBuffer = cachedCmdBuffers[context.workerIndex];
        if (cachedCmdBuffer.source == context.executingModule)
            return cachedCmdBuffer.currentBuffer;

        // If cache miss happens, we should find the Pipe corresponding to the executing Module
        auto pipe = pipeMap.find(context.executingModule);
        ASSERT(pipe != pipeMap.end());
        auto buffer = &pipe->second->buffers[context.executingModule->IsSplittable() ? context.workerIndex : 0];

        // Update cache with new executing module
        cachedCmdBuffer.source = context.executingModule;
        cachedCmdBuffer.currentBuffer = buffer;

        return buffer;
    }
}