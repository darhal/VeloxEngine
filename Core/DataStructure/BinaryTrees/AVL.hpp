#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Allocators/PoolAlloc/MultiPoolAllocator.hpp>

TRE_NS_START

template<typename T, typename Alloc_t = MultiPoolAlloc>
class AVL
{
public:
	struct AVLNode;
	class Iterator;

	struct AVLNode
	{
		template<typename... Args>
		AVLNode(AVLNode* parent, AVLNode* left, AVLNode* right, Args&&... args) : 
			parent(parent), left(left), right(right),  bf(0),
			data(::std::forward<Args>(args)...)
		{}
		T data;
		AVLNode* left;
		AVLNode* right;
		AVLNode* parent;
		int32 bf; // Balance Factor
	};

	typedef AVLNode Node;
	typedef AVLNode Leaf;

public:
	FORCEINLINE AVL();

	~AVL();

	// Find the node with the minimum key
	FORCEINLINE AVLNode* Min(AVLNode* node);

	// Find the node with the maximum key
	FORCEINLINE AVLNode* Max(AVLNode* node);

	// Inserts element at the propreitate position
	template<typename... Args>
	FORCEINLINE void Insert(Args&&... args);

	// Delete the node from the tree
	FORCEINLINE void Remove(const T& data);

	// print the tree structure on the screen
	FORCEINLINE void Print();

private:
	CONSTEXPR static const usize NODE_CHUNKS = 3;

	AVLNode* m_Root;
	Alloc_t m_Allocator;

	AVLNode* SearchTreeHelper(AVLNode* node, const T& data);

	AVLNode* DeleteNodeHelper(AVLNode* node, const T& data);

	// Update the balance factor the node
	void UpdateBalance(AVLNode* node);

	// Rebalance the tree
	void Rebalance(AVLNode* node);

	// Rotate left at node x
	void LeftRotate(AVLNode* x);

	// Rotate right at node x
	void RightRotate(AVLNode* x);

	// Print Helper
	void PrintHelper(AVLNode* root, String indent, bool last);

	// Destroy the tree!
	FORCEINLINE void DestroyTree(AVLNode* cur);

	FORCEINLINE Iterator begin() noexcept;

	FORCEINLINE Iterator end() noexcept;

	FORCEINLINE const Iterator begin() const noexcept;

	FORCEINLINE const Iterator end() const noexcept;

public:
	class Iterator
	{
	public:
		Iterator(AVL<T, Alloc_t>* instance) : m_TreeInstance(instance), m_Node(instance->m_Root)
		{};

		Iterator(AVL<T, Alloc_t>* instance, Node* node) : m_TreeInstance(instance), m_Node(node)
		{};

		bool operator!=(const Iterator& iterator) { return m_Node != iterator.m_Node; }

		Node& operator*() const { return *m_Node; }

		Iterator& operator=(const Iterator& other)
		{
			this->m_Node = other.m_Node;
			this->m_TreeInstance = other.m_TreeInstance;
			return *this;
		}

		Iterator& operator++()
		{
			Node* parent = NULL;

			if (this->m_Node == NULL) {
				return *this; // end iterator does not increment
			}

			parent = this->m_Node->parent;

			// reaches root -> next is end()
			if (parent == NULL) {
				this->m_Node = NULL;
				return *this;
			}

			// left child -> go to right child
			if ((this->m_Node == parent->left) && parent->right != NULL) {
				this->m_Node = parent->right;
			}else {
				this->m_Node = this->m_Node->parent;
				return *this;
			}

			while (true) {
				if (this->m_Node->left != NULL) {
					this->m_Node = this->m_Node->left; // has left child node
				}else if (this->m_Node->right != NULL) {
					this->m_Node = this->m_Node->right; // only right child node
				}else {
					return *this; // has no children -> stop here
				}
			}
		}

		Iterator operator++(int)
		{
			Iterator iterator = *this;
			++(*this);
			return iterator;
		}

		Iterator& operator--()
		{
			// TODO: operator -- have to be implemented
			return *this;
		}

		Iterator operator--(int)
		{
			Iterator iterator = *this;
			--*this;
			return iterator;
		}

	private:
		Node* m_Node;
		AVL<T, Alloc_t>* m_TreeInstance;
	};
};

#include "AVL.inl"

TRE_NS_END