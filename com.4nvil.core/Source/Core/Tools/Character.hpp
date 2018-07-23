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

#include <Core/Tools/Common.hpp>
#include <cstring>

#pragma once

namespace Core
{
    // All the functions related with character array
    class Character
    {
    private:
        Character() {}

    public:
        // Checks if two character arrays are the same
        inline static Bool Equals(const Char* value1, const Char* value2) { return strcmp(value1, value2) == 0; }

        // Checks if two character arrays are not the same
        inline static Bool NotEquals(const Char* value1, const Char* value2) { return strcmp(value1, value2) != 0; }

        // Returns the lenght of character array terminated by 0
        inline static UInt Length(const Char* value) { return strlen(value); }
    };
}