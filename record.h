#include "condition.h"

# ifndef record_H
# define record_H

enum class io_status
{
	success,
	eof,
	format,
	memory,
	open,
	create,
	empty,
};

class record
{
private:
	std::unique_ptr<char[]> name = nullptr;
	std::unique_ptr<char[]> map = nullptr;
	std::unique_ptr<char[]> low_map = nullptr;
	std::unique_ptr<char[]> high_map = nullptr;
	int phone = 0;
	int group = 0;
	int hash_phone = 0;
public:
	record() = default;
	~record() = default;
	const char* get_name() const { return name.get(); }
	int get_phone() const { return phone; }
	int get_group() const { return group; }
	int get_hash_phone() const { return hash_phone; }
	void set_hash_phone(int x) { hash_phone = x; }
	static int hash_by_phone(int phone, unsigned int k);
	io_status init(const char* n, int p, int g, int h_p);
	record(record&& x) = default;
	record& operator= (record&& x) = default;
	record(const record& x) = delete;
	record& operator= (const record&) = delete;
	bool equal(record& x);
	int cmp(record& x, ordering* sort_order);
	bool compare_name(condition x, const record& y);
	bool compare_phone(condition x, const record& y) const;
	bool compare_group(condition x, const record& y) const;
	void print(ordering print_order[] = 0, FILE* fp = stdout);
	io_status read(unsigned int k, FILE* fp = stdin);
	io_status make_map(const char* string);
	bool accept_symbol(char symb, int position);
	bool compare_without_percent(const char* str);
	bool compare(const char* str);
	static int safe_strcmp(const char* x, const char* y);
	static bool scan_int(const char* buf, int* x);
	friend class command;
	friend class command_list;
	friend class list2_node;
	template <class T>
	friend class avl_tree;
	template <class T>
	friend class database;
};
# endif
