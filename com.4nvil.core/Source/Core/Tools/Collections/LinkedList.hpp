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

#include <queue>

namespace Core
{
    template<class T>
    class LinkedList
    {
    public:
        struct Link
        {
            T value;
            Link* next;
            Link* prev;
        };

        struct Iterator
        {
        public:
            Iterator(Link* link)
                : link(link)
            {
            }

            inline Iterator& operator++()
            {
                link = link->next;
                return *this;
            }

            inline Iterator& operator+(int value)
            {
                Iterator itr(link->next);
                return itr;
            }

            inline Iterator& operator--()
            {
                link = link->prev;
                return *this;
            }

            inline Bool operator==(const Iterator& first)
            {
                return link == first.link;
            }

            inline Bool operator!=(const Iterator& first)
            {
                return link != first.link;
            }

            inline T& operator*()
            {
                return link->value;
            }

        public:
            Link* link;
        };

    public:
        LinkedList()
        {
            beginLink.next = &endLink;
            beginLink.prev = nullptr;
            endLink.next = nullptr;
            endLink.prev = &beginLink;
        }

        Void Add(const T& value)
        {
            auto link = PullUnusedLink();
            link->value = value;
            Connect(endLink.prev, link);
            Connect(link, &endLink);
        }

        Void Remove(Iterator itr)
        {
            auto link = itr.link;
            Connect(link->prev, link->next);
            linkPool.push(link);
        }

        Void Insert(Iterator after, const T& value)
        {
            auto link = PullUnusedLink();
            link->value = value;
            Connect(link, after.link->next);
            Connect(after.link, link);
        }

        Link* Find(T& value) const
        {
            for (auto itr = beginLink; itr != endLink; itr = itr.next)
                if (itr->value == value)
                    return itr;
        }

        Void Clear()
        {
            for (auto itr = beginLink; itr != endLink; itr = itr.next)
                linkPool.push(itr);
            Connect(beginLink, endLink);
        }

        inline Iterator begin() const { return Iterator((Link*) &beginLink); }
        inline Iterator end() const { return Iterator((Link*) &endLink); }

    private:
        inline Link* PullUnusedLink()
        {
            if (linkPool.empty())
                return new Link();

            auto link = linkPool.front();
            linkPool.pop();
            return link;
        }

        inline Void Connect(Link* first, Link* second) const
        {
            ASSERT(first != second);
            first->next = second;
            second->prev = first;
        }

    private:
        std::queue<Link*> linkPool;
        Link beginLink;
        Link endLink;
    };
}