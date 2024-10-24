#include "command_list.h"

void command_list::DELETE_elem_from_tree_array(command* cmd, database<list1>* dbb) {
	int i = record::hash_by_phone(cmd->get_phone(), dbb->get_k());
	DELETE_elem_from_tree(cmd, dbb, dbb->get_hash_trees()[i], ordering::phone);
	return;
}
bool command_list::DELETE_elem_from_tree(command* cmd, database<list1>* dbb, avl_tree<list1>* tree, ordering field) {
	child child_position = child::none;
	avl_tree_node<list1>* trr_root = tree->get_root();
	if (!trr_root) return false;
	bool deleted_in_branch = false;
	avl_tree_node<list1>* dbb_new_root = dbb->find_and_delete_elem(tree, trr_root, trr_root, cmd, &child_position, &deleted_in_branch, field);
	tree->set_root(dbb_new_root);
	return deleted_in_branch;
}
io_status command_list::DELETE_elem_from_structures(command* cmd, command* extra_cmd, database<list1>* dbb) {
	DELETE_elem_from_tree(extra_cmd, dbb, dbb->get_tree(), ordering::name);
	DELETE_elem_from_tree_array(cmd, dbb);
	return io_status::success;
}
io_status command_list::INSERT_apply(command* cmd, huge_database<list1>* dbb) {
	if (!dbb) {
		printf("Void database!\n");
		return io_status::create;
	}
	if (cmd->group >= groups_amount || cmd->group < 0) {
		printf("Can't add such group to a database, group should be from 0 to %d\n", groups_amount - 1);
		return io_status::format;
	}
	list2_node* new_node = new list2_node;
	if (new_node == nullptr) {
		printf("Not enough memory for data!\n");
		return io_status::memory;
	}
	if (new_node->init(cmd->name.get(), cmd->phone, cmd->group, record::hash_by_phone(cmd->phone, dbb->get_k())) != io_status::success) {
		printf("Not enough memory for data!\n");
		return io_status::memory;
	}
	bool already_in = false;
	if (dbb->add_elem(new_node, true, &already_in) != io_status::success) {
		delete new_node;
		return io_status::memory;
	}
	if (already_in) delete new_node;
	else {
		dbb->get_list()->set_length(dbb->get_list()->get_length() + 1);
	}
	return io_status::success;
}
io_status command_list::SEARCH_apply(command* cmd, command* extra_cmd, huge_database<list1>* dbb, FILE* output, int& amount) {
	bool net_needed = cmd->sort_order[0] != ordering::none;
	if (cmd->get_type() == command_type::del) {
		if (cmd->get_c_name() == condition::none && cmd->get_c_phone() == condition::none && cmd->get_c_group() == condition::none) {
			dbb->get_list()->erase_list();
			dbb->get_list()->set_head(nullptr);
			dbb->get_list()->set_tail(nullptr);
			dbb->get_list()->set_length(0);
			avl_tree<list1>::delete_subtree(dbb->get_tree()->get_root());
			dbb->get_tree()->set_root(nullptr);
			return io_status::success;
		}
		net_needed = true;
	}
	io_status res = io_status::success;
	list1* net = nullptr;
	if (net_needed) {
		net = new list1();
		if (!net) {
			printf("Not enough memory!\n");
			return io_status::memory;
		}
	}
	field_shouldnt_satisfy nonfield = field_shouldnt_satisfy::none;
	if (cmd->get_op() == operation::lor) {
		if (cmd->get_c_name() == condition::ne || cmd->get_c_name() == condition::like || cmd->get_c_name() == condition::nlike) {
			res = SEARCH_in_list(cmd, dbb, output, amount, net_needed, net);
		}
		else {
			if (cmd->get_c_group() != condition::none) {
				res = SEARCH_in_groups(cmd, dbb, output, amount, net_needed, net);
				nonfield = field_shouldnt_satisfy_utils::bit_or(nonfield, field_shouldnt_satisfy::group);
			}
			if (cmd->get_c_phone() != condition::none && cmd->get_c_phone() != condition::ne) {
				res = SEARCH_in_phone(cmd, dbb, output, amount, net_needed, net, nonfield);
				nonfield = field_shouldnt_satisfy_utils::bit_or(nonfield, field_shouldnt_satisfy::phone);
			}
			if (cmd->get_c_name() != condition::none) {
				res = SEARCH_in_name(cmd, dbb, output, amount, net_needed, net, nonfield);
			}
		}
	}
	else if (cmd->get_c_group() != condition::none) {
		res = SEARCH_in_groups(cmd, dbb, output, amount, net_needed, net);
	}
	else if (cmd->get_c_phone() != condition::none && cmd->get_c_phone() != condition::ne) {
		res = SEARCH_in_phone(cmd, dbb, output, amount, net_needed, net, nonfield);
	}
	else if (!(cmd->get_c_name() == condition::none || cmd->get_c_name() == condition::ne || cmd->get_c_name() == condition::like || cmd->get_c_name() == condition::nlike)) {
		res = SEARCH_in_name(cmd, dbb, output, amount, net_needed, net, nonfield);
	}
	else {
		res = SEARCH_in_list(cmd, dbb, output, amount, net_needed, net);
	}
	if (res != io_status::success) {
		if (net) delete net;
		return res;
	}
	if (cmd->sort_order[0] != ordering::none) {
		net->set_head(cmd->merge_sort(net->get_head()));
		list1_node* curr = net->get_head();
		while (curr) {
			curr->print(cmd->print_order, output);
			amount++;
			fputc('\n', output);
			curr = curr->get_next();
		}
		delete net;
		return io_status::success;
	}
	else if (net_needed) {
		list1_node* curr = net->get_head(), * next = nullptr;
		while (curr) {
			extra_cmd->reset_fields();
			extra_cmd->command_from_record(curr->get_elem());
			DELETE_elem_from_structures(extra_cmd, extra_cmd, dbb);
			DELETE_elem_from_structures(extra_cmd, extra_cmd, dbb->get_groups()[extra_cmd->get_group()]);
			next = curr->get_next();
			dbb->get_list()->delete_node(curr->get_elem());
			delete curr;
			curr = next;
		}
		net->set_head(nullptr);
		delete net;
	}
	return io_status::success;
}
io_status command_list::SEARCH_in_group(command* cmd, database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net) {
	field_shouldnt_satisfy nonfield = field_shouldnt_satisfy::none;
	if (cmd->get_op() == operation::land) {
		if (cmd->get_c_phone() != condition::none && cmd->get_c_phone() != condition::ne) {
			return SEARCH_in_phone(cmd, dbb, output, amount, sort_needed, net, nonfield);
		}
		else if (!(cmd->get_c_name() == condition::none || cmd->get_c_name() == condition::ne || cmd->get_c_name() == condition::like || cmd->get_c_name() == condition::nlike)) {
			return SEARCH_in_name(cmd, dbb, output, amount, sort_needed, net, nonfield);
		}
	}
	child child_position = child::none;
	return dbb->pick_elems_from_tree(dbb->get_tree()->get_root(), cmd, &child_position, sort_needed, amount, ordering::group, field_shouldnt_satisfy::none, net, output);
}
io_status command_list::SEARCH_in_name(command* cmd, database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net, field_shouldnt_satisfy nonfield) {
	return SEARCH_in_tree(cmd, dbb->get_tree(), ordering::name, output, amount, sort_needed, net, nonfield);
}
io_status command_list::SEARCH_in_phone(command* cmd, database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net, field_shouldnt_satisfy nonfield) {
	if (cmd->get_c_phone() == condition::eq) {
		return SEARCH_in_tree(cmd, dbb->get_hash_trees()[record::hash_by_phone(cmd->get_phone(), dbb->get_k())], ordering::phone, output, amount, sort_needed, net, nonfield);
	}
	for (int i = 0; i < dbb->get_hash_trees_len(); i++) {
		SEARCH_in_tree(cmd, dbb->get_hash_trees()[i], ordering::phone, output, amount, sort_needed, net, nonfield);
	}
	return io_status::success;
}
io_status command_list::SEARCH_in_tree(command* cmd, avl_tree<list1>* trr, ordering field, FILE* output, int& amount, bool sort_needed, list1* net, field_shouldnt_satisfy nonfield) {
	child child_position = child::none;
	return database<list1>::pick_elems_from_tree(trr->get_root(), cmd, &child_position, sort_needed, amount, field, nonfield, net, output);
}
io_status command_list::SEARCH_in_list(command* cmd, huge_database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net) {
	if (sort_needed) {
		return cmd->make_net_no_tree(net, dbb->get_list());
	}
	list2_node* curr = dbb->get_list()->get_head();
	while (curr) {
		if (cmd->check_search_conditions(*curr)) {
			curr->print(cmd->print_order, output);
			fputc('\n', output);
			amount++;
		}
		curr = curr->get_next();
	}
	return io_status::success;
}
io_status command_list::SEARCH_group_found(command* cmd, database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net) {
	if (cmd->get_op() == operation::lor) {
		child child_position = child::none;
		return dbb->pick_elems_from_tree(dbb->get_tree()->get_root(), cmd, &child_position, sort_needed, amount, ordering::group, field_shouldnt_satisfy::none, net, output);
	}
	return SEARCH_in_group(cmd, dbb, output, amount, sort_needed, net);
}
io_status command_list::SEARCH_in_groups(command* cmd, huge_database<list1>* dbb, FILE* output, int& amount, bool sort_needed, list1* net) {
	if (cmd->get_c_group() == condition::ne) {
		for (int i = 0; i < groups_amount; i++) {
			if (i != cmd->get_group()) {
				SEARCH_group_found(cmd, dbb->get_groups()[i], output, amount, sort_needed, net);
			}
		}
	}
	else if (cmd->get_c_group() == condition::ge) {
		for (int i = cmd->get_group(); i < groups_amount; i++) {
			SEARCH_group_found(cmd, dbb->get_groups()[i], output, amount, sort_needed, net);
		}
	}
	else if (cmd->get_c_group() == condition::gt) {
		for (int i = cmd->get_group() + 1; i < groups_amount; i++) {
		    SEARCH_group_found(cmd, dbb->get_groups()[i], output, amount, sort_needed, net);
		}
	}
	else if (cmd->get_c_group() == condition::lt) {
		for (int i = 0; i < cmd->get_group() && i < groups_amount; i++) {
			SEARCH_group_found(cmd, dbb->get_groups()[i], output, amount, sort_needed, net);
		}
	}
	else if (cmd->get_c_group() == condition::le) {
		for (int i = 0; i <= cmd->get_group() && i < groups_amount; i++) {
			SEARCH_group_found(cmd, dbb->get_groups()[i], output, amount, sort_needed, net);
		}
	}
	else if (cmd->get_c_group() == condition::eq) {
		if (cmd->get_group() >= 0 && cmd->get_group() < groups_amount) SEARCH_group_found(cmd, dbb->get_groups()[cmd->get_group()], output, amount, sort_needed, net);
	}
	return io_status::success;
}
io_status command_list::apply(command* cmd, command* extra_cmd, huge_database<list1>* dbb, FILE* output, int& amount) {
	if (cmd->get_c_name() == condition::like || cmd->get_c_name() == condition::nlike) {
		io_status res = cmd->make_map(cmd->name.get());
		if (res != io_status::success) {
			if (res == io_status::memory) {
				printf("Not enough memory to process special symbols\n");
			}
			return io_status::memory;
		}
	}
	switch (cmd->get_type()) {
	case (command_type::del): return SEARCH_apply(cmd, extra_cmd, dbb, output, amount);
	case (command_type::insert): return INSERT_apply(cmd, dbb);
	case (command_type::select): return SEARCH_apply(cmd, extra_cmd, dbb, output, amount);
	default: return io_status::success;
	}
	return io_status::success;
}