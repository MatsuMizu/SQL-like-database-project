#include "list2.h"

# ifndef command_H
# define command_H
template <class T>
class database;
class command : public record
{
private:
	command_type type = command_type::none;
	condition c_name = condition::none;
	condition c_phone = condition::none;
	condition c_group = condition::none;
	operation op = operation::none;
	char spaces[max_symbol_code] = {};
public:
	ordering print_order[max_items] = { ordering::none, ordering::none, ordering::none };
	ordering sort_order[max_items] = { ordering::none, ordering::none, ordering::none };
	command() = default;
	~command() = default;
	command(command_type in_type, condition in_c_name, condition in_c_phone, condition in_c_group, operation in_op)
	{
		type = in_type;
		c_name = in_c_name;
		c_phone = in_c_phone;
		c_group = in_c_group;
		op = in_op;
	}
	void reset();
	void reset_fields();
	void command_from_record(record* x) {
		name = std::move(x->name);
		phone = x->phone;
		group = x->group;
		hash_phone = x->hash_phone;
		x->name = nullptr;
	}
	command_type get_type() { return type; }
	condition get_c_name() { return c_name; }
	condition get_c_phone() { return c_phone; }
	condition get_c_group() { return c_group; }
	operation get_op() { return op; }
	bool SEARCH_parse_print_arguments(char* buf, int& print_arguments_read, size_t j);
	bool SEARCH_parse_where_arguments(char* buf, int& command_arguments_read, int& commands_read, ordering& field, bool& nott);
	bool SEARCH_parse_sort_arguments(char* buf, int& print_arguments_read, size_t j);
	bool SEARCH_parse_operation(char* buf);
	bool SEARCH_parse_field(char* buf, ordering& field);
	bool SEARCH_parse_condition(char* buf, int& command_arguments_read, ordering& field, bool& nott);
	bool SEARCH_parse_data(char* buf, ordering& field);
	bool SEARCH_parse(const char* string, size_t str_len, size_t& i, char* buf);
	bool DELETE_parse(const char* string, size_t str_len, size_t& i, char* buf);
	bool INSERT_parse(const char* string, size_t str_len, size_t& i, char* buf);
	bool parse(const char* string, size_t str_len, size_t& i, char* buf);
	void print(FILE* fp = stdout) const;
	io_status make_net_no_tree(list1* net, list2* lss);
	bool check_search_conditions_no_field(ordering field, const record& x);
	bool check_search_conditions(const record& x);
	size_t chop(const char* string, size_t str_len, size_t& i, char* buf);
	void get_spaces(const char* string) {
		size_t str_len = strlen(string);
		for (size_t i = 0; i < max_symbol_code && i <= str_len; i++) {
			spaces[(unsigned char)string[i]] = string[i];
		}
		return;
	}
	list1_node* merge_sort(list1_node* c);
	list1_node* merge(list1_node* a, list1_node* b);
};
# endif
