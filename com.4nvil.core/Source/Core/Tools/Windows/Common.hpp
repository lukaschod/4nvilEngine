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

#if ENABLED_WINDOWS

// Exclude rarely-used stuff from Windows headers for speed up
#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN 
#endif
#ifndef NOMINMAX
#    define NOMINMAX
#endif

// Exclude ERROR macros for our usage
#ifndef NOGDI
#    define NOGDI
#endif

#include <windows.h>
#include <comdef.h>
#include <Core/Tools/Common.hpp>

#ifdef ENABLED_DEBUG
#define ASSERT_SUCCEEDED(hr) \
    if (FAILED(hr)) \
    { \
        _com_error err(hr); \
        LPCTSTR errMsg = err.ErrorMessage(); \
        Core::Console::Write("ERROR: Assertion failed \'" #hr "\'\n"); \
        Core::Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
        Core::Console::Write("    Info: "); \
        Core::Console::Write(errMsg); \
        Core::Console::WriteFmt(" (hr = 0x%08X)", hr); \
        Core::Console::Write("\n"); \
        __debugbreak(); \
    }
#else
#define ASSERT_SUCCEEDED(hr) (Void)(hr)
#endif

#else
#error ENABLED_WINDOWS macro is required for this header
#endif