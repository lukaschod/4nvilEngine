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

#include <Core/Foundation/PipeModule.hpp>

namespace Core
{
    class CallbackModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        // Records callback for this module
        CORE_API Void RecCallback(const ExecutionContext& context, CommandCode code, Void* data, UInt size)
        {
            auto buffer = GetRecordingBuffer(context);
            buffer->commandCount++;
            auto& stream = buffer->stream;
            stream.Write(code);
            stream.Write(data, size);
            stream.Align();
        }

    protected:
        virtual Void ExecuteBeforeModule(ModuleManager* moduleManager, Module* module) override
        {
            base::ExecuteBeforeModule(moduleManager, module);
            Connect(moduleManager, module); // We want to allow module to send callbacks
        }

        virtual Void ExecuteAfterModule(ModuleManager* moduleManager, Module* module) override
        {
            base::ExecuteAfterModule(moduleManager, module);
            Connect(moduleManager, module); // We want to allow module to send callbacks
        }
    };

    template<class T>
    struct AsyncResult
    {
        T result;
        Void* userData;
        Bool finished;
    };

    template<class T>
    struct AsyncCallback
    {
        AsyncCallback() : module(nullptr), code(nullptr) {}
        AsyncCallback(CallbackModule* module, CommandCode code) : module(module), code(code) {}

        inline Void IssueCallback(const ExecutionContext& context, T&& data)
        {
            if (module == nullptr)
                return;
            module->RecCallback(context, code, &data, sizeof(T));
        }

        CallbackModule* module;
        CommandCode code;
    };
}