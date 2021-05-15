/************************************************************/
/*						Public Methods						*/
/************************************************************/

template<typename K, typename T, typename Alloc_t>
FORCEINLINE RedBalckTree<K, T, Alloc_t>::RedBalckTree(const Alloc_t& allocator) : m_Root(TNULL), m_Allocator(::std::move(allocator))
{
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE RedBalckTree<K, T, Alloc_t>::RedBalckTree() : m_Root(TNULL), m_Allocator(sizeof(RBNode), NODE_CHUNKS)
{
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE RedBalckTree<K, T, Alloc_t>::RedBalckTree(RedBalckTree<K, T, Alloc_t>&& other) : 
	m_Root(other.m_Root), m_Allocator(::std::move(other.m_Allocator))
{
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE RedBalckTree<K, T, Alloc_t>::RedBalckTree(const RedBalckTree<K, T, Alloc_t>& other) : 
	m_Root(NULL), m_Allocator(sizeof(RBNode), NODE_CHUNKS)
{
	for(auto& pair : other){
		this->Insert(pair.first, pair.second);
	}
}

template<typename K, typename T, typename Alloc_t>
RedBalckTree<K, T, Alloc_t>& RedBalckTree<K, T, Alloc_t>::operator=(RedBalckTree<K, T, Alloc_t>&& other)
{
	this->DestroyTree(m_Root);
	m_Root = other.m_Root; 
	m_Allocator = ::std::move(m_Allocator);
	return *this;
}

template<typename K, typename T, typename Alloc_t>
RedBalckTree<K, T, Alloc_t>& RedBalckTree<K, T, Alloc_t>::operator=(const RedBalckTree<K, T, Alloc_t>& other)
{
	this->DestroyTree(m_Root);
	for (auto& pair : other) {
		this->Insert(pair.first, pair.second);
	}

	return *this;
}

template<typename K, typename T, typename Alloc_t>
void RedBalckTree<K, T, Alloc_t>::CopyNodeRecursice(RBNode* dst_node, RBNode* src_node)
{
}

template<typename K, typename T, typename Alloc_t>
template<typename ...Args>
FORCEINLINE RedBalckTree<K, T, Alloc_t>::RedBalckTree(const K& key, Args&&... args) : m_Root(TNULL), m_Allocator(sizeof(RBNode), NODE_CHUNKS)
{
	m_Root = m_Allocator.template Allocate<RBNode>();
	new (m_Root) RBNode(NULL, NULL, NULL, key, ::std::forward<Args>(args)...);
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE RedBalckTree<K, T, Alloc_t>::~RedBalckTree()
{
	this->DestroyTree(m_Root);
}

template<typename K, typename T, typename Alloc_t>
template<typename ...Args>
FORCEINLINE T& RedBalckTree<K, T, Alloc_t>::Insert(const K& key, Args&& ...args)
{
	// Ordinary Binary Search Insertion
	RBNode* node = m_Allocator.template Allocate<RBNode>();
	new (node) RBNode(NULL, TNULL, TNULL, key, ::std::forward<Args>(args)...);

	RBNode* y = NULL;
	RBNode* x = m_Root;

	while (x != TNULL) {
		y = x;
		if (Compare(node->key, x->key)) { // quivalent to (node->key < x->key)
			x = x->left;
		}else {
			x = x->right;
		}
	}

	// y is parent of x
	node->parent = y;
	if (y == NULL) {
		m_Root = node;
	}else if (Compare(node->key, y->key)) { // quivalent to (node->key < y->key)
		y->left = node;
	}else {
		y->right = node;
	}

	// if new node is a root node, simply return
	if (node->parent == NULL) {
		node->color = false;
		return node->value;
	}

	// if the grandparent is null, simply return
	if (node->parent->parent == NULL) {
		return node->value;
	}

	// Fix the tree
	this->FixInsert(node);

	return node->value;
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE typename RedBalckTree<K, T, Alloc_t>::RBNode* RedBalckTree<K, T, Alloc_t>::Search(const K& key) const
{
	return SearchTreeHelper(this->m_Root, key);
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE void RedBalckTree<K, T, Alloc_t>::Remove(const K& key)
{
	DeleteNodeHelper(this->m_Root, key);
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE void RedBalckTree<K, T, Alloc_t>::LeftRotate(RBNode* x)
{
	RBNode* y = x->right;
	x->right = y->left;

	if (y->left != TNULL) {
		y->left->parent = x;
	}

	y->parent = x->parent;

	if (x->parent == NULL) {
		this->m_Root = y;
	}else if (x == x->parent->left) {
		x->parent->left = y;
	}else {
		x->parent->right = y;
	}

	y->left = x;
	x->parent = y;
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE void RedBalckTree<K, T, Alloc_t>::RightRotate(RBNode* x)
{
	RBNode* y = x->left;
	x->left = y->right;

	if (y->right != TNULL) {
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
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE void RedBalckTree<K, T, Alloc_t>::Print()
{
	if (m_Root) {
		printf("--------------------------\n");
		PrintHelper(m_Root, "", true);
	}
}

template<typename K, typename T, typename Alloc_t>
typename RedBalckTree<K, T, Alloc_t>::RBNode* RedBalckTree<K, T, Alloc_t>::Min(RBNode* node) {
	while (node->left != TNULL) {
		node = node->left;
	}
	return node;
}

template<typename K, typename T, typename Alloc_t>
typename RedBalckTree<K, T, Alloc_t>::RBNode* RedBalckTree<K, T, Alloc_t>::Max(RBNode* node) {
	while (node->right != TNULL) {
		node = node->right;
	}
	return node;
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE void RedBalckTree<K, T, Alloc_t>::Clear()
{
	this->DestroyTree(m_Root);
	m_Root = TNULL;
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE bool RedBalckTree<K, T, Alloc_t>::IsEmpty() const
{
	return m_Root == TNULL;
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE typename RedBalckTree<K, T, Alloc_t>::Iterator RedBalckTree<K, T, Alloc_t>::begin() noexcept
{
	RBNode* node = m_Root;

	while (node) {
		RBNode* next = node->left;

		if (next == NULL) {
			break;
		}

		node = next;
	}

	return Iterator(this, node);
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE typename RedBalckTree<K, T, Alloc_t>::Iterator RedBalckTree<K, T, Alloc_t>::end() noexcept
{
	return Iterator(this, NULL);
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE typename RedBalckTree<K, T, Alloc_t>::CIterator RedBalckTree<K, T, Alloc_t>::begin() const noexcept
{	
	RBNode* node = m_Root;

	while (node) {
		RBNode* next = node->left;

		if (next == NULL) {
			break;
		}

		node = next;
	}

	return CIterator(this, node);
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE typename RedBalckTree<K, T, Alloc_t>::CIterator RedBalckTree<K, T, Alloc_t>::end() const noexcept
{
	return CIterator(this, NULL);
}


template<typename K, typename T, typename Alloc_t>
FORCEINLINE typename RedBalckTree<K, T, Alloc_t>::CIterator RedBalckTree<K, T, Alloc_t>::cbegin() const noexcept
{
	RBNode* node = m_Root;

	while (node) {
		RBNode* next = node->left;

		if (next == NULL) {
			break;
		}

		node = next;
	}

	return CIterator(this, node);
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE typename RedBalckTree<K, T, Alloc_t>::CIterator RedBalckTree<K, T, Alloc_t>::cend() const noexcept
{
	return CIterator(this, NULL);
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE typename RedBalckTree<K, T, Alloc_t>::Iterator RedBalckTree<K, T, Alloc_t>::rbegin() noexcept
{
	RBNode* node = m_Root;

	while (node) {
		RBNode* next = node->right;

		if (next == NULL) {
			break;
		}

		node = next;
	}

	return Iterator(this, node);
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE typename RedBalckTree<K, T, Alloc_t>::Iterator RedBalckTree<K, T, Alloc_t>::rend() noexcept
{
	return Iterator(this, NULL);
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE typename RedBalckTree<K, T, Alloc_t>::CIterator RedBalckTree<K, T, Alloc_t>::crbegin() const noexcept
{
	RBNode* node = m_Root;

	while (node) {
		RBNode* next = node->right;

		if (next == NULL) {
			break;
		}

		node = next;
	}

	return CIterator(this, node);
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE typename RedBalckTree<K, T, Alloc_t>::CIterator RedBalckTree<K, T, Alloc_t>::crend() const noexcept
{
	return CIterator(this, NULL);
}


/************************************************************/
/*					Private Methods							*/
/************************************************************/

template<typename K, typename T, typename Alloc_t>
FORCEINLINE void RedBalckTree<K, T, Alloc_t>::InsertHelper(RBNode* newNode, RBNode* parent)
{
	if (Compare(newNode->key, parent->key)) {  // newNode->key < parent->key 
		if (parent->left == NULL) {
			parent->left = newNode;
			newNode->parent = parent;
			return;
		}
		return InsertHelper(newNode, parent->left);
	}

	if (parent->right == NULL) {
		parent->right = newNode;
		newNode->parent = parent;
		return;
	}
	return InsertHelper(newNode, parent->right);
}

template<typename K, typename T, typename Alloc_t>
void RedBalckTree<K, T, Alloc_t>::DestroyTree(RBNode* node)
{
	if (node != NULL) {
		DestroyTree(node->left);
		DestroyTree(node->right);
		m_Allocator.Deallocate(node);
	}
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE void RedBalckTree<K, T, Alloc_t>::InitNullNode(RBNode* node, RBNode* parent)
{
	node->parent = parent;
	node->left = NULL;
	node->right = NULL;
	node->color = false;
}

template<typename K, typename T, typename Alloc_t>
typename RedBalckTree<K, T, Alloc_t>::RBNode* RedBalckTree<K, T, Alloc_t>::SearchTreeHelper(RBNode* node, const K& key) const
{
	if (node == TNULL || key == node->key) {
		return node;
	}

	if (Compare(key, node->key)) { // key < node->key
		return SearchTreeHelper(node->left, key);
	}

	return SearchTreeHelper(node->right, key);
}

template<typename K, typename T, typename Alloc_t>
void RedBalckTree<K, T, Alloc_t>::FixDelete(RBNode* x)
{
	RBNode* s;
	while (x != m_Root && x->color == false) {
		if (x == x->parent->left) {
			s = x->parent->right;
			if (s->color == true) {
				// case 3.1
				s->color = 0;
				x->parent->color = 1;
				LeftRotate(x->parent);
				s = x->parent->right;
			}

			if (s->left->color == false && s->right->color == false) {
				// case 3.2
				s->color = true;
				x = x->parent;
			}else {
				if (s->right->color == false) {
					// case 3.3
					s->left->color = false;
					s->color = true;
					RightRotate(s);
					s = x->parent->right;
				}

				// case 3.4
				s->color = x->parent->color;
				x->parent->color = false;
				s->right->color = false;
				LeftRotate(x->parent);
				x = m_Root;
			}
		}else {
			s = x->parent->left;
			if (s->color == true) {
				// case 3.1
				s->color = false;
				x->parent->color = true;
				RightRotate(x->parent);
				s = x->parent->left;
			}

			if (s->right->color == false) {
				// case 3.2
				s->color = true;
				x = x->parent;
			}else {
				if (s->left->color == false) {
					// case 3.3
					s->right->color = false;
					s->color = true;
					LeftRotate(s);
					s = x->parent->left;
				}

				// case 3.4
				s->color = x->parent->color;
				x->parent->color = false;
				s->left->color = false;
				RightRotate(x->parent);
				x = m_Root;
			}
		}
	}
	if (x)
		x->color = false;
}

template<typename K, typename T, typename Alloc_t>
FORCEINLINE void RedBalckTree<K, T, Alloc_t>::RBTransplant(RBNode* u, RBNode* v)
{
	if (u->parent == NULL) {
		m_Root = v;
	}else if (u == u->parent->left) {
		u->parent->left = v;
	}else {
		u->parent->right = v;
	}
	if (v)
		v->parent = u->parent;
}

template<typename K, typename T, typename Alloc_t>
void RedBalckTree<K, T, Alloc_t>::DeleteNodeHelper(RBNode* node, const K& key)
{
	// find the node containing key
	RBNode* z = TNULL;
	RBNode* x = NULL;
	RBNode* y = NULL;

	while (node != TNULL) {
		if (!Compare(node->key, key) && !Compare(key, node->key)) { // node->key == key
			z = node;
		}

		if (Compare(node->key, key)) { // !(node->key > key)
			node = node->right;
		}else {
			node = node->left;
		}
	}

	if (z == TNULL) {
		return;
	}

	y = z;
	bool y_original_color = y->color;
	if (z->left == TNULL) {
		x = z->right;
		this->RBTransplant(z, z->right);
	}else if (z->right == TNULL) {
		x = z->left;
		this->RBTransplant(z, z->left);
	}else {
		y = this->Min(z->right);
		y_original_color = y->color;
		x = y->right;
		if (y->parent == z) {
			x->parent = y;
		}else {
			this->RBTransplant(y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}

		this->RBTransplant(z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	m_Allocator.Deallocate(z); // delete z;

	if (y_original_color == false) {
		FixDelete(x);
	}
}

template<typename K, typename T, typename Alloc_t>
void RedBalckTree<K, T, Alloc_t>::FixInsert(RBNode* k)
{
	RBNode* u = NULL;
	while (k->parent->color == true) {
		if (k->parent == k->parent->parent->right) {
			u = k->parent->parent->left; // uncle
			if (u != NULL && u->color == true) {
				// case 3.1
				u->color = false;
				k->parent->color = false;
				k->parent->parent->color = true;
				k = k->parent->parent;
			}else {
				if (k == k->parent->left) {
					// case 3.2.2
					k = k->parent;
					RightRotate(k);
				}
				// case 3.2.1
				k->parent->color = false;
				k->parent->parent->color = true;
				LeftRotate(k->parent->parent);
			}
		}else {
			u = k->parent->parent->right; // uncle

			if (u != NULL && u->color == true) {
				// mirror case 3.1
				u->color = false;
				k->parent->color = false;
				k->parent->parent->color = true;
				k = k->parent->parent;
			}else {
				if (k == k->parent->right) {
					// mirror case 3.2.2
					k = k->parent;
					LeftRotate(k);
				}
				// mirror case 3.2.1
				k->parent->color = false;
				k->parent->parent->color = true;
				RightRotate(k->parent->parent);
			}
		}

		if (k == m_Root) {
			break;
		}
	}
	m_Root->color = false;
}

template<typename K, typename T, typename Alloc_t>
void RedBalckTree<K, T, Alloc_t>::PrintHelper(RBNode* node, String indent, bool last)
{
	// print the tree structure on the screen
	if (node != NULL) {
		::std::cout << indent;
		
		if (last) {
			::std::cout << "R----";
			indent += String("     ");
		}else {
			::std::cout << "L----";
			indent += String("|    ");
		}

		String sColor = node->color ? String("RED") : String("BLACK");
		::std::cout << node->value << "(" << sColor << ")" << ::std::endl;
		PrintHelper(node->left, indent, false);
		PrintHelper(node->right, indent, true);
	}
}