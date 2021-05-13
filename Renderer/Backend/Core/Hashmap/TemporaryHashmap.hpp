#pragma once

#include <Renderer/Backend/Common.hpp>
#include <Renderer/Backend/Core/ObjectPool/ObjectPool.hpp>
#include <Renderer/Backend/Core/List/List.hpp>
#include <unordered_map>

TRE_NS_START

namespace Renderer
{
	namespace Utils
	{
		template<typename T>
		class HashmapNode
		{
		public:
			FORCEINLINE void SetIndex(uint32 i) { index = i; }

			FORCEINLINE uint32 GetIndex() const { return index; }

			FORCEINLINE Hash GetHash() const { return hash; }

			FORCEINLINE void SetHash(Hash h) { hash = h; }
		private:
			Hash hash = 0;
			uint32 index = 0;
		};

		template<typename T, uint32 RING_SIZE = 4, bool REUSE_OBJECTS = false>
		class TemporaryHashmap
		{
		public:
			TemporaryHashmap() : index(0)
			{}

			~TemporaryHashmap()
			{
				this->Clear();
			}

			void Clear()
			{
				for (auto& ring : rings) {
					for (auto& node : ring)
						objectPool.Free(static_cast<T*>(&node));
					ring.Clear();
				}

				hashmap.clear();

				for (auto& empty : emptys)
					objectPool.Free(static_cast<T*>(&*empty));

				emptys.clear();
				objectPool.Clear();
			}

			void BeginFrame()
			{
				index = (index + 1) & (RING_SIZE - 1);

				for (auto& node : rings[index]) {
					hashmap.erase(node.GetHash());
					this->FreeObject(&node, ReuseTag<REUSE_OBJECTS>());
				}

				rings[index].Clear();
			}

			T* Request(Hash hash)
			{
				auto search = hashmap.find(hash);

				if (search != hashmap.end()) {
					auto node = search->second;

					if (node->GetIndex() != index) {
						rings[index].MoveToFront(rings[node->GetIndex()], node);

						node->SetIndex(index);
					}

					return &*node;
				} else {
					return NULL;
				}
			}

			template<typename... Args>
			T* RequestEmplace(Hash hash, Args&&... args)
			{
				auto search = hashmap.emplace(hash, Itr());

				if (!search.second) { // Insertion didn't happen element already exist
					auto node = search.first->second;

					if (node->GetIndex() != index) {
						rings[index].MoveToFront(rings[node->GetIndex()], node);
						node->SetIndex(index);
					}

					return &*node;
				} else {
					T* node = objectPool.Allocate(std::forward<Args>(args)...);
					search.first->second = node;
					node->SetHash(hash);
					node->SetIndex(index);
					rings[index].InsertFront(node);
					return node;
				}
			}

			template <typename... Args>
			void MakeEmpty(Args&&... args)
			{
				emptys.emplace_back(objectPool.Allocate(std::forward<Args>(args)...));
			}

			T* RequestEmpty(Hash hash)
			{
				if (emptys.empty())
					return NULL;

				auto top = emptys.back();
				emptys.pop_back();
				top->SetHash(hash);
				top->SetIndex(index);
				hashmap.emplace(hash, top).first->second = top;
				rings[index].InsertFront(top);
				return &*top;
			}

			template<typename... Args>
			T* Emplace(Hash hash, Args&&... args)
			{
				T* node = objectPool.Allocate(std::forward<Args>(args)...);
				node->SetHash(hash);
				node->SetIndex(index);
				// emplace/replace
				hashmap.emplace(hash, node).first->second = node;
				rings[index].InsertFront(node);
				return node;
			}
		private:
			typedef typename List<T>::Iterator Itr;

			List<T> rings[RING_SIZE];
			ObjectPool<T> objectPool;
			std::unordered_map<Hash, Itr> hashmap;
			std::vector<Itr> emptys;
			uint32 index;

			template <bool reuse>
			struct ReuseTag
			{
			};

			void FreeObject(T* object, const ReuseTag<false>&)
			{
				objectPool.Free(object);
			}

			void FreeObject(T* object, const ReuseTag<true>&)
			{
				emptys.push_back(object);
			}
		};
	}
}

TRE_NS_END