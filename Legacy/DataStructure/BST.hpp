#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/Misc/Defines/Debug.hpp>
#include "BinaryTree.hpp"

TRE_NS_START


template<typename T, typename Alloc_t = MultiPoolAlloc>
class BinarySearchTree : public BinaryTree<T, Alloc_t>
{
public:
	typedef BinaryTree<T, Alloc_t> BTree;
	typedef typename BinaryTree<T, Alloc_t>::BTLeaf BTLeaf;
	typedef typename BinaryTree<T, Alloc_t>::BTNode BTNode;

	FORCEINLINE BinarySearchTree();

	template<typename... Args>
	FORCEINLINE BinarySearchTree(Args&&... args);

	FORCEINLINE virtual ~BinarySearchTree();

	template<typename... Args>
	FORCEINLINE BTLeaf& Insert(Args&&... args);

	FORCEINLINE void Remove(const T& value);

	FORCEINLINE T& FindMin() const;
	FORCEINLINE T& FindMax() const;

	FORCEINLINE bool Find(const T& value) const;

private:
	FORCEINLINE void InsertHelper(BTLeaf* src, BTLeaf* val);

	FORCEINLINE BTLeaf* FindMinHelper(BTLeaf* node) const;
	FORCEINLINE BTLeaf* FindMaxHelper(BTLeaf* node) const;
	FORCEINLINE BTLeaf* FindHelper(BTLeaf* node, const T& value) const;

	FORCEINLINE BTLeaf* RemoveHelper(const T& value, BTLeaf* node);
};

#include "BST.inl"

template<typename T, typename Alloc_t = MultiPoolAlloc>
using BST = BinarySearchTree<T, Alloc_t>;

template<typename T, typename Alloc_t = MultiPoolAlloc>
using BSTree = BinarySearchTree<T, Alloc_t>;

TRE_NS_END