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

#include <type_traits>

namespace Core::Enum
{
    // Converts enum value into any primitive type
    template<typename E> constexpr typename std::underlying_type<E>::type ToUnderlying(E e) noexcept
    {
        return static_cast<typename std::underlying_type<E>::type>(e);
    }

    template<typename E> inline Bool Contains(E lhs, E rhs)
    {
        return (lhs & rhs) != static_cast<E>(0);
    }
}

#define IMPLEMENT_ENUM_FLAG(Name) \
inline Name operator|(Name lhs, Name rhs) { return static_cast<Name>(static_cast<std::underlying_type_t<Name>>(lhs) | static_cast<std::underlying_type_t<Name>>(rhs)); } \
inline Name operator&(Name lhs, Name rhs) { return static_cast<Name>(static_cast<std::underlying_type_t<Name>>(lhs) & static_cast<std::underlying_type_t<Name>>(rhs)); } \
inline Name operator^(Name lhs, Name rhs) { return static_cast<Name>(static_cast<std::underlying_type_t<Name>>(lhs) ^ static_cast<std::underlying_type_t<Name>>(rhs)); } \
inline Name& operator|=(Name& lhs, Name rhs) { lhs = static_cast<Name>(static_cast<std::underlying_type_t<Name>>(lhs) | static_cast<std::underlying_type_t<Name>>(rhs)); return lhs; } \
inline Name& operator&=(Name& lhs, Name rhs) { rhs = static_cast<Name>(static_cast<std::underlying_type_t<Name>>(lhs) & static_cast<std::underlying_type_t<Name>>(rhs)); return rhs; } \
inline Name& operator^=(Name& lhs, Name rhs) { rhs = static_cast<Name>(static_cast<std::underlying_type_t<Name>>(lhs) ^ static_cast<std::underlying_type_t<Name>>(rhs)); return rhs; }