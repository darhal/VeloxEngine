#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/DataStructure/String/String.hpp>
#include <Core/Memory/Allocators/PoolAlloc/MultiPoolAllocator.hpp>
#include <iterator>
#include <utility>
#include <iostream>

// Based on this : https://github.com/Bibeknam/algorithmtutorprograms/blob/master/data-structures/red-black-trees/RedBlackTree.cpp
// https://codereview.stackexchange.com/questions/153476/iterator-through-binary-trees-with-parent-pointer-without-memory-overhead

TRE_NS_START

template<typename T>
static bool Compare(const T& a, const T& b)
{
	return a < b;
}

template<typename K, typename T, typename Alloc_t = MultiPoolAlloc>
class RedBalckTree
{
public:
	template<typename PointerType>
	class GIterator;

	struct RedBlackNode;

	typedef GIterator<RedBlackNode> Iterator;
	typedef GIterator<const RedBlackNode> CIterator;

	struct RedBlackNode {
	public:
		union {
			struct {
				K key;					// holds the key
				T value;				// holds the value
			};

			struct{
				K first;				// holds the key
				T second;				// holds the value
			};
		};
		
		~RedBlackNode() {};

		template<typename... Args>
		RedBlackNode(RedBlackNode* p, RedBlackNode* r, RedBlackNode* l, const K& key, Args&&... args) : 
			key(key), value(std::forward<Args>(args)...), parent(p), left(l), right(r), color(true)
		{}

		RedBlackNode(const RedBlackNode& other) : 
			key(other.key), value(other.value), parent(other.parent), right(other.right), left(other.left), color(color)
		{}

		RedBlackNode(RedBlackNode&& other) : 
			key(std::move(other.key)), value(std::move(other.value)), parent(other.parent), right(other.right), left(other.left), color(color)
		{}

		RedBlackNode* GetRight() { return right; }
		RedBlackNode* GetLeft() { return left; }
		RedBlackNode* GetParent() { return parent; }
		T& GetValue() { return value; }
		K& GetKey() { return key; }

		void SetRight(RedBlackNode* r) { right = r; }
		void SetLeft(RedBlackNode* l) { left = l; }
		void SetParent(RedBlackNode* p) { parent = p; }
		
	private:
		RedBlackNode* parent;	// pointer to the parent
		RedBlackNode* left;		// pointer to left child
		RedBlackNode* right;	// pointer to right child
		bool color;				// true -> Red, false -> Black

		friend RedBalckTree<K, T, Alloc_t>;
	};

	typedef RedBlackNode RBNode;
	typedef RedBlackNode RBLeaf;

	FORCEINLINE RedBalckTree();

	FORCEINLINE RedBalckTree(RedBalckTree<K, T, Alloc_t>&& other);

	FORCEINLINE RedBalckTree(const RedBalckTree<K, T, Alloc_t>& other);

	RedBalckTree& operator=(RedBalckTree<K, T, Alloc_t>&& other);

	RedBalckTree& operator=(const RedBalckTree<K, T, Alloc_t>& other);

	virtual ~RedBalckTree();

	template<typename... Args>
	FORCEINLINE RedBalckTree(const K& key, Args&&... args);

	FORCEINLINE RBNode* Search(const K& key) const;

	template<typename... Args>
	FORCEINLINE T& Insert(const K& key, Args&&... args);

	FORCEINLINE void Remove(const K& key);

	FORCEINLINE void LeftRotate(RBNode* x);

	FORCEINLINE void RightRotate(RBNode* x);

	// find the node with the minimum key
	FORCEINLINE RBNode* Min(RBNode* node);

	// find the node with the maximum key
	FORCEINLINE RBNode* Max(RBNode* node);

	FORCEINLINE void Clear();

	FORCEINLINE bool IsEmpty() const;

	FORCEINLINE void Print();

	FORCEINLINE Iterator begin() noexcept;

	FORCEINLINE Iterator end() noexcept;

	FORCEINLINE const CIterator begin() const noexcept;

	FORCEINLINE const CIterator end() const noexcept;

	FORCEINLINE const CIterator cbegin() const noexcept;

	FORCEINLINE const CIterator cend() const noexcept;

private:
	CONSTEXPR static const usize NODE_CHUNKS = 3;
	CONSTEXPR static RBNode* TNULL = NULL;//reinterpret_cast<RBNode*>(-1);

	RBNode* m_Root;
	Alloc_t m_Allocator;

	FORCEINLINE void InsertHelper(RBNode* newNode, RBNode* parent);

	void DestroyTree(RBNode* node);

	// initializes the nodes with appropirate values
	// all the pointers are set to point to the null pointer
	FORCEINLINE void InitNullNode(RBNode* node, RBNode* parent);

	RBNode* SearchTreeHelper(RBNode* node, const K& key) const;

	FORCEINLINE void RBTransplant(RBNode* u, RBNode* v);

	// fix the rb tree modified by the delete operation
	void FixDelete(RBNode* x);

	void DeleteNodeHelper(RBNode* node, const K& key);

	// fix the red-black tree
	void FixInsert(RBNode* k);

	void PrintHelper(RBNode* node, String indent, bool last);

	void CopyNodeRecursice(RBNode* dst_node, RBNode* src_node);
public:

	template<typename DataType>
	class GIterator : public std::iterator<std::bidirectional_iterator_tag, DataType, ptrdiff_t, DataType*, DataType&>
	{
	public:
		GIterator(const RedBalckTree<K, T, Alloc_t>* instance) : m_Node(instance->m_Root), m_TreeInstance(instance)
		{};

		GIterator(const RedBalckTree<K, T, Alloc_t>* instance, DataType* node) :  m_Node(node), m_TreeInstance(instance)
		{};
		
		bool operator!=(const GIterator& iterator) { return m_Node != iterator.m_Node; }

		DataType& operator*() { return *m_Node; }
		const DataType& operator*() const { return (*m_Node); }

		DataType* operator->() { return m_Node; }
		const DataType* operator->() const { return m_Node; }

		GIterator& operator=(const GIterator& other)
		{
			this->m_Node = other.m_Node;
			this->m_TreeInstance = other.m_TreeInstance;
			return *this;
		}

		GIterator& operator++()
		{
			DataType* parent = NULL;

			if (this->m_Node == NULL || this->m_Node == TNULL) {
				return *this; // end iterator does not increment
			}

			parent = this->m_Node->parent;

			// reaches root -> next is end()
			if (parent == NULL || parent == TNULL) {
				this->m_Node = NULL;
				return *this;
			}

			// left child -> go to right child
			if ((this->m_Node == parent->left) && (parent->right != NULL || parent->right != TNULL)) {
				this->m_Node = parent->right;
			}else{
				this->m_Node = this->m_Node->parent;
				return *this;
			}

			while (true) {
				if (this->m_Node->left != NULL || this->m_Node->left != TNULL) {
					this->m_Node = this->m_Node->left; // has left child node
				}else if (this->m_Node->right != NULL || this->m_Node->right != TNULL) {
					this->m_Node = this->m_Node->right; // only right child node
				}else {
					return *this; // has no children -> stop here
				}
			}
		}

		GIterator operator++(int)
		{
			GIterator iterator = *this;
			++*this;
			return iterator;
		}

		GIterator& operator--()
		{
			// TODO: operator -- have to be implemented
			return *this;
		}

		GIterator operator--(int)
		{
			GIterator iterator = *this;
			--*this;
			return iterator;
		}

	private:
		DataType* m_Node;
		const RedBalckTree<K, T, Alloc_t>* m_TreeInstance;
	};
};

template<typename K, typename T, typename Alloc_t>
class RedBalckTree<const K, const T, Alloc_t> : public RedBalckTree<K, T, Alloc_t>
{
};

//template<typename K, typename T, typename Alloc_t = MultiPoolAlloc>
//using RBTree = RedBalckTree<K, T, Alloc_t>;

template<typename K, typename T, typename Alloc_t = MultiPoolAlloc>
using RBT = RedBalckTree<K, T, Alloc_t>;

#include "RedBlackTree.inl"

TRE_NS_END