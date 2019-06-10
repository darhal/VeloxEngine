#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
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

	template<typename... Args>
	FORCEINLINE BTLeaf& Insert(Args&&... args);

	FORCEINLINE void Remove(const T& value);

	FORCEINLINE T& FindMin() const;
	FORCEINLINE T& FindMax() const;

	FORCEINLINE T& Find(BTLeaf* node, T& value) const;
private:
	FORCEINLINE void InsertHelper(BTLeaf* src, BTLeaf val);

	FORCEINLINE BTLeaf* FindMinHelper(BTLeaf* node) const;
	FORCEINLINE BTLeaf* FindMaxHelper(BTLeaf* node) const;
	FORCEINLINE BTLeaf* FindHelper(BTLeaf* node, T& value) const;

	FORCEINLINE BTLeaf* RemoveHelper(T& value, BTLeaf* node);
};

template<typename T, typename Alloc_t>
FORCEINLINE BinarySearchTree<T, Alloc_t>::BinarySearchTree() : BinaryTree<T, Alloc_t>()
{
}

template<typename T, typename Alloc_t>
template<typename ...Args>
FORCEINLINE BinarySearchTree<T, Alloc_t>::BinarySearchTree(Args&&... args) : BinaryTree<T, Alloc_t>(std::forward<Args>(args)...)
{
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinarySearchTree<T, Alloc_t>::BTLeaf* BinarySearchTree<T, Alloc_t>::FindMinHelper(BTLeaf* node) const
{
	if (node == NULL)
		return NULL;
	else if (node->left == NULL)
		return node;
	else
		return this->FindMinHelper(node->left);
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinarySearchTree<T, Alloc_t>::BTLeaf* BinarySearchTree<T, Alloc_t>::FindMaxHelper(BTLeaf* node) const
{
	if (node == NULL)
		return NULL;
	else if (node->right == NULL)
		return node;
	else
		return this->FindMaxHelper(node->right);
}


template<typename T, typename Alloc_t>
FORCEINLINE T& BinarySearchTree<T, Alloc_t>::FindMin() const
{
	BTLeaf* res = this->FindMinHelper(BTLeaf::m_Root);
	ASSERTF(!(res == NULL), "Attempt to search an empty tree!");
	return res->element;
}

template<typename T, typename Alloc_t>
FORCEINLINE T& BinarySearchTree<T, Alloc_t>::FindMax() const
{
	BTLeaf* res = this->FindMinHelper(BTLeaf::m_Root);
	ASSERTF(!(res == NULL), "Attempt to search an empty tree!");
	return res->element;
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinarySearchTree<T, Alloc_t>::BTLeaf* BinarySearchTree<T, Alloc_t>::FindHelper(BTLeaf* node, T& value) const
{
	if (node == NULL)
		return NULL;
	else if (value < node->element)
		return Find(node->left, value);
	else if (value > node->element)
		return Find(node->right, value);
	else
		return node;
}

template<typename T, typename Alloc_t>
FORCEINLINE T& BinarySearchTree<T, Alloc_t>::Find(BTLeaf* node, T& value) const
{
	BTLeaf* res = this->FindHelper(BTLeaf::m_Root);
	ASSERTF(!(res == NULL), "Attempt to search an empty tree!");
	return res->element;
}

template<typename T, typename Alloc_t>
FORCEINLINE void BinarySearchTree<T, Alloc_t>::InsertHelper(BTLeaf * src, BTLeaf val)
{
	if (BTree::m_Root->element > val->element) {
		if (BTree::m_Root->left == NULL) {
			BTree::m_Root->left = val;
		}else{
			InsertHelper(BTree::m_Root->left, val);
		}
	}else{
		if (BTree::m_Root->right == NULL) {
			BTree::m_Root->right = val;
		}else{
			InsertHelper(BTree::m_Root->right, val);
		}
	}
}

template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE typename BinarySearchTree<T, Alloc_t>::BTLeaf& BinarySearchTree<T, Alloc_t>::Insert(Args&&... args)
{
	if (BTree::m_Root == NULL) {
		BTree::m_Root = BTree::m_Allocator.Allocate<BTLeaf>();
		new (BTree::m_Root) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
		return *BTree::m_Root;
	}
	auto newLeaf = BTree::m_Allocator.Allocate<BTLeaf>();
	new (newLeaf) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
	this->InsertHelper(BTree::m_Root, newLeaf);
	return *newLeaf;
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinarySearchTree<T, Alloc_t>::BTLeaf* BinarySearchTree<T, Alloc_t>::RemoveHelper(T& value, BTLeaf* node)
{
	BTLeaf* temp;
	if (node == NULL)
		return NULL;
	else if (value < node->element)
		node->left = this->RemoveHelper(value, node->left);
	else if (value > node->element)
		node->right = this->RemoveHelper(value, node->right);
	else if (node->left && node->right){
		temp = this->FindMinHelper(node->right);
		node->element = temp->element;
		node->right = this->RemoveHelper(node->element, node->right);
	}else{
		temp = node;
		if (node->left == NULL)
			node = node->right;
		else if (node->right == NULL)
			node = node->left;
		BTLeaf::m_Allocator.Deallocate(temp);
	}

	return node;
}

template<typename T, typename Alloc_t>
FORCEINLINE void BinarySearchTree<T, Alloc_t>::Remove(const T& value)
{
	BTLeaf::m_Root = this->RemoveHelper(value, BTLeaf::m_Root);
}

template<typename T, typename Alloc_t = MultiPoolAlloc>
using BST = BinarySearchTree<T, Alloc_t>;

TRE_NS_END