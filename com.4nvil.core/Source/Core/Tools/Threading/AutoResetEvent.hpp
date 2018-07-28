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

#include <mutex>
#include <condition_variable>
#include <Core/Tools/Common.hpp>

namespace Core::Threading
{
    class AutoResetEvent
    {
    public:
        CORE_API explicit AutoResetEvent(Bool initial = false);

        CORE_API Void Set();
        CORE_API Void Reset();
        CORE_API Bool WaitOne();

    private:
        AutoResetEvent(const AutoResetEvent&);
        AutoResetEvent& operator=(const AutoResetEvent&); // non-copyable

    private:
        std::condition_variable signal;
        std::mutex protect;
        Bool flag;
    };
}