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

#ifndef DISABLED_CORE
#ifdef ENABLED_CORE
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif
#else
#define CORE_API
#endif

typedef signed char         Int8;
typedef short               Int16;
typedef int                 Int32;
typedef long long           Int64;
typedef Int64               Int; // Engine will support x64 only no need for conditional
typedef unsigned char       UInt8;
typedef unsigned short      UInt16;
typedef unsigned int        UInt32;
typedef unsigned long long  UInt64;
typedef UInt64              UInt; // Engine will support x64 only no need for conditional

typedef void                Void;
typedef float               Float;
typedef double              Float64;
typedef Float               Float32;
typedef bool                Bool;

typedef char                Char;
typedef char                Char8;