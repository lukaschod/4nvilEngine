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

			inline bool operator==(const Iterator& first)
			{
				return link == first.link;
			}

			inline bool operator!=(const Iterator& first)
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

		void Add(const T& value)
		{
			auto link = PullUnusedLink();
			link->value = value;
			Connect(endLink.prev, link);
			Connect(link, &endLink);
		}

		void Remove(Iterator itr)
		{
			auto link = itr.link;
			Connect(link->prev, link->next);
			linkPool.push(link);
		}

		void Insert(Iterator after, const T& value)
		{
			auto link = PullUnusedLink();
			link->value = value;
			Connect(after.link, link);
			Connect(link, after.link->next);
		}

		Link* Find(T& value) const
		{
			for (auto itr = beginLink; itr != endLink; itr = itr.next)
				if (itr->value == value)
					return itr;
		}

		void Clear()
		{
			for (auto itr = beginLink; itr != endLink; itr = itr.next)
				linkPool.push(itr);
			Connect(beginLink, endLink);
		}

		inline Iterator begin() const { return Iterator((Link*) beginLink.next); }
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

		inline void Connect(Link* first, Link* second) const
		{
			first->next = second;
			second->prev = first;
		}

	private:
		std::queue<Link*> linkPool;
		Link beginLink;
		Link endLink;
	};
}