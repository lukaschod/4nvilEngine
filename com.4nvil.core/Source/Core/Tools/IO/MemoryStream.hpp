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

#include <stdlib.h>
#include <Core/Tools/Common.hpp>

namespace Core::IO
{
    class MemoryStream
    {
    public:
        MemoryStream(UInt capacity = 128)
        {
            end = begin = data = (UInt8*) malloc(capacity);
            end += capacity;
        }

        template<typename T>
        inline Void Write(T& data)
        {
            Write((Void*) &data, sizeof(T));
        }

        inline Void Write(std::string& data)
        {
            Write((Void*) data.c_str(), data.size());
        }

        inline Void Write(UInt32 v)
        {
            Write((Void*) &v, sizeof(UInt32));
        }

        inline Void Write(Void* data, UInt size)
        {
            MakeSureHaveSpace(size);
            memcpy(this->data, data, size);
            this->data += size;
        }

        template<typename T>
        inline Void Read(T& data)
        {
            Read((Void*) &data, sizeof(T));
        }

        inline Void Read(Void* data, UInt size)
        {
            memcpy(data, this->data, size);
            this->data += size;
        }

        template<typename T>
        inline T& FastRead()
        {
            T& ptr = *(T*) data;
            this->data += sizeof(T);
            return ptr;
        }

        inline std::string FastRead()
        {
            std::string out((const char*)data);
            this->data += sizeof(out.size());
            return out;
        }

        template<typename T>
        inline T& FastRead(UInt& offset) const
        {
            T& ptr = *(T*) (begin + offset);
            offset += sizeof(T);
            return ptr;
        }

        inline Void Reset()
        {
            data = begin;
        }

        inline UInt GetCapacity()
        {
            return end - begin;
        }

        inline Void Align(UInt padding = sizeof(Void*))
        {
            data += (UInt) data % padding;
            ASSERT(data <= end);
        }

    private:
        inline Void MakeSureHaveSpace(UInt size)
        {
            while (data + size > end)
            {
                auto capacity = data - begin;
                auto requiredCapacity = (end - begin) * 2;
                begin = (UInt8*) realloc(begin, requiredCapacity);
                data = begin + capacity;
                end = begin + requiredCapacity;
            }
        }

    private:
        UInt8* begin;
        UInt8* end;
        AUTOMATED_PROPERTY_GETSET(UInt8*, data);
    };
}