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
#include <Core/Tools/String.hpp>

namespace Core
{
    class Guid
    {
    public:
        inline Bool operator==(const Guid& rhs) const { return memcmp(this, &rhs, sizeof(Guid)) == 0; }
        inline Bool operator!=(const Guid& rhs) const { return memcmp(this, &rhs, sizeof(Guid)) != 0; }

        // Convert Guid to 32 digits separated by hyphens
        CORE_API Void ToString(Char* value);

        // Convert 32 digits seperated by hyphens into Guid
        CORE_API static Bool TryParse(const Char* value, Guid& guid);

        // Generate new unique Guid
        CORE_API static Guid Generate();

    public:
        UInt32 data1;
        UInt16 data2;
        UInt16 data3;
        UInt8 data4[8];
    };
}