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
#include <vector>

namespace Core
{
    template<class TypeName>
    class Iterator
    {
    public:
        Iterator() {}
        Iterator(TypeName* ptr) : ptr(ptr) {}

        inline TypeName& operator *() const { return *ptr; }

        inline const Iterator<TypeName>& operator+=(const Iterator<TypeName>& v) { ptr += v.ptr; return *this; }
        inline const Iterator<TypeName>& operator-=(const Iterator<TypeName>& v) { ptr -= v.ptr; return *this; }
        inline const Iterator<TypeName>& operator*=(const Iterator<TypeName>& v) { ptr *= v.ptr; return *this; }
        inline const Iterator<TypeName>& operator/=(const Iterator<TypeName>& v) { ptr /= v.ptr; return *this; }
        inline const Iterator<TypeName>& operator+=(int v) { ptr += v; return *this; }
        inline const Iterator<TypeName>& operator-=(int v) { ptr -= v; return *this; }
        inline const Iterator<TypeName>& operator*=(int v) { ptr *= v; return *this; }
        inline const Iterator<TypeName>& operator/=(int v) { ptr /= v; return *this; }

        inline Bool operator==(const Iterator<TypeName>& v) const { return ptr == v.ptr; }
        inline Bool operator!=(const Iterator<TypeName>& v) const { return ptr != v.ptr; }
        inline Bool operator<=(const Iterator<TypeName>& v) const { return ptr <= v.ptr; }
        inline Bool operator>=(const Iterator<TypeName>& v) const { return ptr >= v.ptr; }
        inline Bool operator&&(const Iterator<TypeName>& v) const { return ptr && v.ptr; }
        inline Bool operator||(const Iterator<TypeName>& v) const { return ptr || v.ptr; }
        inline Bool operator<(const Iterator<TypeName>& v) const { return ptr < v.ptr; }
        inline Bool operator>(const Iterator<TypeName>& v) const { return ptr > v.ptr; }

        inline const Iterator<TypeName>& operator++() { ptr++; return *this; }
        inline const Iterator<TypeName>& operator--() { ptr--; return *this; }

        TypeName* ptr;
    };

    template<class TypeName>
    inline const Iterator<TypeName> operator<(int first, const Iterator<TypeName>& second) { return first < second.ptr; }

    template<class TypeName>
    inline const Iterator<TypeName> operator>(int first, const Iterator<TypeName>& second) { return first > second.ptr; }

    template<class TypeName>
    inline const Iterator<TypeName> operator+(const Iterator<TypeName>& first, const Iterator<TypeName>& second)
    {
        Iterator<TypeName> o = first;
        o += second;
        return o;
    }

    template<class TypeName>
    inline const Iterator<TypeName> operator-(const Iterator<TypeName>& first, const Iterator<TypeName>& second)
    {
        Iterator<TypeName> o = first;
        o -= second;
        return o;
    }

    template<class TypeName>
    inline const Iterator<TypeName> operator+(const Iterator<TypeName>& first, int second)
    {
        Iterator<TypeName> o = first;
        o += second;
        return o;
    }

    template<class TypeName>
    inline const Iterator<TypeName> operator-(const Iterator<TypeName>& first, int second)
    {
        Iterator<TypeName> o = first;
        o -= second;
        return o;
    }

    template<class TypeName>
    class List2
    {
    public:
        List2()
        {
            reserve(1);
        }

        List2(UInt capacity)
        {
            _begin = (TypeName*) malloc(sizeof(TypeName) * capacity);
            _end = _begin + capacity;
            current = _begin;
        }

        inline TypeName& get(UInt index) const
        {
            ASSERT(_begin <= _begin + index && _begin + index <= _end);
            return _begin[index];
        }

        inline TypeName& operator[](UInt index) const
        {
            return get(index);
        }

        inline Void push_back(const TypeName& item)
        {
            reserveFor(size() + 1);
            *current = item;
            current++;
        }

        inline Void push_back(const TypeName&& item)
        {
            reserveFor(size() + 1);
            *current = item;
            current++;
        }

        inline Void pop_back()
        {
            current--;
        }

        inline Void clear()
        {
            current = _begin;
        }

        inline Void resize(UInt _size)
        {
            reserveFor(size() + _size);
            current = _begin + _size;
        }

        inline Void reserve(UInt capacity)
        {
            auto size2 = size();
            _begin = (TypeName*) realloc(_begin, sizeof(TypeName) *capacity);
            _end = _begin + capacity;
            current = _begin + size2;
        }

        inline Void erase(Iterator<TypeName>& itr)
        {
            memcpy(itr.ptr + 1, itr.ptr, (current - itr.ptr) * sizeof(TypeName));
            current--;
        }

        /*inline Iterator<TypeName> find(TypeName&& item)
        {
            for (auto itr = begin(); itr < end(); itr++)
            {

            }
        }*/

        inline TypeName* data() const { return _begin; }
        inline Bool empty() const { return current == _begin; }
        inline UInt size() const { return current - _begin; }
        inline UInt capacity() const { return _end - _begin; }
        inline Iterator<TypeName> begin() const { return Iterator<TypeName>(_begin); }
        inline Iterator<TypeName> end() const { return Iterator<TypeName>(current); }
        inline TypeName& front() const { return *_begin; }
        inline TypeName& back() const { return *_end; }

    private:
        inline Void reserveFor(UInt size2)
        {
            auto capacity2 = capacity();
            if (size() + size2 >= capacity2)
                reserve(capacity2 * 2);
        }

    private:
        TypeName* _begin;
        TypeName* _end;
        TypeName* current;
    };

    template<class T>
    class List : public std::vector<T>
    {
    public:
        List() :
            std::vector<T>()
        {
        }
        List(UInt size) :
            std::vector<T>(size)
        {
        }

        Void remove(T item)
        {
            auto target = std::find(begin(), end(), item);
            ASSERT(target != end());
            erase(target);
        }

        Void safe_set(UInt32 index, T& item)
        {
            if (size() <= index)
                resize(index + 1);

            auto target = this->begin() + index;
            *target = item;
        }

        Int32 find(T item) const
        {
            for (int i = 0; i < size(); i++)
            {
                if (this->data()[i] == item)
                    return i;
            }
            return -1;
        }

        Bool contains(T item) const
        {
            return find(item) != -1;
        }

        Bool safe_push_back(T item)
        {
            if (contains(item))
                return false;

            push_back(item);
            return true;
        }

        Void push_back_list(const List<T>& list)
        {
            auto startSize = size();
            this->resize(size() + list.size());
            memcpy(data() + startSize, list.data(), list.size() * sizeof(T));
        }
    };
}