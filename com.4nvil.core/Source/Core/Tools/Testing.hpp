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

#include <Core/Tools/Console.hpp>

// Automatically enable asserting in debug mode
#if !defined(ENABLED_ASSERT) && defined(ENABLED_DEBUG)
#define ENABLED_ASSERT
#endif

#define STRINGIFY(x) #x
#define STRINGIFY_BUILTIN(x) STRINGIFY(x)

// Assert with custom message use it for debugging purpose
#define RELEASE_ASSERT_MSG(isFalse, ...) \
    if (!(bool)(isFalse)) \
    { \
        Core::Console::Write("ERROR: Assertion failed \'" #isFalse "\'\n"); \
        Core::Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
        Core::Console::Write("    Info:"); \
        Core::Console::WriteFmt(__VA_ARGS__); \
        Core::Console::Write("\n"); \
        __debugbreak(); \
    }

// Default assert use it for debugging purpose
#define RELEASE_ASSERT(isFalse) \
    if (!(bool)(isFalse)) \
    { \
        Core::Console::Write("ERROR: Assertion failed \'" #isFalse "\'\n"); \
        Core::Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
        __debugbreak(); \
    }

// Unconditional assert
#define RELEASE_ERROR(...) \
    { \
        Core::Console::Write("ERROR:"); \
        Core::Console::WriteFmt(__VA_ARGS__); \
        Core::Console::Write("\n"); \
        Core::Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
        __debugbreak(); \
    }

// Prints into IDEA console
#define TRACE(...) \
    { \
        Core::Console::Write("TRACE:"); \
        Core::Console::WriteFmt(__VA_ARGS__); \
        Core::Console::Write("\n"); \
    }

#ifdef ENABLED_ASSERT
#    define ASSERT(isFalse) RELEASE_ASSERT(isFalse)
#    define ASSERT_MSG(isFalse, ...) RELEASE_ASSERT_MSG(isFalse, __VA_ARGS__)
#    define ERROR(...) RELEASE_ERROR(__VA_ARGS__)
#else
#    define ASSERT(isFalse) (void)(isFalse)
#    define ASSERT_MSG(isFalse, ...) (void)(isFalse)
#    define ERROR(...) (void)0
#endif