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

#include <Core\Tools\Threading\AutoResetEvent.hpp>

using namespace Core::Threading;

AutoResetEvent::AutoResetEvent(bool initial)
    : flag(initial)
{
}

void AutoResetEvent::Set()
{
    std::lock_guard<std::mutex> _(protect);
    flag = true;
    signal.notify_one();
}

void AutoResetEvent::Reset()
{
    std::lock_guard<std::mutex> _(protect);
    flag = false;
}

bool AutoResetEvent::WaitOne()
{
    std::unique_lock<std::mutex> lk(protect);
    while (!flag) // prevent spurious wakeups from doing harm
        signal.wait(lk);
    flag = false; // waiting resets the flag
    return true;
}