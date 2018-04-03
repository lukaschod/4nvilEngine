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

#include <Core/Tools/Testing.hpp>

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

#ifdef _WIN64
typedef unsigned __int64 size_t;
#else
typedef unsigned int     size_t;
#endif

namespace Core
{
#define AUTOMATED_PROPERTY_GETPTR(Type, Name) \
    protected: Type Name; \
    public: inline Type* Get_##Name() { return &Name; } \
    protected:

#define AUTOMATED_PROPERTY_GETADR(Type, Name) \
    protected: Type Name; \
    public: inline Type& Get_##Name() { return Name; } \
    protected:

#define AUTOMATED_PROPERTY_GET(Type, Name) \
    protected: Type Name; \
    public: GETTER(Type, Name) \
    protected:

#define AUTOMATED_PROPERTY_SET(Type, Name) \
    protected: Type Name; \
    public: SETTER(Type, Name) \
    protected:

#define AUTOMATED_PROPERTY_GETSET(Type, Name) \
    protected: Type Name; \
    public: GETTER(Type, Name) SETTER(Type, Name) \
    protected:

#define SETTER(Type, Name) inline void Set_##Name(Type value) { Name = value; }
#define GETTER(Type, Name) inline Type Get_##Name() const { return Name; }

#define SAFE_DELETE(Pointer) if (Pointer != nullptr) delete Pointer;

#define SAFE_VECTOR_DELETE(Vector) \
    while (Vector.size() != 0) \
    { \
        auto item = Vector.back(); \
        SAFE_DELETE(item); \
        Vector.pop_back(); \
    }

#define IMPLEMENT_ENUM_FLAG(Name) \
inline Name operator|(Name a, Name b) { return static_cast<Name>(static_cast<int>(a) | static_cast<int>(b)); } \
inline Name operator&(Name a, Name b) { return static_cast<Name>(static_cast<int>(a) & static_cast<int>(b)); }
}

#define BASE_IS(Class) typedef Class base;