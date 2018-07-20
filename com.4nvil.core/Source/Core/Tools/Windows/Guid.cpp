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

#include <Core/Tools/Guid.hpp>
#include <Core/Tools/Windows/Common.hpp>

using namespace Core;

Void Guid::ToString(Char* value)
{
    sprintf(value, "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
        data1, data2, data3,
        data4[0], data4[1], data4[2], data4[3],
        data4[4], data4[5], data4[6], data4[7]);
}

Bool Guid::TryParse(const Char* value, Guid& guid)
{
    return sscanf(value, "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
        &guid.data1, &guid.data2, &guid.data3,
        &guid.data4[0], &guid.data4[1], &guid.data4[2], &guid.data4[3],
        &guid.data4[4], &guid.data4[5], &guid.data4[6], &guid.data4[7]) == 11;
}

Guid Guid::Generate()
{
    Guid guid;
    ASSERT_SUCCEEDED(CoCreateGuid((GUID*)&guid));
    return guid;
}