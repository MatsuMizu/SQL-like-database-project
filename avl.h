#include "command.h"

# ifndef avl
# define avl
template <class T> class avl_tree;
template <class T> class avl_tree_node
{
private:
	T* content = nullptr;
	avl_tree_node* left = nullptr;
	avl_tree_node* right = nullptr;
	int balance = 0;
	int branch = 0;
public:
	avl_tree_node() = default;
	avl_tree_node(const avl_tree_node& x) = delete;
	avl_tree_node(avl_tree_node&& x)
	{
		content = x.content; x.content = nullptr;
		left = x.left; x.left = nullptr;
		right = x.right; x.right = nullptr;
		balance = x.balance; x.balance = 0;
		branch = x.branch; x.branch = 0;
	}
	avl_tree_node& operator= (const avl_tree_node& x) = delete;
	avl_tree_node& operator= (avl_tree_node&& x)
	{
		if (this == &x)
			return *this;
		content = x.content; x.content = nullptr;
		left = x.left; x.left = nullptr;
		right = x.right; x.right = nullptr;
		balance = x.balance; x.balance = 0;
		branch = x.branch; x.branch = 0;
	}
	~avl_tree_node()
	{
		delete_node();
	}
	avl_tree_node<T>* get_left() { return left; }
	avl_tree_node<T>* get_right() { return right; }
	void update_balance() {
		int right_res = 0, left_res = 0;
		if (right) right_res = right->branch;
		if (left) left_res = left->branch;
		balance = right_res - left_res;
		branch = (left_res > right_res ? left_res : right_res) + 1;
	}
	void print() {
		list1_node* curr = content->head;
		while (curr) {
			curr->elem->print();
			printf("      ");
			curr = curr->next;
		}
		printf("     bal: %d  dep: %d\n", balance, branch);
	}
private:
	void delete_node()
	{
		delete content;
		left = nullptr;
		right = nullptr;
		balance = 0;
		branch = 0;
	}
	template <class K>
	friend class avl_tree;
	template <class K>
	friend class database;
};
template <class T>
class avl_tree
{
private:
	avl_tree_node<T>* root = nullptr;
public:
	avl_tree() = default;
	avl_tree(const avl_tree<T>& x) = delete;
	avl_tree(avl_tree<T>&& x)
	{
		root = x.root; x.root = nullptr;
	}
	~avl_tree()
	{
		delete_subtree(root);
		root = nullptr;
	}
	avl_tree<T>& operator= (const avl_tree<T>& x) = delete;
	avl_tree<T>& operator= (avl_tree<T>&& x)
	{
		if (this == &x)
			return *this;
		delete_subtree(root);
		root = x.root; x.root = nullptr;
		return *this;
	}
	void print(unsigned int r = 10, FILE* fp = stdout) const
	{
		print_subtree(root, 0, r, fp);
	}
	avl_tree_node<T>* get_root() {
		return root;
	}
	void set_root(avl_tree_node<T>* x) { root = x; }
	io_status read(FILE* fp = stdin)
	{
		avl_tree_node<T> x;
		while (x.read(fp) == io_status::success) {
			add_node(x);
		}
		if (!feof(fp))
			return io_status::eof;
		return io_status::success;
	}
	io_status add_node(list1_node* x, bool check_uniqueness, bool* already_in, ordering field) {
		if (!x) {
			return io_status::success;
		}
		if (!root) {
			avl_tree_node<T>* new_root = new avl_tree_node<T>();
			if (!new_root) return io_status::memory;
			root = new_root;
			root->content = new T();
			if (root->content == nullptr) return io_status::memory;
			root->content->set_head(x);
			root->content->set_tail(x);
			root->branch += 1;
			return io_status::success;
		}
		io_status marker = io_status::success;
		avl_tree_node<T>* new_root = nullptr;
		new_root = add_node_subtree(root, x, &marker, check_uniqueness, already_in, field);
		if (marker != io_status::success) return marker;
		root = new_root;
		return io_status::success;
	}
private:
	avl_tree_node<T>* add_node_subtree(avl_tree_node<T>* curr, list1_node* x, io_status* marker, bool check_uniqueness, bool* already_in, ordering field) {
		if (!x) {
			*marker = io_status::memory;
			return nullptr;
		}
		if (!curr || !curr->content) {
			avl_tree_node<T>* new_node = new avl_tree_node<T>();
			if (!new_node) {
				*marker = io_status::memory;
				return nullptr;
			}
			new_node->branch += 1;
			new_node->content = new T();
			if (new_node->content == nullptr) {
				*marker = io_status::memory;
				return nullptr;
			}
			new_node->content->set_head(x);
			new_node->content->set_tail(x);
			return new_node;
		}
		int cmp_res = cmp_by_field(x->elem, curr->content->get_head()->get_elem(), field);
		if (cmp_res < 0) {
			curr->left = add_node_subtree(curr->left, x, marker, check_uniqueness, already_in, field);
			curr->branch += 1;
		}
		else if (cmp_res > 0) {
			curr->right = add_node_subtree(curr->right, x, marker, check_uniqueness, already_in, field);
			curr->branch += 1;
		}
		else {
			curr->content->add_node(x, check_uniqueness, already_in);
			return curr;
		}
		if (*marker != io_status::success) return nullptr;
		return fix_node(curr);
	}
	static avl_tree_node<T>* fix_node(avl_tree_node<T>* curr) {
		if (!curr) return curr;
		curr->update_balance();
		if (curr->balance >= 2) {
			if (curr->right && curr->right->balance < 0 && curr->right->left) {
				curr->right = rotate_right(curr->right);
			}
			if (curr->right)
			{
				return rotate_left(curr);
			}
		}
		if (curr->balance <= -2)
		{
			if (curr->left && curr->left->balance > 0 && curr->left->right) {
				curr->left = rotate_left(curr->left);
			}
			if (curr->left)
			{
				return rotate_right(curr);
			}
		}
		return curr;
	}
	static avl_tree_node<T>* rotate_left(avl_tree_node<T>* x) {
		if (!x) return nullptr;
		avl_tree_node<T>* right_child = x->right;
		if (!right_child) return nullptr;
		x->right = right_child->left;
		right_child->left = x;
		x->update_balance();
		right_child->update_balance();
		return right_child;
	}
	static avl_tree_node<T>* rotate_right(avl_tree_node<T>* x) {
		if (!x) return nullptr;
		avl_tree_node<T>* left_child = x->left;
		if (!left_child) return nullptr;
		x->left = left_child->right;
		left_child->right = x;
		x->update_balance();
		left_child->update_balance();
		return left_child;
	}
	static void fix_node_safe(avl_tree_node<T>* parent, avl_tree_node<T>* child) {
		if (parent->left == child)
			parent->left = fix_node(child);
		else if (parent->right == child)
			parent->right = fix_node(child);
	}
	static void rotate_left_safe(avl_tree_node<T>* parent, avl_tree_node<T>* child) {
		if (parent->left == child)
			parent->left = rotate_left(child);
		else if (parent->right == child)
			parent->right = rotate_left(child);
	}
	static void rotate_right_safe(avl_tree_node<T>* parent, avl_tree_node<T>* child) {
		if (parent->left == child)
			parent->left = rotate_right(child);
		else if (parent->right == child)
			parent->right = rotate_right(child);
	}
	static void delete_subtree(avl_tree_node<T>* curr) {
		if (curr == nullptr)
			return;
		delete_subtree(curr->left);
		delete_subtree(curr->right);
		delete curr;
	}
	static void print_subtree(avl_tree_node<T>* curr, int level, int r, FILE* fp = stdout) {
		if (curr == nullptr || level > r)
			return;
		int spaces = level * 2;
		for (int i = 0; i < spaces; i++)
			printf(" ");
		curr->print();
		printf("\n");
		//printf(" %d\n", curr->branch);
		print_subtree(curr->left, level + 1, r, fp);
		print_subtree(curr->right, level + 1, r, fp);
	}
	template <class K>
	friend class database;
	friend class command;
	friend class command_list;
public:
	static int get_subtree_size(avl_tree_node<T>* curr) {
		return (curr ? (1 + get_subtree_size(curr->left) + get_subtree_size(curr->right)) : 0);
	}
	static int cmp_by_field(record* x, record* y, ordering field) {
		if (field == ordering::name) {
			if (x->get_name() == nullptr) return 0;
			return record::safe_strcmp(x->get_name(), y->get_name());
		}
		else if (field == ordering::phone) {
			return x->get_phone() - y->get_phone();
		}
		else return 0;
	}
};


# endif
