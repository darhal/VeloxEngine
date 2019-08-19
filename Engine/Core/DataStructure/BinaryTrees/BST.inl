template<typename T, typename Alloc_t>
FORCEINLINE BinarySearchTree<T, Alloc_t>::BinarySearchTree() : BinaryTree<T, Alloc_t>()
{
}

template<typename T, typename Alloc_t>
FORCEINLINE BinarySearchTree<T, Alloc_t>::~BinarySearchTree()
{
	// It will call base class dtor!
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
	BTLeaf* res = this->FindMinHelper(BTree::m_Root);
	ASSERTF((res == NULL), "Attempt to search an empty tree!");
	return res->element;
}

template<typename T, typename Alloc_t>
FORCEINLINE T& BinarySearchTree<T, Alloc_t>::FindMax() const
{
	BTLeaf* res = this->FindMaxHelper(BTree::m_Root);
	ASSERTF((res == NULL), "Attempt to search an empty tree!");
	return res->element;
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinarySearchTree<T, Alloc_t>::BTLeaf* BinarySearchTree<T, Alloc_t>::FindHelper(BTLeaf* node, const T& value) const
{
	if (node == NULL)
		return NULL;
	else if (value < node->element)
		return FindHelper(node->left, value);
	else if (value > node->element)
		return FindHelper(node->right, value);
	else
		return node;
}

template<typename T, typename Alloc_t>
FORCEINLINE bool BinarySearchTree<T, Alloc_t>::Find(const T& value) const
{
	BTLeaf* res = this->FindHelper(BTree::m_Root, value);
	return res != NULL;
}

template<typename T, typename Alloc_t>
FORCEINLINE void BinarySearchTree<T, Alloc_t>::InsertHelper(BTLeaf* src, BTLeaf* val)
{
	if (src->element > val->element) {
		if (src->left == NULL) {
			src->left = val;
		}else{
			InsertHelper(src->left, val);
		}
	}else{
		if (src->right == NULL) {
			src->right = val;
		}else{
			InsertHelper(src->right, val);
		}
	}
}

template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE typename BinarySearchTree<T, Alloc_t>::BTLeaf& BinarySearchTree<T, Alloc_t>::Insert(Args&&... args)
{
	if (BTree::m_Root == NULL) {
		BTree::m_Root = BTree::m_Allocator.template Allocate<BTLeaf>();
		new (BTree::m_Root) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
		return *BTree::m_Root;
	}
	auto newLeaf = BTree::m_Allocator.template Allocate<BTLeaf>();
	new (newLeaf) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
	this->InsertHelper(BTree::m_Root, newLeaf);
	return *newLeaf;
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinarySearchTree<T, Alloc_t>::BTLeaf* BinarySearchTree<T, Alloc_t>::RemoveHelper(const T& value, BTLeaf* node)
{
	BTLeaf* temp;
	if (node == NULL)
		return NULL;
	else if (value < node->element)
		node->left = this->RemoveHelper(value, node->left);
	else if (value > node->element)
		node->right = this->RemoveHelper(value, node->right);
	else if (node->left && node->right) {
		temp = this->FindMinHelper(node->right);
		node->element = temp->element;
		node->right = this->RemoveHelper(node->element, node->right);
	}else {
		temp = node;
		if (node->left == NULL)
			node = node->right;
		else if (node->right == NULL)
			node = node->left;
		BTree::m_Allocator.Deallocate(temp);
	}

	return node;
}

template<typename T, typename Alloc_t>
FORCEINLINE void BinarySearchTree<T, Alloc_t>::Remove(const T& value)
{
	BTree::m_Root = this->RemoveHelper(value, BTree::m_Root);
}