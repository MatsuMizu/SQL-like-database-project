#include "database.h"
#include "command.h"

# ifndef command_list_H
# define command_list_H
class command_list
{
public:
	static io_status apply(command* cmd, command* extra_cmd, huge_database<list1>* dbb, FILE* output, int& amount);
	static bool DELETE_elem_from_tree(command* cmd, database<list1>* dbb, avl_tree<list1>* tree, ordering field);
	static void DELETE_elem_from_tree_array(command* cmd, database<list1>* dbb);
	static io_status DELETE_elem_from_structures(command* cmd, command* extra_cmd, database<list1>* dbb);
	static io_status INSERT_apply(command* cmd, huge_database<list1>* dbb);
	static io_status SEARCH_apply(command* cmd, command* extra_cmd, huge_database<list1>* dbb, FILE* output, int& amount);
	static io_status SEARCH_in_groups(command* cmd, huge_database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net);
	static io_status SEARCH_group_found(command* cmd, database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net);
	static io_status SEARCH_in_phone(command* cmd, database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net, field_shouldnt_satisfy nonfield);
	static io_status SEARCH_in_name(command* cmd, database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net, field_shouldnt_satisfy nonfield);
	static io_status SEARCH_in_list(command* cmd, huge_database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net);
	static io_status SEARCH_in_tree(command* cmd, avl_tree<list1>* trr, ordering field, FILE* output, int& amount, bool sort_needed, list1* net, field_shouldnt_satisfy nonfield);
	static io_status SEARCH_in_group(command* cmd, database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net);
};
# endif