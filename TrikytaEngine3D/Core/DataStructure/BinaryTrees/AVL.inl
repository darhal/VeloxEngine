#include "AVL.hpp"

template<typename T, typename Alloc_t>
FORCEINLINE AVL<T, Alloc_t>::AVL() : m_Root(NULL), m_Allocator(sizeof(AVLNode), NODE_CHUNKS, false)
{

}

template<typename T, typename Alloc_t>
FORCEINLINE AVL<T, Alloc_t>::~AVL()
{
	this->DestroyTree(m_Root);
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
FORCEINLINE void AVL<T, Alloc_t>::UpdateBalance(AVLNode* node) {
	if (node->bf < -1 || node->bf > 1) {
		this->Rebalance(node);
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
			this->UpdateBalance(node->parent);
		}
	}
}

// rebalance the tree
template<typename T, typename Alloc_t>
FORCEINLINE void AVL<T, Alloc_t>::Rebalance(AVLNode* node) {
	if (node == NULL) return;
	if (node->right && node->bf > 0) {
		if (node->right->bf < 0) {
			this->RightRotate(node->right);
			this->LeftRotate(node);
		}else{
			LeftRotate(node);
		}
	}else if (node->left && node->bf < 0) {
		if (node->left->bf > 0) {
			this->LeftRotate(node->left);
			this->RightRotate(node);
		}else {
			this->RightRotate(node);
		}
	}
}

// find the node with the minimum key
template<typename T, typename Alloc_t>
FORCEINLINE typename AVL<T, Alloc_t>::AVLNode* AVL<T, Alloc_t>::Min(AVLNode* node)
{
	while (node->left != NULL) {
		node = node->left;
	}
	return node;
}

// find the node with the maximum key
template<typename T, typename Alloc_t>
FORCEINLINE typename AVL<T, Alloc_t>::AVLNode* AVL<T, Alloc_t>::Max(AVLNode* node)
{
	while (node->right != NULL) {
		node = node->right;
	}
	return node;
}

// rotate left at node x
template<typename T, typename Alloc_t>
FORCEINLINE void AVL<T, Alloc_t>::LeftRotate(AVLNode* x)
{
	AVLNode* y = x->right;
	x->right = y->left;
	if (y->left != NULL) {
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == NULL) {
		this->m_Root = y;
	}
	else if (x == x->parent->left) {
		x->parent->left = y;
	}
	else {
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;

	// update the balance factor
	x->bf = x->bf - 1 - MAX(0, y->bf);
	y->bf = y->bf - 1 + MIN(0, x->bf);
}

// rotate right at node x
template<typename T, typename Alloc_t>
FORCEINLINE void AVL<T, Alloc_t>::RightRotate(AVLNode* x)
{
	AVLNode* y = x->left;
	x->left = y->right;
	if (y->right != NULL) {
		y->right->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == NULL) {
		this->m_Root = y;
	}
	else if (x == x->parent->right) {
		x->parent->right = y;
	}
	else {
		x->parent->left = y;
	}
	y->right = x;
	x->parent = y;

	// update the balance factor
	x->bf = x->bf + 1 - MIN(0, y->bf);
	y->bf = y->bf + 1 + MAX(0, x->bf);
}

// insert the key to the tree in its appropriate position
template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE void AVL<T, Alloc_t>::Insert(Args&&... args)
{
	// PART 1: Ordinary BST insert
	AVLNode* node = m_Allocator.Allocate<AVLNode>();
	new (node) AVLNode(NULL, NULL, NULL, std::forward<Args>(args)...);

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
	if (y == NULL) {
		m_Root = node;
	}else if (Compare(node->data, y->data)) {
		y->left = node;
	}else {
		y->right = node;
	}

	// PART 2: re-balance the node if necessary
	this->UpdateBalance(node);
}

template<typename T, typename Alloc_t>
FORCEINLINE void AVL<T, Alloc_t>::Remove(const T& data)
{
	AVLNode* deletedNode = this->DeleteNodeHelper(this->m_Root, data);
	m_Allocator.Deallocate(deletedNode);
}

template<typename T, typename Alloc_t>
FORCEINLINE void AVL<T, Alloc_t>::Print()
{
	this->PrintHelper(this->m_Root, "", true);
}

template<typename T, typename Alloc_t>
void AVL<T, Alloc_t>::PrintHelper(AVLNode* root, String indent, bool last)
{
	// print the tree structure on the screen
	if (root != NULL) {
		std::cout << indent;
		if (last) {
			std::cout << "R----";
			indent += "     ";
		}
		else {
			std::cout << "L----";
			indent += "|    ";
		}

		std::cout << root->data << "( BF = " << root->bf << ")" << std::endl;

		PrintHelper(root->left, indent, false);
		PrintHelper(root->right, indent, true);
	}
}

template<typename T, typename Alloc_t>
FORCEINLINE void AVL<T, Alloc_t>::DestroyTree(AVLNode* cur)
{
	if (cur != NULL) {
		this->DestroyTree(cur->left);
		this->DestroyTree(cur->right);
		m_Allocator.Deallocate(cur);
	}
}
