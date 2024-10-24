#include "list2.h"

void list2::print(unsigned int print_limit, FILE* fp) {
	unsigned int i = 0;
	list2_node* curr = nullptr;
	(void)fp;
	if (!head) return;
	for (curr = head; i < print_limit && curr; curr = curr->get_next(), i++) {
		curr->print();
		fprintf(fp, "\n");
	}
}
list2_node* list2::delete_node(list2_node* curr) {
	if (!curr) return nullptr;
	if (curr == head) {
		head = curr->next;
	}
	if (curr == tail) {
		tail = curr->prev;
	}
	list2_node* before = curr->prev;
	list2_node* after = curr->next;
	if (before) {
		before->next = after;
	}
	if (after) {
		after->prev = before;
	}
	delete curr;
	return after;
}

void list1::print(unsigned int print_limit, FILE* fp) {
	unsigned int i = 0;
	list1_node* curr = nullptr;
	(void)fp;
	if (!head) return;
	ordering print_order[max_items] = { ordering::none, ordering::none, ordering::none };
	for (curr = head; i < print_limit && curr; curr = curr->get_next(), i++) {
		curr->print(print_order, fp);
	}
}
void list1::add_node(list1_node* x, bool check_uniqueness, bool* already_in) {
	if (!x) return;
	if (!head || !tail) {
		head = x;
		tail = x;
		return;
	}
	if (check_uniqueness) {
		list1_node* curr = head;
		while (curr) {
			if (curr->get_elem()->get_group() == x->get_elem()->get_group() && curr->get_elem()->get_phone() == x->get_elem()->get_phone() && curr->get_elem()->safe_strcmp(curr->get_elem()->get_name(), x->get_elem()->get_name()) == 0) {
				if (already_in) *already_in = true;
				return;
			}
			curr = curr->next;
		}
	}
	tail->next = x;
	tail = x;
	x->next = nullptr;
	return;
}

void list1::adaptive_add_element(list2_node* x)
{
	if (head == nullptr)
	{
		head = new list1_node();
		tail = head;
		head->set_elem(x);
		return;
	}
	list1_node* curr = head;
	while (curr != nullptr)
	{
		if (curr->get_elem() == nullptr)
		{
			curr->set_elem(x);
			return;
		}
		if (curr->get_next())
		{
			curr = curr->get_next();
		}
		else
		{
			curr->set_next(new list1_node());
			tail = curr->get_next();
		}
	}
}

void list1::destroy_elements_in_list2_and_crear_list1(list2* list, bool destroy_list)
{
#if debug_helpers
	int amount = 0;
#endif
	list1_node* node_to_delete = get_head();
	while (node_to_delete && node_to_delete->get_elem())
	{
		list1_node* current_node = node_to_delete;
		node_to_delete = node_to_delete->get_next();
		list2_node* element = current_node->get_elem();
		if (element)
		{
			list->delete_node(element);
#if debug_helpers
			amount++;
#endif
		}
		current_node->set_elem(nullptr);
	}
	if (destroy_list)
	{
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
#if debug_helpers
	printf("Elements deleted: %d\n", amount);
#endif
}