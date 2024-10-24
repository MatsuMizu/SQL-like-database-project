#include "avl.h"
#include "command.h"

# ifndef data
# define data
template <class T>
class huge_database;
template <class T>
class database
{
private:
	avl_tree<T>* trr = nullptr;
	avl_tree<T>** hash_trrs = nullptr;
	int k = 0;
	int hash_trrs_len = 0;
	int group = 0;
public:
	database() = default;
	database(int t) {
		k = t;
		trr = new avl_tree<T>();
		hash_trrs_len = int(pow(10, k));
		hash_trrs = new avl_tree<T>*[hash_trrs_len];
		for (int i = 0; i < hash_trrs_len; i++) {
			hash_trrs[i] = new avl_tree<T>();
		}
	}
	database(const database& x) = delete;
	database& operator= (const database& r) = delete;
	~database() {
		if (trr) {
			delete trr;
			trr = nullptr;
		}
		if (hash_trrs) {
			for (int i = 0; i < hash_trrs_len; i++) {
				delete hash_trrs[i];
			}
			delete[] hash_trrs;
			hash_trrs = nullptr;
		}
	}
	avl_tree<T>* get_tree() { return trr; }
	avl_tree<T>** get_hash_trees() { return hash_trrs; }
	int get_k() { return k; }
	int get_hash_trees_len() { return hash_trrs_len; }
	static search_directions get_search_direction(int compare_result, condition compare_condition) {
		search_directions return_direction = search_directions::none;
		if (((compare_result > 0) && (compare_condition == condition::gt || compare_condition == condition::ge)) ||
			((compare_result == 0) && (compare_condition == condition::eq || compare_condition == condition::ge || compare_condition == condition::le)) ||
			((compare_result < 0) && (compare_condition == condition::lt || compare_condition == condition::le)))
			return_direction = search_directions_utils::bit_or(return_direction, search_directions::here);

		if ((compare_result > 0) ||
			((compare_result == 0) && (compare_condition == condition::lt || compare_condition == condition::le)) ||
			((compare_result < 0) && (compare_condition == condition::lt || compare_condition == condition::le)))
			return_direction = search_directions_utils::bit_or(return_direction, search_directions::left);

		if ((compare_result > 0 && (compare_condition == condition::gt || compare_condition == condition::ge)) ||
			((compare_result == 0) && (compare_condition == condition::gt || compare_condition == condition::ge)) ||
			(compare_result < 0))
			return_direction = search_directions_utils::bit_or(return_direction, search_directions::right);

		return return_direction;
	}
	io_status add_elem_to_db(list2_node* x, bool check_uniqueness, bool* already_in) {
		list1_node* trr_elem = new list1_node();
		if (trr_elem == nullptr) {
			printf("Not enough memory for data!\n");
			return io_status::memory;
		}
		trr_elem->set_elem(x);
		list1_node* hash_trrs_elem = new list1_node();
		if (hash_trrs_elem == nullptr) {
			delete trr_elem;
			printf("Not enough memory for data!\n");
			return io_status::memory;
		}
		hash_trrs_elem->set_elem(x);
		if (trr->add_node(trr_elem, check_uniqueness, already_in, ordering::name) != io_status::success) {
			delete trr_elem;
			delete hash_trrs_elem;
			printf("Not enough memory for data while adding tree nodes!\n");
			return io_status::memory;
		}
		if (*already_in) {
			delete trr_elem;
			delete hash_trrs_elem;
			return io_status::success;
		}
		if (hash_trrs[x->get_hash_phone()]->add_node(hash_trrs_elem, false, already_in, ordering::phone) != io_status::success) {
			delete trr_elem;
			delete hash_trrs_elem;
			printf("Not enough memory for data while adding tree nodes!\n");
			return io_status::memory;
		}
		return io_status::success;
	}
	avl_tree_node<T>* find_and_delete_elem(avl_tree<T>* tree, avl_tree_node<T>* parent, avl_tree_node<T>* curr, command* cmd, child* child_position, bool* deleted_in_branch, ordering field) {
		int cmp_res = 0;
		child save_child_position = child::none;
		condition cond = condition::none;
		if (field == ordering::name) {
			cond = cmd->get_c_name();
		}
		else if (field == ordering::phone) {
			cond = cmd->get_c_phone();
		}
		avl_tree_node<T>* fixed_child = nullptr;
		bool deleted_smth = false;
		if (!parent || !curr || !curr->content || !curr->content->get_head()) return parent;
		cmp_res = avl_tree<list1>::cmp_by_field(curr->content->get_head()->get_elem(), cmd, field);
		if (curr->content->get_head()->get_elem()->get_name() == nullptr) {
			cmp_res = 0;
		}
		search_directions search_direction = get_search_direction(cmp_res, cond);
		if (field == ordering::group) {
			search_direction = search_directions::max;
		}
		if ((search_directions_utils::bit_and(search_direction, search_directions::here)) == search_directions::here) {
			deleted_smth = false;
			if (*child_position == child::none) {
				fixed_child = delete_root(tree, cmd, &deleted_smth, field);
			}
			else {
				fixed_child = delete_child(parent, *child_position, cmd, &deleted_smth, field);
			}
			if (deleted_smth) {
				*deleted_in_branch = true;
				return fixed_child;
			}
		}
		search_directions directions[2] = { search_directions::left , search_directions::right };
		save_child_position = *child_position;
		for (int i = 0; i < 2; i++) {
			if (search_directions_utils::bit_and(search_direction, directions[i]) == directions[i]) {
				*child_position = directions[i] == search_directions::left ? child::left : child::right;
				avl_tree_node<T>* next_curr = directions[i] == search_directions::left ? curr->left : curr->right;
				if (!next_curr) {
					continue;
				}
				avl_tree_node<T>* node_to_fix = find_and_delete_elem(tree, curr, next_curr, cmd, child_position, deleted_in_branch, field);
				if (save_child_position == child::left || save_child_position == child::right)
				{
					avl_tree<T>::fix_node_safe(parent, node_to_fix);
				}
				else
				{
					fixed_child = avl_tree<T>::fix_node(node_to_fix);
					return fixed_child;
				}
#if debug_helpers > 1
				printf("\nSubtree\n");
				trr->print_subtree(parent, 1, 10000);
#endif
			}
		}
		return parent;
	}
	void gently_delete_avl_node(avl_tree_node<T>* node, command* cmd, bool* deleted_smth, ordering field) {
		if (!node || !node->content) return;
		list1_node* deletable_parent = node->content->get_head();
		list1_node* deletable = deletable_parent;
		bool head_stays = false;
		while (deletable_parent) {
			if (!head_stays && cmd->check_search_conditions_no_field(field, *(deletable_parent->get_elem()))) {
				if (deletable_parent == node->content->get_head()) {

					if (deletable_parent == node->content->get_tail()) {
						delete deletable_parent;
						deletable_parent = nullptr;
						*deleted_smth = true;
						node->content->set_head(nullptr);
						node->content->set_tail(nullptr);
						continue;
					}
					else {
						list1_node* got_next = deletable_parent->get_next();
						delete deletable_parent;
						*deleted_smth = true;
						node->content->set_head(got_next);
						deletable_parent = got_next;
						continue;
					}
				}
				else {
					head_stays = true;
				}
			}
			else if (deletable_parent->get_next() && cmd->check_search_conditions_no_field(field, *(deletable_parent->get_next()->get_elem()))) {
				deletable = deletable_parent->get_next();
				deletable_parent->set_next(deletable->get_next());
				if (deletable == node->content->get_tail()) {
					node->content->set_tail(deletable_parent);
				}
				delete deletable;
				deletable = nullptr;
				*deleted_smth = true;
			}
			deletable_parent = deletable_parent ? deletable_parent->get_next() : nullptr;
		}
	}
	void delete_avl_node(avl_tree_node<T>* node, command* cmd, bool* deleted_smth, bool* deleted_node, ordering field) {
		if (cmd->get_op() == operation::lor || cmd->get_op() == operation::none) {
			*deleted_smth = true;
			*deleted_node = true;
			fully_delete_avl_node(node);
			return;
		}
		gently_delete_avl_node(node, cmd, deleted_smth, field);
		if (!node->content->get_head()) {
			*deleted_node = true;
			delete node;
		}
	}
	void fully_delete_avl_node(avl_tree_node<T>* node) {
		if (!node) return;
		if (!node->content) delete node;
		list1_node* subnode = node->content->get_head(), *next_subnode = nullptr;
		while (subnode) {
			next_subnode = subnode->get_next();
			delete subnode;
			subnode = next_subnode;
		}
		node->content->set_head(nullptr);
		node->content->set_tail(nullptr);
		delete node;
	}
	avl_tree_node<T>* delete_root(avl_tree<T>* tree, command* cmd, bool* deleted_smth, ordering field) {
		avl_tree_node<T>*& tree_root = tree->root;
		bool deleted_node = false;
		if (!tree_root->left || !tree_root->right) {
			avl_tree_node<T>* air = !tree_root->left ? tree_root->right : (!tree_root->right ? tree_root->left : nullptr);
			delete_avl_node(tree_root, cmd, deleted_smth, &deleted_node, field);
			if (deleted_node) {
				tree_root = air;
			}
			return tree_root;
		}
		gently_delete_avl_node(tree_root, cmd, deleted_smth, field);
		if (!tree_root->content->get_head()) {
			avl_tree_node<T>* victim = tree_root;
			avl_tree_node<T>* branch_to_save = nullptr;
			bool forget_child = exchange_and_delete_elem(tree_root, tree_root, tree_root->right, child::none, branch_to_save);
			if (forget_child) {
				tree_root->right = branch_to_save;
			}
			fully_delete_avl_node(victim);
			avl_tree_node<T>* fixed_root = avl_tree<T>::fix_node(tree_root);
			return fixed_root;
		}
		return tree->root;
	}
	avl_tree_node<T>* delete_child(avl_tree_node<T>* parent, child child_position, command* cmd, bool* deleted_smth, ordering field) {
		avl_tree_node<T>* victim = nullptr, * air = nullptr;
		bool deleted_node = false;
		if (!parent) return nullptr;
		if (child_position == child::none) return parent;
		victim = child_position == child::left ? parent->left : parent->right;
		avl_tree_node<T>*& node_to_replace = child_position == child::left ? parent->left : parent->right;
		if (!victim) return parent;
		air = !victim->left ? victim->right : (!victim->right ? victim->left : nullptr);
		if (air || (!victim->left && !victim->right)) {
			delete_avl_node(victim, cmd, deleted_smth, &deleted_node, field);
			if (deleted_node) {
				node_to_replace = air;
				parent->update_balance();
			}
			return parent;
		}
		gently_delete_avl_node(victim, cmd, deleted_smth, field);
		if (!victim->content->get_head()) {
			avl_tree_node<T>* branch_to_save = nullptr;
			bool forget_child = exchange_and_delete_elem(parent, victim, victim->right, child_position, branch_to_save);
			if (forget_child) {
				node_to_replace->right = branch_to_save;
			}
			fully_delete_avl_node(victim);
			avl_tree<T>::fix_node_safe(node_to_replace, node_to_replace->right);
		}
#if debug_helpers > 1
		printf("\nSubtree\n");
		trr->print_subtree(parent, 1, 10000);
#endif
		return parent;
	}
	bool exchange_and_delete_elem(avl_tree_node<T>*& parent, avl_tree_node<T>* victim, avl_tree_node<T>* curr, child child_position, avl_tree_node<T>*& branch_to_save) {
		if (!curr->left) {
			branch_to_save = curr->right;
			if (child_position != child::none) {
				avl_tree_node<T>*& node_to_replace = child_position == child::left ? parent->left : parent->right;
				node_to_replace = curr;
			}
			else {
				parent = curr;
			}
			curr->left = victim->left;
			curr->right = victim->right;
			return true;
		}
		bool forget_child = exchange_and_delete_elem(parent, victim, curr->left, child_position, branch_to_save);
		if (forget_child) {
			curr->left = branch_to_save;
			branch_to_save = nullptr;
		}
		avl_tree<T>::fix_node_safe(curr, curr->left);
#if debug_helpers > 1
		printf("\nSubtree\n");
		trr->print_subtree(curr, 1, 10000);
#endif
		return false;
	}
	static io_status pick_elems_from_node(avl_tree_node<T>* node, command* cmd, list1* net, ordering field, field_shouldnt_satisfy nonfield) {
		if (!node) return io_status::success;
		list1_node* curr = node->content->get_head();
		bool satisfies = false;
		while (curr) {
			if (field == ordering::group) {
				satisfies = cmd->check_search_conditions(*(curr->get_elem()));
			}
			else {
				satisfies = cmd->check_search_conditions_no_field(field, *(curr->get_elem()));
			}
			if (field == ordering::none || satisfies) {
				if (field_shouldnt_satisfy_utils::bit_and(field_shouldnt_satisfy::phone, nonfield) == field_shouldnt_satisfy::phone) {
					if (cmd->compare_phone(cmd->get_c_phone(), *(curr->get_elem()))) {
						curr = curr->get_next();
						continue;
					}
				}
				if (field_shouldnt_satisfy_utils::bit_and(field_shouldnt_satisfy::group, nonfield) == field_shouldnt_satisfy::group) {
					if (cmd->compare_group(cmd->get_c_group(), *(curr->get_elem()))) {
						curr = curr->get_next();
						continue;
					}
				}
				list1_node* curr_in_net = new list1_node();
				if (!curr_in_net) {
					printf("Not enough memory for net!\n");
					delete net;
					return io_status::memory;
				}
				curr_in_net->set_elem(curr->get_elem());
				net->add_node(curr_in_net);
			}
			curr = curr->get_next();
		}
		return io_status::success;
	}
	static void print_elems_from_node(avl_tree_node<T>* node, command* cmd, FILE* fp, int& amount, ordering field, field_shouldnt_satisfy nonfield) {
		if (!node) return;
		list1_node* curr = node->content->get_head();
		bool satisfies = false;
		while (curr) {
			if (field == ordering::group) {
				satisfies = cmd->check_search_conditions(*(curr->get_elem()));
			}
			else {
				satisfies = cmd->check_search_conditions_no_field(field, *(curr->get_elem()));
			}
			if (field == ordering::none || satisfies) {
				if (field_shouldnt_satisfy_utils::bit_and(field_shouldnt_satisfy::phone, nonfield) == field_shouldnt_satisfy::phone) {
					if (cmd->compare_phone(cmd->get_c_phone(), *(curr->get_elem()))) {
						curr = curr->get_next();
						continue;
					}
			    }
				if (field_shouldnt_satisfy_utils::bit_and(field_shouldnt_satisfy::group, nonfield) == field_shouldnt_satisfy::group) {
					if (cmd->compare_group(cmd->get_c_group(), *(curr->get_elem()))) {
						curr = curr->get_next();
						continue;
					}
				}
				curr->get_elem()->print(cmd->print_order, fp);
				fputc('\n', fp);
				amount += 1;
			}
			curr = curr->get_next();
		}
	}
	static io_status pick_elems_from_tree(avl_tree_node<T>* curr, command* cmd, child* child_position, bool sort_needed, int& amount, ordering field, field_shouldnt_satisfy nonfield, list1* net = nullptr, FILE* fp = stdout) {
		int cmp_res = 0;
#if debug_helpers
		child save_child_position = child::none;
#endif
		condition cond = condition::none;
		if (field == ordering::name) {
			cond = cmd->get_c_name();
		}
		else if (field == ordering::phone) {
			cond = cmd->get_c_phone();
		}
		if (!curr || !curr->content || !curr->content->get_head()) return io_status::success;
		cmp_res = avl_tree<list1>::cmp_by_field(curr->content->get_head()->get_elem(), cmd, field);
		search_directions search_direction = get_search_direction(cmp_res, cond);
		if (field == ordering::group) {
			search_direction = search_directions::max;
		}
		if (search_directions_utils::bit_and(search_direction, search_directions::here) == search_directions::here) {
			if (sort_needed) {
				if (pick_elems_from_node(curr, cmd, net, field, nonfield) != io_status::success) {
					return io_status::memory;
				}
			}
			else {
				print_elems_from_node(curr, cmd, fp, amount, field, nonfield);
			}
		}
		search_directions directions[2] = { search_directions::left , search_directions::right };
#if debug_helpers
		save_child_position = *child_position;
#endif
		for (int i = 0; i < 2; i++) {
			if (search_directions_utils::bit_and(search_direction, directions[i]) == directions[i]) {
				*child_position = directions[i] == search_directions::left ? child::left : child::right;
				avl_tree_node<T>* next_curr = directions[i] == search_directions::left ? curr->left : curr->right;
				if (!next_curr) {
					continue;
				}
				if (pick_elems_from_tree(next_curr, cmd, child_position, sort_needed, amount, field, nonfield, net, fp) != io_status::success) {
					return io_status::memory;
				}
			}
		}
		return io_status::success;
	}
	template <class K>
	friend class huge_database;
};

template <class T>
class huge_database : public database<T> {
private:
	list2* lss = nullptr;
	database<T>** groups = nullptr;
public:
	huge_database(int t) {
		lss = new list2();
		database<T>::k = t;
		database<T>::trr = new avl_tree<T>();
		database<T>::hash_trrs_len = int(pow(10, database<T>::k));
		database<T>::hash_trrs = new avl_tree<T>*[database<T>::hash_trrs_len];
		for (int i = 0; i < database<T>::hash_trrs_len; i++) {
			database<T>::hash_trrs[i] = new avl_tree<T>();
		}
		groups = new database<T>*[groups_amount];
		for (int i = 0; i < groups_amount; i++) {
			groups[i] = new database<T>(t);
		}
	}
	huge_database(const huge_database & x) = delete;
	~huge_database() {
		if (lss) {
			delete lss;
			lss = nullptr;
		}
		if (database<T>::trr) {
			delete database<T>::trr;
			database<T>::trr = nullptr;
		}
		if (database<T>::hash_trrs) {
			for (int i = 0; i < database<T>::hash_trrs_len; i++) {
				delete database<T>::hash_trrs[i];
			}
			delete[] database<T>::hash_trrs;
			database<T>::hash_trrs = nullptr;
		}
		if (groups) {
			for (int i = 0; i < groups_amount; i++) {
				delete groups[i];
			}
			delete[] groups;
			groups = nullptr;
		}
	}
	huge_database& operator= (const huge_database & x) = delete;
	list2* get_list() { return lss; }
	database<T>** get_groups() { return groups; }
	void print() {
		printf("List of students:\n");
		lss->print(100000);
		printf("\n\n\n");
		printf("Tree of students:\n");
		database<T>::trr->print();
		printf("\n\n\n");
		for (int i = 0; i < int(pow(10, database<T>::k)); i++) {
			if (database<T>::hash_trrs[i]->get_root()) {
				printf("Hash = %d\n", i);
				database<T>::hash_trrs[i]->print();
				printf("\n");
			}
		}
		printf("Groups:\n");
		for (int i = 0; i < groups_amount; i++) {
			if (groups[i]->get_tree()->get_root()) {
				printf("Group = %d\n", i);
				printf("Tree of students:\n");
				groups[i]->trr->print();
				printf("\n\n\n");
				for (int j = 0; j < int(pow(10, database<T>::k)); j++) {
					if (groups[i]->hash_trrs[j]->get_root()) {
						printf("Hash = %d\n", j);
						groups[i]->hash_trrs[j]->print();
						printf("\n");
					}
				}
				printf("\n\n\n");
			}
		}
		printf("\n\n\n");
	}
	io_status add_elem(list2_node* x, bool check_uniqueness, bool* already_in) {
		io_status res = this->database<T>::add_elem_to_db(x, check_uniqueness, already_in);
		if (res != io_status::success || *already_in) {
			return res;
		}
		int x_group = x->get_group();
		if (x_group >= groups_amount || x_group < 0) {
			printf("Can't add such group to a database, group should be from 0 to %d\n", groups_amount - 1);
			return io_status::format;
		}
		res = groups[x_group]->add_elem_to_db(x, false, already_in);
		if (res != io_status::success) {
			return res;
		}
		if (!lss->get_head()) {
			lss->set_head(x);
			lss->set_tail(x);
		}
		else {
			if (!*already_in) {
				x->set_prev(lss->get_tail());
				lss->get_tail()->set_next(x);
				lss->set_tail(x);
			}
		}
		return io_status::success;
	}
	io_status read(FILE* fp, unsigned int max_read = -1) {
		unsigned int already_read = 0;
		if (feof(fp)) {
			printf("Void list\n");
			return io_status::eof;
		}
		list2_node* new_list2_node = new list2_node;
		if (new_list2_node == nullptr) {
			printf("Not enough memory for data!\n");
			return io_status::memory;
		}
		bool already_in = false;
		io_status res = new_list2_node->read(database<T>::k, fp);
		while (res == io_status::success && already_read <= max_read) {
			already_read++;
			already_in = false;
			if (add_elem(new_list2_node, false, &already_in) != io_status::success) {
				delete new_list2_node;
				return io_status::memory;
			}
			new_list2_node = new list2_node;
			if (new_list2_node == nullptr) {
				printf("Not enough memory for data!\n");
				return io_status::memory;
			}
			res = new_list2_node->read(database<T>::k, fp);
		}
		if (new_list2_node)
		{
			delete new_list2_node;
		}
		if (res == io_status::format) {
			printf("Reading error in data\n");
			return io_status::format;
		}
		if (!feof(fp)) {
			printf("Didn't reach the end of data\n");
			return io_status::eof;
		}
		lss->set_length(already_read);
		return io_status::success;
	}
	void delete_from_list(list2_node* node_to_delete)
	{
		lss->delete_node(node_to_delete);
	}
};
#endif