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

namespace Core
{
    class Module;

    struct ExecutionContext
    {
        // Index of worker in current execution
        UInt32 workerIndex;

        // Start of job offset index
        UInt32 start;

        // End of job offset index
        UInt32 end;

        // Module that is currently executed
        Module* executingModule;
    };

    class IModuleExecutor
    {
    public:
        // Prepares for new frame
        virtual Void Reset() pure;

        // Initializes the executor
        virtual Void Start() pure;

        // Finalizes the executor
        virtual Void Stop() pure;

        // Returns if the executor is still running
        virtual Bool IsRunning() const pure;

        // Returs the number of total workers
        virtual UInt32 GetWorkerCount() const pure;
    };
}