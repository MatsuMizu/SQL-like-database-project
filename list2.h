#include "record.h"

# ifndef list_2
# define list_2
template <class T> class avl_tree;
class list2_node : public record
{
private:
	list2_node* next = nullptr;
	list2_node* prev = nullptr;
public:
	list2_node() = default;
	list2_node(const list2_node& x) = delete;
	list2_node(list2_node&& x) : record((record&&)x) {
		next = x.next;
		prev = x.prev;
		x.next = nullptr;
		x.prev = nullptr;
	}
	~list2_node() {
		next = nullptr;
		prev = nullptr;
	}
	list2_node& operator= (const list2_node& x) = delete;
	list2_node& operator= (list2_node&& x)
	{
		next = x.next;
		prev = x.prev;
		x.next = nullptr;
		x.prev = nullptr;
		return *this;
	}
	list2_node* get_next() const {
		return this->next;
	}
	list2_node* get_prev() const {
		return this->prev;
	}
	void set_next(list2_node* pot_next) {
		this->next = pot_next;
	}
	void set_prev(list2_node* pot_prev) {
		this->prev = pot_prev;
	}
	friend class list2;
	template <class T>
	friend class avl_tree_node;
};
class list2
{
private:
	list2_node* head = nullptr;
	list2_node* tail = nullptr;
	unsigned int length = 0;
public:
	list2() = default;
	list2(const list2& x) = delete;
	list2& operator= (const list2& r) = delete;
	~list2() {
		erase_list();
	}
	list2_node* get_head() { return head; }
	list2_node* get_tail() { return tail; }
	unsigned int get_length() { return length; }
	void set_head(list2_node* x) { head = x; }
	void set_tail(list2_node* x) { tail = x; }
	void set_length(unsigned int x) { length = x; }
	void print(unsigned int r = 10, FILE* fp = stdout);
	void erase_list() {
		list2_node* curr = nullptr, * next = nullptr;
		if (head) {
			for (curr = head; curr; curr = next) {
				next = curr->next;
				delete curr;
			}
		}
	}
	list2_node* delete_node(list2_node* curr);
};
# endif

# ifndef list_1
# define list_1
class list1_node
{
private:
	list2_node* elem = nullptr;
	list1_node* next = nullptr;
public:
	list1_node() = default;
	list1_node(const list1_node& x) = delete;
	list1_node(list1_node&& x) {
		elem = x.elem;
		x.elem = nullptr;
		next = x.next;
		x.next = nullptr;
	}
	~list1_node() {
		elem = nullptr;
		next = nullptr;
	}
	list1_node& operator= (const list1_node& x) = delete;
	list1_node& operator= (list1_node&& x)
	{
		elem = x.elem;
		x.elem = nullptr;
		next = x.next;
		x.next = nullptr;
		return *this;
	}
	list1_node* get_next() const { return next; }
	void set_next(list1_node* pot_next) { next = pot_next; }
	list2_node* get_elem() const { return elem; }
	void set_elem(list2_node* x) { elem = x; }
	void print(ordering print_order[] = 0, FILE* fp = stdout) {
		elem->print(print_order, fp);
	}
	list1_node* adaptive_add_element(list2_node* element)
	{
		if (!elem)
		{
			elem = element;
			return this;
		}
		else
		{
			next = new list1_node();
			next->elem = element;
			return next;
		}
	}
	friend class list1;
	template <class T>
	friend class avl_tree_node;
	template <class T>
	friend class avl_tree;
};
class list1
{
private:
	list1_node* head = nullptr;
	list1_node* tail = nullptr;
public:
	list1() = default;
	list1(const list1& x) = delete;
	list1& operator= (const list1& r) = delete;
	~list1() {
		list1_node* curr = nullptr, * next = nullptr;
		if (head) {
			for (curr = head; curr; curr = next) {
				next = curr->next;
				delete curr;
			}
		}
		head = nullptr;
		tail = nullptr;
	}
	list1_node* get_head() { return head; }
	void set_head(list1_node* x) { head = x; }
	list1_node* get_tail() { return tail; }
	void set_tail(list1_node* x) { tail = x; }
	void print(unsigned int r = 10, FILE* fp = stdout);
	void add_node(list1_node* x, bool check_uniqueness = false, bool* already_in = nullptr);
    void adaptive_add_element(list2_node* x);
    void destroy_elements_in_list2_and_crear_list1(list2* list, bool destroy_list);
	template <class T>
	friend class avl_tree_node;
};
# endif