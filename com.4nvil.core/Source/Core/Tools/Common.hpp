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

// We do not want to use secure functions, as they are not standard
// Note: Investigate is above statemant really true
#define _CRT_SECURE_NO_WARNINGS

// Our special keyword for restrict specifing
#ifdef ENABLE_RESTRICT
#   define noalias __restrict // We use lower case as we will treat as special keyword
#else
#   define noalias
#endif

// Our special keyword for accesing the base class
#define BASE_IS(Class) typedef Class base;

#include <Core/Tools/Types.hpp>
#include <Core/Tools/Testing.hpp>