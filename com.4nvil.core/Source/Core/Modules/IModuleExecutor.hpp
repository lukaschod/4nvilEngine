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

    class IModuleExecutor
    {
    public:
        // Prepares for new frame
        virtual Void Reset() = 0;

        // Initializes the executor
        virtual Void Start() = 0;

        // Finalizes the executor
        virtual Void Stop() = 0;

        // Returns if the executor is still running
        virtual Bool IsRunning() = 0;

        // Returs the number of total workers
        virtual UInt32 GetWorkerCount() = 0;
    };
}