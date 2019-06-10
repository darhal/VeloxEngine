#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

template<typename T, typename Alloc_t = MultiPoolAlloc>
class BinaryTree
{
public:
	struct BinaryTreeLeaf {
	public:
		template<typename... Args>
		BinaryTreeLeaf(BinaryTreeLeaf* r, BinaryTreeLeaf* l, Args&&... args) : right(r), left(l), element(std::forward<Args>(args)...)
		{}

		BinaryTreeLeaf* GetRight() { return right; }
		BinaryTreeLeaf* GetLeft() { return left; }
		T& GetElement() { return element; }

		void SetRight(BinaryTreeLeaf* r) { right = r; }
		void SetLeft(BinaryTreeLeaf* l) { left = l; }

		template<typename... Args>
		T& EmplaceRight(Args&&... args){
			right = m_Allocator.Allocate<BTLeaf>();
			new (right) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
			return *right;
		}

		template<typename... Args>
		T& EmplaceLeft(Args&&... args){
			left = m_Allocator.Allocate<BTLeaf>();
			new (left) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
			return *left;
		}

	private:
		T element;
		BinaryTreeLeaf* right;
		BinaryTreeLeaf* left;

		friend class BinaryTree<T, Alloc_t>;
	};

	typedef BinaryTreeLeaf BTLeaf;
	typedef BinaryTreeLeaf BTNode;
public:
	FORCEINLINE BinaryTree();

	template<typename... Args>
	FORCEINLINE BinaryTree(Args&&... args);

	FORCEINLINE ~BinaryTree();

	FORCEINLINE BTLeaf* GetRoot() const;
	FORCEINLINE BTLeaf* GetRight(BTLeaf* cur = NULL) const;
	FORCEINLINE BTLeaf* GetLeft(BTLeaf* cur = NULL) const;

	template<typename... Args>
	FORCEINLINE BTLeaf* InsertRight(BTLeaf* cur, Args&&... args);
	template<typename... Args>
	FORCEINLINE BTLeaf* InsertLeft(BTLeaf* cur, Args&&... args);
	template<typename... Args>
	FORCEINLINE BTLeaf* SetRoot(Args&&... args);

	template<typename... Args>
	FORCEINLINE BTLeaf* InsertTopRight(Args&&... args);
	template<typename... Args>
	FORCEINLINE BTLeaf* InsertTopLeft(Args&&... args);

	FORCEINLINE void RemoveRight(BTLeaf* cur);
	FORCEINLINE void RemoveLeft(BTLeaf* cur);
	FORCEINLINE void RemoveNode(BTLeaf* cur);

	FORCEINLINE BTLeaf* RightRotate(BTLeaf* parent, BTLeaf* cur);
	FORCEINLINE BTLeaf* LeftRotate(BTLeaf* parent, BTLeaf* cur);

	FORCEINLINE usize MaxDepth(BTLeaf* cur = NULL);
	FORCEINLINE usize Height(BTLeaf* cur);

	FORCEINLINE usize Count(BTLeaf* cur);

	FORCEINLINE void Clear();

protected:
	FORCEINLINE void DestroyTree(BTLeaf* cur);

	static const usize NODE_CHUNKS = 3;

	BTLeaf* m_Root;
	Alloc_t m_Allocator;
};

template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE BinaryTree<T, Alloc_t>::BinaryTree(Args&&... args) : m_Allocator(sizeof(BTLeaf), NODE_CHUNKS)
{
	m_Root = m_Allocator.Allocate<BTLeaf>();
	new (m_Root) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
}

template<typename T, typename Alloc_t>
FORCEINLINE BinaryTree<T, Alloc_t>::BinaryTree() : m_Root(NULL), m_Allocator(sizeof(BTLeaf), NODE_CHUNKS)
{
}

template<typename T, typename Alloc_t>
FORCEINLINE BinaryTree<T, Alloc_t>::~BinaryTree()
{
	this->DestroyTree(m_Root);
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTLeaf* BinaryTree<T, Alloc_t>::GetRoot() const
{
	return m_Root;
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTLeaf* BinaryTree<T, Alloc_t>::GetRight(BTLeaf* cur) const
{
	if (cur == NULL) {
		if (m_Root == NULL) return NULL;
		return m_Root->right;
	}
	return cur->right;
	
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTLeaf* BinaryTree<T, Alloc_t>::GetLeft(BTLeaf* cur) const
{
	if (cur == NULL) {
		if (m_Root == NULL) return NULL;
		return m_Root->left;
	}
	return cur->left;
}

template<typename T, typename Alloc_t>
FORCEINLINE void BinaryTree<T, Alloc_t>::RemoveRight(BTLeaf* cur)
{
	ASSERTF(!(cur == NULL), "Attempt to remove a null node!");
	this->RemoveNode(cur->right);
	cur->right = NULL;
}

template<typename T, typename Alloc_t>
FORCEINLINE void BinaryTree<T, Alloc_t>::RemoveLeft(BTLeaf* cur)
{
	ASSERTF(!(cur == NULL), "Attempt to remove a null node!");
	this->RemoveNode(cur->left);
	cur->left = NULL;
}

template<typename T, typename Alloc_t>
FORCEINLINE void BinaryTree<T, Alloc_t>::RemoveNode(BTLeaf* cur)
{
	if (cur != NULL) {
		RemoveNode(cur->left);
		RemoveNode(cur->right);
		m_Allocator.Deallocate(cur);
	}
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTLeaf* BinaryTree<T, Alloc_t>::RightRotate(BTLeaf* parent, BTLeaf* cur)
{
	ASSERTF(!(parent == NULL || cur == NULL), "Attempt to perform rotation with NULL parent or NULL leaf!");
	auto left = cur->left;
	cur->left = left->right;
	left->right = cur;
	if (parent != NULL) {
		if (parent->right == cur) {
			parent->right = left;
		}
		else if (parent->left == cur) {
			parent->left = left;
		}
	}
	return left;
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTLeaf* BinaryTree<T, Alloc_t>::LeftRotate(BTLeaf* parent, BTLeaf* cur)
{
	ASSERTF(!(parent == NULL || cur == NULL), "Attempt to perform rotation with NULL parent or NULL leaf!");
	auto right = cur->right;
	cur->right = right->left;
	right->left = cur;
	if (parent != NULL) {
		if (parent->right == cur) {
			parent->right = right;
		}
		else if (parent->left == cur) {
			parent->left = right;
		}
	}
	return right;
}

template<typename T, typename Alloc_t>
FORCEINLINE usize BinaryTree<T, Alloc_t>::MaxDepth(BTLeaf* cur)
{
	if (cur == NULL) cur = m_Root;
	if (cur == NULL)
		return 0;
	else
	{
		// compute the depth of each subtree 
		usize lDepth = this->MaxDepth(cur->left);
		usize rDepth = this->MaxDepth(cur->right);

		// use the larger one
		return (lDepth >= rDepth) ? (lDepth + 1) : (rDepth + 1);
	}
}

template<typename T, typename Alloc_t>
FORCEINLINE usize BinaryTree<T, Alloc_t>::Height(BTLeaf* cur)
{
	if (cur == NULL)
		return 0;

	// find the height of each subtree
	int lh = this->Height(cur->left);
	int rh = this->Height(cur->right);

	return 1 + ((lh >= rh) ? lh : rh);
}

template<typename T, typename Alloc_t>
FORCEINLINE usize BinaryTree<T, Alloc_t>::Count(BTLeaf* cur)
{
	if (m_Root == NULL) return 0;
	else return 1 + this->Count(m_Root->left) + this->Count(m_Root->right);
}

template<typename T, typename Alloc_t>
FORCEINLINE void BinaryTree<T, Alloc_t>::Clear()
{
	this->RemoveNode(m_Root);
}

template<typename T, typename Alloc_t>
FORCEINLINE void BinaryTree<T, Alloc_t>::DestroyTree(BTLeaf* cur)
{
	this->RemoveNode(cur);
}

template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTLeaf*  BinaryTree<T, Alloc_t>::InsertRight(BTLeaf* cur, Args&&... args)
{
	if (cur == NULL) cur = m_Root;
	ASSERTF(!(m_Root == NULL), "Attempt to insert in a tree with a NULL root!");
	/*if (cur == NULL) {
		this->SetRoot(std::forward<Args>(args)...);
		return;
	}*/
	cur->right = m_Allocator.Allocate<BTLeaf>();
	new (cur->right) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
	return cur->right;
}

template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTLeaf*  BinaryTree<T, Alloc_t>::InsertLeft(BTLeaf* cur, Args&&... args)
{
	if (cur == NULL) cur = m_Root;
	ASSERTF(!(m_Root == NULL), "Attempt to insert in a tree with a NULL root!");
	/*if (cur == NULL) {
		this->SetRoot(std::forward<Args>(args)...);
		return;
	}*/
	cur->left = m_Allocator.Allocate<BTLeaf>();
	new (cur->left) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
	return cur->left;
}

template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTLeaf* BinaryTree<T, Alloc_t>::SetRoot(Args&& ...args)
{
	if (m_Root == NULL) {
		m_Root = m_Allocator.Allocate<BTLeaf>();
		new (m_Root) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
		return m_Root;
	}
	m_Root->element = T(std::forward<Args>(args)...);
	return m_Root;
}

template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTLeaf* BinaryTree<T, Alloc_t>::InsertTopRight(Args&& ...args)
{
	ASSERTF(!(m_Root == NULL), "Attempt to insert in a tree with a NULL root!");
	BTLeaf* cur = m_Root;
	while (cur->right != NULL) {
		cur = cur->right;
	}
	cur->right = m_Allocator.Allocate<BTLeaf>();
	new (cur->right) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
	return cur->right;
}

template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTLeaf* BinaryTree<T, Alloc_t>::InsertTopLeft(Args&& ...args)
{
	ASSERTF(!(m_Root == NULL), "Attempt to insert in a tree with a NULL root!");
	BTLeaf* cur = m_Root;
	while (cur->left != NULL) {
		cur = cur->left;
	}
	cur->left = m_Allocator.Allocate<BTLeaf>();
	new (cur->left) BTLeaf(NULL, NULL, std::forward<Args>(args)...);
	return cur->left;
}


TRE_NS_END