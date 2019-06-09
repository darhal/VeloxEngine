#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

template<typename T, typename Alloc_t = MultiPoolAlloc>
class BinaryTree
{
public:

	struct BinaryTreeNode {
	public:
		template<typename... Args>
		BinaryTreeNode(BinaryTreeNode* r, BinaryTreeNode* l, Args&&... args) : right(r), left(l), element(std::forward<Args>(args)...)
		{}

		BinaryTreeNode* GetRight() { return right; }
		BinaryTreeNode* GetLeft() { return left; }
		T& GetElement() { return element; }

		void SetRight(BinaryTreeNode* r) { right = r; }
		void SetLeft(BinaryTreeNode* l) { left = l; }

		template<typename... Args>
		T& EmplaceRight(Args&&... args){
			right = m_Allocator.Allocate<BTNode>(1);
			new (right) BTNode(NULL, NULL, std::forward<Args>(args)...);
			return *right;
		}

		template<typename... Args>
		T& EmplaceLeft(Args&&... args){
			left = m_Allocator.Allocate<BTNode>(1);
			new (left) BTNode(NULL, NULL, std::forward<Args>(args)...);
			return *left;
		}

	private:

		T element;
		BinaryTreeNode* right;
		BinaryTreeNode* left;

		friend class BinaryTree<T, Alloc_t>;
	};

	typedef BinaryTreeNode BTNode;

public:
	template<typename... Args>
	BinaryTree(Args&&... args);

	FORCEINLINE BTNode* GetRoot() const;
	FORCEINLINE BTNode* GetRight(BTNode* cur = NULL) const;
	FORCEINLINE BTNode* GetLeft(BTNode* cur = NULL) const;

	// TODO: Modify it to make the add add to last non null node!
	template<typename... Args>
	FORCEINLINE BTNode* AddRight(BTNode* cur, Args&&... args);
	template<typename... Args>
	FORCEINLINE BTNode* AddLeft(BTNode* cur, Args&&... args);
	template<typename... Args>
	FORCEINLINE BTNode* SetRoot(Args&&... args);

private:
	static const usize NODE_CHUNKS = 3;

	BTNode* m_Root;
	Alloc_t m_Allocator;
};

template<typename T, typename Alloc_t>
template<typename... Args>
BinaryTree<T, Alloc_t>::BinaryTree(Args&&... args) : m_Allocator(sizeof(BTNode), NODE_CHUNKS)
{
	m_Root = m_Allocator.Allocate<BTNode>();
	new (m_Root) BTNode(NULL, NULL, std::forward<Args>(args)...);
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTNode* BinaryTree<T, Alloc_t>::GetRoot() const
{
	return m_Root;
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTNode* BinaryTree<T, Alloc_t>::GetRight(BTNode* cur) const
{
	if (cur == NULL) {
		if (m_Root == NULL) return NULL;
		return m_Root->right;
	}
	return cur->right;
	
}

template<typename T, typename Alloc_t>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTNode* BinaryTree<T, Alloc_t>::GetLeft(BTNode* cur) const
{
	if (cur == NULL) {
		if (m_Root == NULL) return NULL;
		return m_Root->left;
	}
	return cur->left;
}

template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTNode*  BinaryTree<T, Alloc_t>::AddRight(BTNode* cur, Args&&... args)
{
	if (cur == NULL) cur = m_Root;
	cur->right = m_Allocator.Allocate<BTNode>();
	new (cur->right) BTNode(NULL, NULL, std::forward<Args>(args)...);
	return cur->right;
}

template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTNode*  BinaryTree<T, Alloc_t>::AddLeft(BTNode* cur, Args&&... args)
{
	if (cur == NULL) cur = m_Root;
	cur->left = m_Allocator.Allocate<BTNode>();
	new (cur->left) BTNode(NULL, NULL, std::forward<Args>(args)...);
	return cur->left;
}

template<typename T, typename Alloc_t>
template<typename... Args>
FORCEINLINE typename BinaryTree<T, Alloc_t>::BTNode* BinaryTree<T, Alloc_t>::SetRoot(Args&& ...args)
{
	if (m_Root == NULL) {
		m_Root = m_Allocator.Allocate<BTNode>();
		new (m_Root) BTNode(NULL, NULL, std::forward<Args>(args)...);
		return m_Root;
	}
	m_Root->element = T(std::forward<Args>(args)...);
	return m_Root;
}


TRE_NS_END