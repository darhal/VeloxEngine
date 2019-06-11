#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <sstream>

TRE_NS_START

using std::vector;
using std::string;
using std::cout;
using std::stringstream;

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

	public:
		T element;
		BinaryTreeLeaf* right;
		BinaryTreeLeaf* left;

		friend class BinaryTree<T, Alloc_t>;
	};

	typedef BinaryTreeLeaf BTLeaf;
	typedef BinaryTreeLeaf BTNode;
	typedef BinaryTreeLeaf Node;
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

	FORCEINLINE usize MaxDepth(BTLeaf* cur = NULL) const;
	FORCEINLINE usize Height(BTLeaf* cur) const;

	FORCEINLINE usize Count(BTLeaf* cur);

	FORCEINLINE void Clear();

	FORCEINLINE bool IsEmpty() const;

	FORCEINLINE void Print(BTLeaf* node = NULL);
	FORCEINLINE void PrintBT(const String& prefix, const BTLeaf* node, bool isLeft);

	// Dumps a representation of the tree to cout
	void Dump() const {
		const usize d = this->MaxDepth();

		// If this tree is empty, tell someone
		if (d == 0) {
			cout << " <empty tree>\n";
			return;
		}

		// This tree is not empty, so get a list of node values...
		const auto rows_disp = get_row_display();
		// then format these into a text representation...
		auto formatted_rows = row_formatter(rows_disp);
		// then trim excess space characters from the left sides of the text...
		trim_rows_left(formatted_rows);
		// then dump the text to cout.
		for (const auto& row : formatted_rows) {
			std::cout << ' ' << row << '\n';
		}
	}
protected:
	FORCEINLINE void DestroyTree(BTLeaf* cur);
	FORCEINLINE usize MaxDepthHelper(BTLeaf* cur) const;

	static const usize NODE_CHUNKS = 3;

	BTLeaf* m_Root;
	Alloc_t m_Allocator;

	struct cell_display {
		string   valstr;
		bool     present;
		cell_display() : present(false) {}
		cell_display(std::string valstr) : valstr(valstr), present(true) {}
	};

	using display_rows = vector< vector< cell_display > >;

	// The text tree generation code below is all iterative, to avoid stack faults.

	// get_row_display builds a vector of vectors of cell_display structs
	// each vector of cell_display structs represents one row, starting at the root
	display_rows get_row_display() const {
		// start off by traversing the tree to
		// build a vector of vectors of Node pointers
		vector<Node*> traversal_stack;
		vector< std::vector<Node*> > rows;
		if (!m_Root) return display_rows();

		Node* p = m_Root;
		const usize max_depth = this->MaxDepth();
		rows.resize(max_depth);
		int depth = 0;
		for (;;) {
			// Max-depth Nodes are always a leaf or null
			// This special case blocks deeper traversal
			if (depth == max_depth - 1) {
				rows[depth].push_back(p);
				if (depth == 0) break;
				--depth;
				continue;
			}

			// First visit to node?  Go to left child.
			if (traversal_stack.size() == depth) {
				rows[depth].push_back(p);
				traversal_stack.push_back(p);
				if (p) p = p->left;
				++depth;
				continue;
			}

			// Odd child count? Go to right child.
			if (rows[depth + 1].size() % 2) {
				p = traversal_stack.back();
				if (p) p = p->right;
				++depth;
				continue;
			}

			// Time to leave if we get here

			// Exit loop if this is the root
			if (depth == 0) break;

			traversal_stack.pop_back();
			p = traversal_stack.back();
			--depth;
		}

		// Use rows of Node pointers to populate rows of cell_display structs.
		// All possible slots in the tree get a cell_display struct,
		// so if there is no actual Node at a struct's location,
		// its boolean "present" field is set to false.
		// The struct also contains a string representation of
		// its Node's value, created using a std::stringstream object.
		display_rows rows_disp;
		stringstream ss;
		for (const auto& row : rows) {
			rows_disp.emplace_back();
			for (Node* pn : row) {
				if (pn) {
					ss << pn->element;
					rows_disp.back().push_back(cell_display(ss.str()));
					ss = stringstream();
				}
				else {
					rows_disp.back().push_back(cell_display());
				}
			}
		}
		return rows_disp;
	}

	// row_formatter takes the vector of rows of cell_display structs 
	// generated by get_row_display and formats it into a test representation
	// as a vector of strings
	vector<string> row_formatter(const display_rows& rows_disp) const {
		using s_t = string::size_type;

		// First find the maximum value string length and put it in cell_width
		s_t cell_width = 0;
		for (const auto& row_disp : rows_disp) {
			for (const auto& cd : row_disp) {
				if (cd.present && cd.valstr.length() > cell_width) {
					cell_width = cd.valstr.length();
				}
			}
		}

		// make sure the cell_width is an odd number
		if (cell_width % 2 == 0) ++cell_width;

		// formatted_rows will hold the results
		vector<string> formatted_rows;

		// some of these counting variables are related,
		// so its should be possible to eliminate some of them.
		s_t row_count = rows_disp.size();

		// this row's element count, a power of two
		s_t row_elem_count = 1 << (row_count - 1);

		// left_pad holds the number of space charactes at the beginning of the bottom row
		s_t left_pad = 0;

		// Work from the level of maximum depth, up to the root
		// ("formatted_rows" will need to be reversed when done) 
		for (s_t r = 0; r < row_count; ++r) {
			const auto& cd_row = rows_disp[row_count - r - 1]; // r reverse-indexes the row
			// "space" will be the number of rows of slashes needed to get
			// from this row to the next.  It is also used to determine other
			// text offsets.
			s_t space = (s_t(1) << r) * (cell_width + 1) / 2 - 1;
			// "row" holds the line of text currently being assembled
			string row;
			// iterate over each element in this row
			for (s_t c = 0; c < row_elem_count; ++c) {
				// add padding, more when this is not the leftmost element
				row += string(c ? left_pad * 2 + 1 : left_pad, ' ');
				if (cd_row[c].present) {
					// This position corresponds to an existing Node
					const string& valstr = cd_row[c].valstr;
					// Try to pad the left and right sides of the value string
					// with the same number of spaces.  If padding requires an
					// odd number of spaces, right-sided children get the longer
					// padding on the right side, while left-sided children
					// get it on the left side.
					s_t long_padding = cell_width - valstr.length();
					s_t short_padding = long_padding / 2;
					long_padding -= short_padding;
					row += string(c % 2 ? short_padding : long_padding, ' ');
					row += valstr;
					row += string(c % 2 ? long_padding : short_padding, ' ');
				}
				else {
					// This position is empty, Nodeless...
					row += string(cell_width, ' ');
				}
			}
			// A row of spaced-apart value strings is ready, add it to the result vector
			formatted_rows.push_back(row);

			// The root has been added, so this loop is finsished
			if (row_elem_count == 1) break;

			// Add rows of forward- and back- slash characters, spaced apart
			// to "connect" two rows' Node value strings.
			// The "space" variable counts the number of rows needed here.
			s_t left_space = space + 1;
			s_t right_space = space - 1;
			for (s_t sr = 0; sr < space; ++sr) {
				string row;
				for (s_t c = 0; c < row_elem_count; ++c) {
					if (c % 2 == 0) {
						row += string(c ? left_space * 2 + 1 : left_space, ' ');
						row += cd_row[c].present ? '/' : ' ';
						row += string(right_space + 1, ' ');
					}
					else {
						row += string(right_space, ' ');
						row += cd_row[c].present ? '\\' : ' ';
					}
				}
				formatted_rows.push_back(row);
				++left_space;
				--right_space;
			}
			left_pad += space + 1;
			row_elem_count /= 2;
		}

		// Reverse the result, placing the root node at the beginning (top)
		std::reverse(formatted_rows.begin(), formatted_rows.end());

		return formatted_rows;
	}

	// Trims an equal number of space characters from
	// the beginning of each string in the vector.
	// At least one string in the vector will end up beginning
	// with no space characters.
	static void trim_rows_left(vector<string>& rows) {
		if (!rows.size()) return;
		auto min_space = rows.front().length();
		for (const auto& row : rows) {
			auto i = row.find_first_not_of(' ');
			if (i == string::npos) i = row.length();
			if (i == 0) return;
			if (i < min_space) min_space = i;
		}
		for (auto& row : rows) {
			row.erase(0, min_space);
		}
	}

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
FORCEINLINE usize BinaryTree<T, Alloc_t>::MaxDepth(BTLeaf * cur) const
{
	if (cur == NULL)
		cur = m_Root;
	return MaxDepthHelper(cur);
}

template<typename T, typename Alloc_t>
FORCEINLINE usize BinaryTree<T, Alloc_t>::MaxDepthHelper(BTLeaf* cur) const
{
	if (cur == NULL)
		return 0;
	else
	{
		// compute the depth of each subtree 
		usize lDepth = this->MaxDepthHelper(cur->left);
		usize rDepth = this->MaxDepthHelper(cur->right);

		// use the larger one
		return (lDepth >= rDepth) ? (lDepth + 1) : (rDepth + 1);
	}
}

template<typename T, typename Alloc_t>
FORCEINLINE usize BinaryTree<T, Alloc_t>::Height(BTLeaf* cur) const
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
	m_Root = NULL;
}

template<typename T, typename Alloc_t>
FORCEINLINE bool BinaryTree<T, Alloc_t>::IsEmpty() const
{
	return m_Root == NULL;
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

template<typename T, typename Alloc_t>
FORCEINLINE void BinaryTree<T, Alloc_t>::PrintBT(const String& prefix, const BTLeaf* node, bool isLeft)
{
	if (node != NULL)
	{
		std::cout << prefix;
		const wchar_t* c3 = L"\u2515\u2500\u2500";
		const wchar_t* c4 = L"\u251C\u2500\u2500";   
		std::wcout << (isLeft ? "L--" : "R--");

		// print the value of the node
		std::cout << node->element << std::endl;

		// enter the next tree level - left and right branch
		PrintBT(prefix + (isLeft ? "|   " : "    "), node->left, true);
		PrintBT(prefix + (isLeft ? "|   " : "    "), node->right, false);
	}
}

/*template<typename T, typename Alloc_t>
FORCEINLINE void BinaryTree<T, Alloc_t>::GraphicalPrint(BTLeaf* node, int trace[], int depth)
{
	if (node == NULL)
		return;
	trace[depth] = 0;
	GraphicalPrint(node->left, trace, depth + 4);
	char indentation[256] = "";
	int i = 0;
	while (i < depth) {
		if (i > 0 && trace[i] != trace[i - 1]) {
			indentation[i] = '|';
		}else{
			indentation[i] = ' ';
		}
		i++;
	}
	printf("%s+--", indentation);
	std::cout << node->element << std::endl;
	trace[depth] = 1;
	GraphicalPrint(node->right, trace, depth + 4);
}*/

template<typename T, typename Alloc_t>
FORCEINLINE void BinaryTree<T, Alloc_t>::Print(BTLeaf* node)
{
	if (node == NULL)
		node = m_Root;
	this->PrintBT("", node, false);
}

TRE_NS_END