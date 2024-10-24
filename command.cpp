#include "command.h"
#include "database.h"

void command::reset() {
	type = command_type::none;
	name.reset();
	map.reset();
	low_map.reset();
	high_map.reset();
	phone = 0;
	group = 0;
	c_name = condition::none;
	c_phone = condition::none;
	c_group = condition::none;
	op = operation::none;
	for (int i = 0; i < max_items; i++) {
		print_order[i] = ordering::none;
	}
	for (int i = 0; i < max_items; i++) {
		sort_order[i] = ordering::none;
	}
}
void command::reset_fields() {
	name.reset();
	map.reset();
	low_map.reset();
	high_map.reset();
	phone = 0;
	group = 0;
}
bool command::SEARCH_parse_print_arguments(char* buf, int& print_arguments_read, size_t j) {
	if (safe_strcmp(buf, "*") == 0) {
		print_order[0] = ordering::name;
		print_order[1] = ordering::phone;
		print_order[2] = ordering::group;
		print_arguments_read = max_items;
		return true;
	}
	if (print_arguments_read < 2) {
		if (j >= 1 && buf[j - 1] == ',') {
			buf[j - 1] = '\0';
		}
	}
	print_arguments_read++;
	if (safe_strcmp(buf, "name") == 0) {
		print_order[print_arguments_read - 1] = ordering::name;
	}
	else if (safe_strcmp(buf, "phone") == 0) {
		print_order[print_arguments_read - 1] = ordering::phone;
	}
	else if (safe_strcmp(buf, "group") == 0) {
		print_order[print_arguments_read - 1] = ordering::group;
	}
	else return false;
	return true;
}
bool command::SEARCH_parse_operation(char* buf) {
	if (safe_strcmp(buf, "or") == 0) {
		if (op == operation::land) return false;
		else {
			op = operation::lor;
		}
	}
	else if (safe_strcmp(buf, "and") == 0) {
		if (op == operation::lor) return false;
		else {
			op = operation::land;
		}
	}
	else {
		return false;
	}
	return true;
}
bool command::SEARCH_parse_field(char* buf, ordering& field) {
	if (safe_strcmp(buf, "name") == 0) {
		field = ordering::name;
	}
	else if (safe_strcmp(buf, "phone") == 0) {
		field = ordering::phone;
	}
	else if (safe_strcmp(buf, "group") == 0) {
		field = ordering::group;
	}
	else return false;
	return true;
}
bool command::SEARCH_parse_condition(char* buf, int& command_arguments_read, ordering& field, bool& nott) {
	condition* c_code = nullptr;
	switch (field) {
	case (ordering::none): return false;
	case(ordering::name): c_code = &c_name; break;
	case(ordering::phone): c_code = &c_phone; break;
	case(ordering::group): c_code = &c_group; break;
	}
	if (*c_code != condition::none) return false;
	if (nott) {
		if (safe_strcmp(buf, "like") == 0) {
			*c_code = condition::nlike;
			nott = false;
		}
		else return false;
	}
	else if (safe_strcmp(buf, "=") == 0) *c_code = condition::eq;
	else if (safe_strcmp(buf, "<>") == 0) *c_code = condition::ne;
	else if (safe_strcmp(buf, "<") == 0) *c_code = condition::lt;
	else if (safe_strcmp(buf, ">") == 0) *c_code = condition::gt;
	else if (safe_strcmp(buf, "<=") == 0) *c_code = condition::le;
	else if (safe_strcmp(buf, ">=") == 0) *c_code = condition::ge;
	else if (safe_strcmp(buf, "not") == 0) nott = true;
	else if (safe_strcmp(buf, "like") == 0) *c_code = condition::like;
	else return false;
	if (field != ordering::name && (*c_code == condition::like || *c_code == condition::nlike)) return false;
	if (!nott) {
		command_arguments_read = 2;
	}
	return true;
}
bool command::SEARCH_parse_data(char* buf, ordering& field) {
	if (field == ordering::name) {
		if (buf) {
			name = std::make_unique<char[]>(strlen(buf) + 1);
			if (!name) {
				printf("Not enough memory for command data!\n");
			}
			strcpy(name.get(), buf);
		}
		else
			name = nullptr;
	}
	else if (field == ordering::phone) {
		if (buf) {
			if (!scan_int(buf, &phone)) return false;
		}
		else phone = 0;
	}
	else if (field == ordering::group) {
		if (buf) {
			if (!scan_int(buf, &group)) return false;
		}
		else group = 0;
	}
	else {
		return false;
	}
	return true;
}
bool command::SEARCH_parse_where_arguments(char* buf, int& command_arguments_read, int& commands_read, ordering& field, bool& nott) {
	if (commands_read > 0 && command_arguments_read == -1) {
		command_arguments_read = 0;
		return SEARCH_parse_operation(buf);
	}
	if (command_arguments_read == 0) {
		command_arguments_read = 1;
		return SEARCH_parse_field(buf, field);
	}
	if (command_arguments_read == 1) {
		return SEARCH_parse_condition(buf, command_arguments_read, field, nott);
	}
	if (command_arguments_read == 2) {
		commands_read++;
		command_arguments_read = -1;
		return SEARCH_parse_data(buf, field);
	}
	return false;
}
bool command::SEARCH_parse_sort_arguments(char* buf, int& sort_arguments_read, size_t j) {
	if (sort_arguments_read < 2) {
		if (j >= 1 && buf[j - 1] == ',') {
			buf[j - 1] = '\0';
		}
	}
	sort_arguments_read++;
	ordering field = ordering::none;
	if (safe_strcmp(buf, "name") == 0) {
		field = ordering::name;
	}
	else if (safe_strcmp(buf, "phone") == 0) {
		field = ordering::phone;
	}
	else if (safe_strcmp(buf, "group") == 0) {
		field = ordering::group;
	}
	else return false;
	for (int i = 0; i < sort_arguments_read && i < max_items; i++) {
		if (sort_order[i] == field) return false;
	}
	sort_order[sort_arguments_read - 1] = field;
	return true;
}
bool command::SEARCH_parse(const char* string, size_t str_len, size_t& i, char* buf) {
	int print_arguments_read = 0, sort_arguments_read = 0, where_arguments_read = -1, where_conditions_read = 0;
	ordering field = ordering::none;
	size_t j = 0;
	bool print_expected = true, where_began = false, where_finished = false, sort_began = false, nott = false;
	while (i <= str_len) {
		j = chop(string, str_len, i, buf);
		if (j == 0) break;
		if (print_expected) {
			if (safe_strcmp(buf, "where") == 0) {
				print_expected = false;
				where_began = true;
				where_arguments_read = 0;
				continue;
			}
			else if (safe_strcmp(buf, "order") == 0) {
				print_expected = false;
				if (i > str_len) return false;
				chop(string, str_len, i, buf);
				if (safe_strcmp(buf, "by") == 0) {
					if (i > str_len) return false;
					else sort_began = true;
				}
				else return false;
			}
			else if (print_arguments_read >= max_items || !SEARCH_parse_print_arguments(buf, print_arguments_read, j)) {
				return false;
			}
			if (print_arguments_read == 3) print_expected = false;
		}
		else if (!where_began && !sort_began) {
			if (safe_strcmp(buf, "where") == 0) {
				print_expected = false;
				where_began = true;
				where_arguments_read = 0;
				continue;
			}
			else if (safe_strcmp(buf, "order") == 0) {
				where_finished = true;
				if (i > str_len) return false;
				chop(string, str_len, i, buf);
				if (safe_strcmp(buf, "by") == 0) {
					if (i > str_len) return false;
					else sort_began = true;
				}
				else return false;
			}
		}
		else if (where_began && !where_finished) {
			if (!SEARCH_parse_where_arguments(buf, where_arguments_read, where_conditions_read, field, nott)) {
				if (safe_strcmp(buf, "order") == 0) {
					where_finished = true;
					if (i > str_len) return false;
					chop(string, str_len, i, buf);
					if (safe_strcmp(buf, "by") == 0) {
						if (i > str_len) return false;
						else sort_began = true;
					}
					else return false;
				}
				else return false;
			}
			if (where_conditions_read == 3) where_finished = true;
		}
		else if (safe_strcmp(buf, "order") == 0) {
			where_finished = true;
			if (i > str_len) return false;
			chop(string, str_len, i, buf);
			if (safe_strcmp(buf, "by") == 0) {
				if (i > str_len) return false;
				else sort_began = true;
			}
			else return false;
		}
		else if (sort_began) {
			if (sort_arguments_read >= max_items || !SEARCH_parse_sort_arguments(buf, sort_arguments_read, j)) {
				return false;
			}
			if (sort_arguments_read == 3) break;
		}
		else return false;
	}
	if (where_began) {
		if (where_conditions_read == 0) return false;
		if (sort_began && !where_finished) return false;
	}
	if (sort_began) {
		if (sort_arguments_read == 0) return false;
	}
	return true;
}
bool command::INSERT_parse(const char* string, size_t str_len, size_t& i, char* buf) {
	int fields_read = 0;
	char* read_from = nullptr;
	ordering field = ordering::none;
	size_t j = 0;
	while (fields_read < max_items && i <= str_len) {
		j = chop(string, str_len, i, buf);
		if (fields_read < 2) {
			if (j >= 1 && buf[j - 1] == ',') {
				buf[j - 1] = '\0';
			}
			else return false;
		}
		else {
			if (j >= 1 && buf[j - 1] == ')') {
				buf[j - 1] = '\0';
			}
			else return false;
		}
		if (fields_read == 0) {
			field = ordering::name;
			if (buf[0] == '(') {
				read_from = buf + 1;
			}
			else return false;
		}
		else if (fields_read == 1) {
			field = ordering::phone;
			read_from = buf;
		}
		else if (fields_read == 2) {
			field = ordering::group;
			read_from = buf;
		}
		else return false;
		if (!SEARCH_parse_data(read_from, field)) return false;
		fields_read++;
	}
	if (fields_read != 3) return false;
	return true;
}
bool command::DELETE_parse(const char* string, size_t str_len, size_t& i, char* buf) {
	int command_arguments_read = -1, commands_read = 0;
	ordering field = ordering::none;
	bool wheree = false, nott = false;
	while (i <= str_len) {
		if (string[i] == ';') {
			if (command_arguments_read == -1) return true;
			else return false;
		}
		chop(string, str_len, i, buf);
		if (!wheree) {
			if (safe_strcmp(buf, "where") == 0) {
				wheree = true;
				command_arguments_read = 0;
				continue;
			}
			else return false;
		}
		else {
			if (commands_read == max_commands) return false;
			if (!SEARCH_parse_where_arguments(buf, command_arguments_read, commands_read, field, nott)) return false;
		}
	}
	if (command_arguments_read != -1) return false;
	return true;
}
bool command::parse(const char* string, size_t str_len, size_t& i, char* buf) {
	this->reset();
	bool command_possibly_read = true;
	size_t j = chop(string, str_len, i, buf);
	if (safe_strcmp(buf, "quit") == 0) {
		type = command_type::quit;
	}
	else if (safe_strcmp(buf, "insert") == 0) {
		type = command_type::insert;
		if (!INSERT_parse(string, str_len, i, buf)) {
			command_possibly_read = false;
		}
	}
	else if (safe_strcmp(buf, "delete") == 0) {
		type = command_type::del;
		if (!DELETE_parse(string, str_len, i, buf)) {
			command_possibly_read = false;
		}
	}
	else if (safe_strcmp(buf, "select") == 0) {
		type = command_type::select;
		if (!SEARCH_parse(string, str_len, i, buf)) {
			command_possibly_read = false;
		}
	}
	else if (j != 0) {
		command_possibly_read = false;
	}
	if (!command_possibly_read || !(i <= str_len && string[i] == ';')) {
		while (i <= str_len && string[i] != ';') i++;
		i++;
		if (j == 0) return true;
		type = command_type::none;
		return false;
	}
	else i++;
	return true;
}

io_status command::make_net_no_tree(list1* net, list2* lss) {
	if (!lss) return io_status::success;
	list2_node* curr = lss->get_head();
	while (curr) {
		if (check_search_conditions(*curr)) {
			list1_node* curr_in_net = new list1_node();
			if (!curr_in_net) {
				printf("Not enough memory for net!\n");
				delete net;
				return io_status::memory;
			}
			curr_in_net->set_elem(curr);
			net->add_node(curr_in_net);
		}
		curr = curr->get_next();
	}
	return io_status::success;
}
void command::print(FILE* fp) const {
	(void)fp;
	if (type == command_type::none) return;
	printf("Command type: ");
	switch (type) {
	case (command_type::none): break;
	case (command_type::quit): printf("Quit\n"); { printf("\n\n"); return; }
	case (command_type::insert): printf("Insert\n"); break;
	case (command_type::select): printf("Select\n"); break;
	case (command_type::del): printf("Delete\n"); break;
	}
	if (type == command_type::insert) {
		printf("%s %d %d\n", name.get(), phone, group);
		printf("\n\n");
		return;
	}
	printf("Operation: ");
	switch (op) {
	case (operation::none): printf("\n"); break;
	case (operation::land): printf("AND\n"); break;
	case (operation::lor): printf("OR\n"); break;
	}
	printf("Conditions:\n");
	if (c_name != condition::none) {
		printf("name: ");
		switch (c_name)
		{
		case condition::none:
			break;
		case condition::eq:
			printf("eq"); break;
		case condition::ne:
			printf("ne"); break;
		case condition::lt:
			printf("lt"); break;
		case condition::gt:
			printf("gt"); break;
		case condition::le:
			printf("le"); break;
		case condition::ge:
			printf("ge"); break;
		case condition::nlike:
			printf("nlike"); break;
		case condition::like:
			printf("like"); break;
		}
		printf(" %s\n", name.get());
	}
	if (c_phone != condition::none) {
		printf("phone: ");
		switch (c_phone)
		{
		case condition::none:
			break;
		case condition::eq:
			printf("eq"); break;
		case condition::ne:
			printf("ne"); break;
		case condition::lt:
			printf("lt"); break;
		case condition::gt:
			printf("gt"); break;
		case condition::le:
			printf("le"); break;
		case condition::ge:
			printf("ge"); break;
		case condition::nlike:
			printf("nlike"); break;
		case condition::like:
			printf("like"); break;
		}
		printf(" %d\n", phone);
	}
	if (c_group != condition::none) {
		printf("group: ");
		switch (c_group)
		{
		case condition::none:
			break;
		case condition::eq:
			printf("eq"); break;
		case condition::ne:
			printf("ne"); break;
		case condition::lt:
			printf("lt"); break;
		case condition::gt:
			printf("gt"); break;
		case condition::le:
			printf("le"); break;
		case condition::ge:
			printf("ge"); break;
		case condition::nlike:
			printf("nlike"); break;
		case condition::like:
			printf("like"); break;
		}
		printf(" %d\n", group);
	}
	if (type == command_type::select) {
		printf("Print: ");
		for (int i = 0; i < max_items; i++)
			switch (print_order[i])
			{
			case ordering::name:
				printf("name "); break;
			case ordering::phone:
				printf("phone "); break;
			case ordering::group:
				printf("group "); break;
			case ordering::none:
				continue;
			}
		printf("\nSort by: ");
		for (int i = 0; i < max_items; i++)
			switch (sort_order[i])
			{
			case ordering::name:
				printf("name "); break;
			case ordering::phone:
				printf("phone "); break;
			case ordering::group:
				printf("group "); break;
			case ordering::none:
				continue;
			}
	}
	printf("\n\n");
}
bool command::check_search_conditions_no_field(ordering field, const record& x) {
	if (op == operation::lor) return true;
	if (field == ordering::name) {
		if (c_phone != condition::none) {
			if (!compare_phone(c_phone, x)) {
				return false;
			}
		}
		if (c_group != condition::none) {
			if (!compare_group(c_group, x)) {
				return false;
			}
		}
	}
	if (field == ordering::phone) {
		if (c_name != condition::none) {
			if (x.get_name() == nullptr) return true;
			if (!compare_name(c_name, x)) {
				return false;
			}
		}
		if (c_group != condition::none) {
			if (!compare_group(c_group, x)) {
				return false;
			}
		}
	}
	return true;
}
bool command::check_search_conditions(const record& x) {
	int fields_satisfy = 0;
	if (c_name != condition::none) {
		if (x.get_name() == nullptr) return true;
		if (compare_name(c_name, x)) {
			fields_satisfy++;
		}
		else if (op == operation::land) return false;
	}
	if (c_phone != condition::none) {
		if (compare_phone(c_phone, x)) {
			fields_satisfy++;
		}
		else if (op == operation::land) return false;
	}
	if (c_group != condition::none) {
		if (compare_group(c_group, x)) {
			fields_satisfy++;
		}
		else if (op == operation::land) return false;
	}
	if (c_name == condition::none && c_phone == condition::none && c_group == condition::none) return true;
	if ((op == operation::lor || op == operation::none) && fields_satisfy > 0) return true;
	if (op == operation::land) return true;
	return false;
}
size_t command::chop(const char* string, size_t str_len, size_t& i, char* buf) {
	size_t j = 0;
	while (i <= str_len && !(string[i] != spaces[static_cast<int>(string[i])])) {
		i++;
	}
	while (i <= str_len && string[i] != spaces[static_cast<int>(string[i])] && string[i] != ';') {
		buf[j] = string[i];
		i++;
		j++;
	}
	buf[j] = '\0';
	while (i <= str_len && !(string[i] != spaces[static_cast<int>(string[i])])) {
		i++;
	}
	return j;
}
list1_node* command::merge_sort(list1_node* curr) {
	if (!curr || !(curr->get_next())) return curr;
	list1_node* a = curr, * b = curr->get_next();
	while (b && b->get_next()) {
		curr = curr->get_next();
		b = b->get_next()->get_next();
	}
	b = curr->get_next();
	curr->set_next(nullptr);
	return merge(merge_sort(a), merge_sort(b));
}
list1_node* command::merge(list1_node* a, list1_node* b) {
	list1_node dummy;
	list1_node* fake_head = &dummy;
	list1_node* c = fake_head;
	while (a && b) {
		if (a->get_elem()->cmp(*(b->get_elem()), sort_order) > 0) {
			c->set_next(a);
			c = a;
			a = a->get_next();
		}
		else {
			c->set_next(b);
			c = b;
			b = b->get_next();
		}
	}
	if (!a) {
		c->set_next(b);
	}
	else c->set_next(a);
	return fake_head->get_next();
}