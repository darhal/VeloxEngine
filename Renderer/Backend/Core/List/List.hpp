#pragma once

#include <Renderer/Backend/Common.hpp>

TRE_NS_START

namespace Renderer
{
	namespace Utils
	{
		template<typename T>
		struct ListNode
		{
			ListNode<T>* prev = NULL;
			ListNode<T>* next = NULL;
		};

		template<typename T>
		class List
		{
		public:
			class Iterator
			{
			public:
				friend class List<T>;

				Iterator(ListNode<T>* node_)
					: node(node_)
				{
				}

				Iterator() = default;

				explicit operator bool() const
				{
					return node != nullptr;
				}

				bool operator==(const Iterator& other) const
				{
					return node == other.node;
				}

				bool operator!=(const Iterator& other) const
				{
					return node != other.node;
				}

				T& operator*()
				{
					return *static_cast<T*>(node);
				}

				const T& operator*() const
				{
					return *static_cast<T*>(node);
				}

				T* Get()
				{
					return static_cast<T*>(node);
				}

				const T* Get() const
				{
					return static_cast<const T*>(node);
				}

				T* operator->()
				{
					return static_cast<T*>(node);
				}

				const T* operator->() const
				{
					return static_cast<T*>(node);
				}

				Iterator& operator++()
				{
					node = node->next;
					return *this;
				}

				Iterator& operator--()
				{
					node = node->prev;
					return *this;
				}

			private:
				ListNode<T>* node = NULL;
			};
		public:
			void Clear()
			{
				head = NULL;
				tail = NULL;
			}

			Iterator Erase(Iterator itr)
			{
				auto* node = itr.Get();
				auto* next = node->next;
				auto* prev = node->prev;

				if (prev)
					prev->next = next;
				else
					head = next;

				if (next)
					next->prev = prev;
				else
					tail = prev;

				return next;
			}

			void InsertFront(Iterator itr)
			{
				auto* node = itr.Get();

				if (head)
					head->prev = node;
				else
					tail = node;

				node->next = head;
				node->prev = nullptr;
				head = node;
			}

			void InsertBack(Iterator itr)
			{
				auto* node = itr.Get();
				if (tail)
					tail->next = node;
				else
					head = node;

				node->prev = tail;
				node->next = nullptr;
				tail = node;
			}

			void MoveToFront(List<T>& other, Iterator itr)
			{
				other.Erase(itr);
				InsertFront(itr);
			}

			void MoveToBack(List<T>& other, Iterator itr)
			{
				other.Erase(itr);
				InsertBack(itr);
			}

			bool IsEmpty() const
			{
				return head == nullptr;
			}

			Iterator begin()
			{
				return Iterator(head);
			}

			Iterator rbegin()
			{
				return Iterator(tail);
			}

			Iterator end()
			{
				return Iterator();
			}

		private:
			ListNode<T>* head = NULL;
			ListNode<T>* tail = NULL;
		};
	}
}

TRE_NS_END