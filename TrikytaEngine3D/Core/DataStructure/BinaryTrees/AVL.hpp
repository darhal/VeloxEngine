#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

template<typename T, typename Alloc_t = MultiPoolAlloc>
class AVL
{
public:
	struct AVLNode;
public:
	AVL();
	// find the node with the minimum key
	AVLNode* Min(AVLNode* node);

	// find the node with the maximum key
	AVLNode* Max(AVLNode* node);

	template<typename... Args>
	void Insert(Args&&... data);
private:
	struct AVLNode
	{
		template<typename... Args>
		AVLNode(AVLNode* parent, AVLNode* left, AVLNode* right, Args&&... args) : 
			parent(parent), left(left), right(right), 
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

	AVLNode* m_Root;
	Alloc_t m_Allocator;

	AVLNode* SearchTreeHelper(AVLNode* node, const T& data);
	AVLNode* DeleteNodeHelper(AVLNode* node, const T& data);

	// update the balance factor the node
	void UpdateBalance(AVLNode* node);

	// Rebalance the tree
	void Rebalance(AVLNode* node);

	// rotate left at node x
	void LeftRotate(AVLNode* x);

	// rotate right at node x
	void RightRotate(AVLNode* x);
};

template<typename T, typename Alloc_t>
AVL<T, Alloc_t>::AVL()
{

}

template<typename T, typename Alloc_t>
FORCEINLINE typename AVL<T, Alloc_t>::AVLNode* AVL<T, Alloc_t>::SearchTreeHelper(AVLNode* node, const T& data)
{
	if (node == NULL || Compare(data, node->data) && Compare(node->data, data)) {
		return node;
	}

	if (Compare(data, node->data)) {
		return SearchTreeHelper(node->left, data);
	}
	return SearchTreeHelper(node->right, data);
}

template<typename T, typename Alloc_t>
FORCEINLINE typename AVL<T, Alloc_t>::AVLNode* AVL<T, Alloc_t>::DeleteNodeHelper(AVLNode* node, const T& data)
{
	// search the key
	if (node == NULL) return node;
	else if (Compare(data, node->data)) node->left = DeleteNodeHelper(node->left, data);
	else if (Compare(node->data, data)) node->right = DeleteNodeHelper(node->right, data);
	else {
		// the key has been found, now delete it

		// case 1: node is a leaf node
		if (node->left == NULL && node->right == NULL) {
			m_Allocator.Deallocate(node); // delete node;
			node = NULL;
		}

		// case 2: node has only one child
		else if (node->left == NULL) {
			AVLNode* temp = node;
			node = node->right;
			m_Allocator.Deallocate(temp); // delete temp;
		}

		else if (node->right == NULL) {
			AVLNode* temp = node;
			node = node->left;
			m_Allocator.Deallocate(temp); // delete temp;
		}

		// case 3: has both children
		else {
			AVLNode* temp = minimum(node->right);
			node->data = temp->data;
			node->right = DeleteNodeHelper(node->right, temp->data);
		}

	}

	// Write the update balance logic here 
	// YOUR CODE HERE

	return node;
}


// update the balance factor the node
template<typename T, typename Alloc_t>
void AVL<T, Alloc_t>::UpdateBalance(AVLNode* node) {
	if (node->bf < -1 || node->bf > 1) {
		Rebalance(node);
		return;
	}

	if (node->parent != nullptr) {
		if (node == node->parent->left) {
			node->parent->bf -= 1;
		}

		if (node == node->parent->right) {
			node->parent->bf += 1;
		}

		if (node->parent->bf != 0) {
			UpdateBalance(node->parent);
		}
	}
}

// rebalance the tree
template<typename T, typename Alloc_t>
void AVL<T, Alloc_t>::Rebalance(AVLNode* node) {
	if (node->bf > 0) {
		if (node->right->bf < 0) {
			RightRotate(node->right);
			LeftRotate(node);
		}else{
			LeftRotate(node);
		}
	}else if (node->bf < 0) {
		if (node->left->bf > 0) {
			LeftRotate(node->left);
			RightRotate(node);
		}else{
			RightRotate(node);
		}
	}
}

// find the node with the minimum key
template<typename T, typename Alloc_t>
typename AVL<T, Alloc_t>::AVLNode* AVL<T, Alloc_t>::Min(AVLNode* node) 
{
	while (node->left != NULL) {
		node = node->left;
	}
	return node;
}

// find the node with the maximum key
template<typename T, typename Alloc_t>
typename AVL<T, Alloc_t>::AVLNode* AVL<T, Alloc_t>::Max(AVLNode* node)
{
	while (node->right != NULL) {
		node = node->right;
	}
	return node;
}

// rotate left at node x
template<typename T, typename Alloc_t>
void AVL<T, Alloc_t>::LeftRotate(AVLNode* x)
{
	AVLNode* y = x->right;
	x->right = y->left;
	if (y->left != NULL) {
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == NULL) {
		this->root = y;
	}else if (x == x->parent->left) {
		x->parent->left = y;
	}else {
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;

	// update the balance factor
	x->bf = x->bf - 1 - max(0, y->bf);
	y->bf = y->bf - 1 + min(0, x->bf);
}

// rotate right at node x
template<typename T, typename Alloc_t>
void AVL<T, Alloc_t>::RightRotate(AVLNode* x)
{
	AVLNode* y = x->left;
	x->left = y->right;
	if (y->right != NULL) {
		y->right->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == NULL) {
		this->root = y;
	}else if (x == x->parent->right) {
		x->parent->right = y;
	}else{
		x->parent->left = y;
	}
	y->right = x;
	x->parent = y;

	// update the balance factor
	x->bf = x->bf + 1 - min(0, y->bf);
	y->bf = y->bf + 1 + max(0, x->bf);
}

// insert the key to the tree in its appropriate position
template<typename T, typename Alloc_t>
template<typename... Args>
void AVL<T, Alloc_t>::Insert(Args&&... args)
{
	// PART 1: Ordinary BST insert
	AVLNode* node = m_Allocator.Allocate<AVLNode>();
	new (node) AVLNode(NULL, NULL, NULL, std::forward<Args>(args)..);

	AVLNode* y = NULL;
	AVLNode* x = this->m_Root;

	while (x != NULL) {
		y = x;
		if (Compare(node->data, x->data)) {
			x = x->left;
		}else{
			x = x->right;
		}
	}

	// y is parent of x
	node->parent = y;
	if (y == nullptr) {
		m_Root = node;
	}else if (Compare(node->data, y->data)) {
		y->left = node;
	}else{
		y->right = node;
	}

	// PART 2: re-balance the node if necessary
	this->UpdateBalance(node);
}

TRE_NS_END