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
#include <Core/Tools/Enum.hpp>

namespace Core::Graphics
{
    enum class BufferUsageFlags
    {
        None = 0,
        Shader = 1 << 0,
        CpuToGpu = 1 << 1,
        GpuOnly = 1 << 2,
        GpuToCpu = 1 << 3,
    };
    IMPLEMENT_ENUM_FLAG(BufferUsageFlags);

    struct IBuffer
    {
        IBuffer(UInt size)
            : usage(BufferUsageFlags::Shader | BufferUsageFlags::CpuToGpu)
            , size(size)
        {
        }

        inline UInt GetSize() const { return size; }

        UInt size;
        BufferUsageFlags usage;
    };
}