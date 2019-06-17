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
	struct AVLNode
	{
		template<typename... Args>
		AVLNode(AVLNode* parent, AVLNode* left, AVLNode* right, Args&&... args) : 
			parent(parent), left(left), right(right),  bf(0),
			data(std::forward<Args>(args)...)
		{}
		T data;
		AVLNode* left;
		AVLNode* right;
		AVLNode* parent;
		int32 bf; // Balance Factor
	};

	typedef AVLNode Node;
	typedef AVLNode Leaf;

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
};

#include "AVL.inl"

TRE_NS_END