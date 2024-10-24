#include "record.h"

using namespace std;
int record::hash_by_phone(int phone, unsigned int k) {
	unsigned int k10 = 1;
	for (unsigned int i = 1; i <= k; i++) {
		k10 *= 10;
	}
	int res = phone % k10;
#if debug_helpers
	printf("Last %d numbers of %d: %d\n", k, phone, res);
#endif
	return res;
}
io_status record::init(const char* n, int p, int g, int h_p)
{
	phone = p;
	group = g;
	hash_phone = h_p;
	if (n)
	{
		name = make_unique<char[]>(strlen(n) + 1);
		if (!name) return io_status::memory;
		strcpy(name.get(), n);
	}
	else
		name = nullptr;
	return io_status::success;
}
io_status record::read(unsigned int k, FILE* fp)
{
	char buf[LEN];
	name = nullptr;
	int res = fscanf(fp, "%s%d%d", buf, &phone, &group);
	if (res != 3)
	{
		if (res == -1) return io_status::empty;
		return io_status::format;
	}
	int hash_phone = hash_by_phone(phone, k);
	if (init(buf, phone, group, hash_phone) != io_status::success)
		return io_status::memory;
	return io_status::success;
}
bool record::equal(record& x) {
	if (group != x.group) return false;
	if (phone != x.phone) return false;
	if (safe_strcmp(name.get(), x.name.get()) != 0) return false;
	return true;
}
int record::cmp(record& x, ordering* sort_order) {
	int current_res = 0;
	for (int i = 0; i < max_items; i++) {
		switch (sort_order[i]) {
		case (ordering::name): {
			current_res = safe_strcmp(x.name.get(), name.get()); break;
		}
		case (ordering::phone): {
			current_res = x.phone - phone; break;
		}
		case (ordering::group): {
			current_res = x.group - group; break;
		}
		case (ordering::none): {
			return current_res;
		}
		}
		if (current_res != 0) return current_res;
	}
	return current_res;
}
bool record::compare_name(condition x, const record& y)
{
	switch (x)
	{
	case condition::none:
		return true;
	case condition::eq:
		return (safe_strcmp(name.get(), y.name.get()) == 0 ? true : false);
	case condition::ne:
		return (safe_strcmp(name.get(), y.name.get()) != 0 ? true : false);
	case condition::gt:
		return (safe_strcmp(name.get(), y.name.get()) < 0 ? true : false);
	case condition::lt:
		return (safe_strcmp(name.get(), y.name.get()) > 0 ? true : false);
	case condition::ge:
		return (safe_strcmp(name.get(), y.name.get()) <= 0 ? true : false);
	case condition::le:
		return (safe_strcmp(name.get(), y.name.get()) >= 0 ? true : false);
	case condition::nlike:
		return !(compare(y.name.get()));
	case condition::like:
		return compare(y.name.get());
	}
	return false;
}
bool record::compare_phone(condition x, const record& y) const
{
	switch (x)
	{
	case condition::none:
		return true;
	case condition::eq:
		return phone == y.phone;
	case condition::ne:
		return phone != y.phone;
	case condition::gt:
		return phone < y.phone;
	case condition::lt:
		return phone > y.phone;
	case condition::ge:
		return phone <= y.phone;
	case condition::le:
		return phone >= y.phone;
	default:
		return false;
	}
}
bool record::compare_group(condition x, const record& y) const
{
	switch (x)
	{
	case condition::none:
		return true;
	case condition::eq:
		return group == y.group;
	case condition::ne:
		return group != y.group;
	case condition::gt:
		return group < y.group;
	case condition::lt:
		return group > y.group;
	case condition::ge:
		return group <= y.group;
	case condition::le:
		return group >= y.group;
	default:
		return false;
	}
}
void record::print(ordering print_order[], FILE* fp)
{
	const ordering default_ordering[max_items] = { ordering::name, ordering::phone, ordering::group };
	const ordering* p = (print_order ? print_order : default_ordering);
	for (int i = 0; i < max_items; i++)
		switch (p[i])
		{
		case ordering::name:
			fprintf(fp, "%s ", name.get()); break;
		case ordering::phone:
			fprintf(fp, "%d ", phone); break;
		case ordering::group:
			fprintf(fp, "%d ", group); break;
		case ordering::none:
			continue;
		}
}
io_status record::make_map(const char* str)
{
	size_t str_len = strlen(str), i = 0, parsed = 0;
	map = std::make_unique<char[]>(str_len + 1);
	if (!map) return io_status::memory;
	low_map = std::make_unique<char[]>(str_len + 1);
	if (!low_map) return io_status::memory;
	high_map = std::make_unique<char[]>(str_len + 1);
	if (!high_map) return io_status::memory;
	bool slash = false, percent = false;
	for (i = 0; i < str_len; i++) {
		if (slash) {
			slash = false;
			name[parsed] = str[i];
			map[parsed] = '0';
			low_map[parsed] = '@';
			high_map[parsed] = '@';
			parsed++;
		}
		else {
			if (str[i] == '\\') {
				slash = true;
				percent = false;
			}
			else if (str[i] == '_') {
				percent = false;
				map[parsed] = '_';
				name[parsed] = str[i];
				low_map[parsed] = '@';
				high_map[parsed] = '@';
				parsed++;
			}
			else if (str[i] == '%') {
				if (!percent)
				{
					percent = true;
					name[parsed] = str[i];
					map[parsed] = '%';
					low_map[parsed] = '@';
					high_map[parsed] = '@';
					parsed++;
				}
			}
			else if (str[i] == '[') {
				percent = false;
				if (i + 4 <= str_len && str[i + 2] == '-' && str[i + 4] == ']') {
					map[parsed] = '[';
					low_map[parsed] = str[i + 1];
					high_map[parsed] = str[i + 3];
					i += 4;
				}
				else if (i + 5 <= str_len && str[i + 1] == '^' && str[i + 3] == '-' && str[i + 5] == ']') {
					map[parsed] = '^';
					low_map[parsed] = str[i + 2];
					high_map[parsed] = str[i + 4];
					i += 5;
				}
				else {
					printf("Template usage error: %s\n", str);
					return io_status::format;
				}
				name[parsed] = '@';
				parsed++;
			}
			else if (str[i] == ']' || str[i] == '^')
			{
				printf("Template usage error: %s\n", str);
				return io_status::format;
			}
			else
			{
				percent = false;
				name[parsed] = str[i];
				map[parsed] = '0';
				low_map[parsed] = '@';
				high_map[parsed] = '@';
				parsed++;
			}
		}
	}
	if (slash)
	{
		printf("Template usage error: %s\n", str);
		return io_status::format;
	}
	name[parsed] = '\0';
	map[parsed] = '\0';
	low_map[parsed] = '\0';
	high_map[parsed] = '\0';
	return io_status::success;
}
bool record::accept_symbol(char symb, int position) {
	char symb_code = map[position];
	if (symb_code == '0') {
		return (symb == name[position]);
	}
	else if (symb_code == '_') return true;
	else if (symb_code == '[') {
		return (symb >= low_map[position] && symb <= high_map[position]);
	}
	else if (symb_code == '^')
	{
		return (!(symb >= low_map[position] && symb <= high_map[position]));
	}
	return false;
}
bool record::compare_without_percent(const char* str) {
	size_t str_len = strlen(str), name_len = strlen(name.get()), i = 0;
	if (str_len != name_len) return false;
	for (i = 0; i < str_len; i++) {
		if (!accept_symbol(str[i], (int)i))
			return false;
	}
	return true;
}
bool record::compare(const char* str) {
	if (!str) return false;
	int percent_amount = 0, i = 0;
	size_t str_len = strlen(str);
	for (int i = 0; map[i]; i++) {
		if (map[i] == '%') {
			percent_amount++;
		}
	}
	if (percent_amount == 0) {
		return compare_without_percent(str);
	}
	for (i = 0; str[i] && name[i] && map[i] != '%'; i++) {  // before the first %
		if (!accept_symbol(str[i], i))
			return false;
	}
	int percents_done = 0, sea_beg = i, sea_i = i;   // a ship of curtain symbols is trying to find a place for an anchor to park in a sea of unknown symbols
	while (percents_done < percent_amount - 1) {  // for inner %
		percents_done++;
		int sea_len = 0;
		for (i = sea_beg + 1; name[i] && map[i] != '%'; i++) {
			sea_len++;
		}
		bool parking_possible = true, parking_found = false;
		for (int anchor = sea_i; str[anchor]; anchor++) {
			int ship_fit = 0;
			for (ship_fit = 0; str[anchor + ship_fit] && ship_fit < sea_len; ship_fit++) {
				if (!accept_symbol(str[anchor + ship_fit], sea_beg + ship_fit + 1)) {
					parking_possible = false;
					break;
				}
			}
			if (parking_possible) {
				sea_i = anchor + ship_fit;
				sea_beg = i;
				parking_found = true;
				break;
			}
			else
				parking_possible = true;
		}
		if (!parking_found)
			return false;
	}
	int tail_len = 0;
	for (i = sea_beg + 1; name[i]; i++) {
		tail_len++;
	}
	if (tail_len == 0) {
		return true;
	}
	if ((int)str_len - sea_i < tail_len)
		return false;
	for (i = 0; i < tail_len; i++) {
		if (!accept_symbol(str[str_len - tail_len + i], sea_beg + i + 1))
			return false;
	}
	return true;
}
int record::safe_strcmp(const char* x, const char* y)
{
	if (y == nullptr)
	{
		if (x != nullptr)
			return 1;
		return 0;
	}
	if (x == nullptr)
		return -1;
	return strcmp(x, y);
}
bool record::scan_int(const char* buf, int* x) {
	if (!buf) return false;
	int offset = 0;
	if (sscanf(buf, "%d%n", x, &offset) != 1) {
		return false;
	}
	size_t data_len = strlen(buf);
	int allowed_offset = 0;
	if (data_len >= 1) {
		allowed_offset += (buf[data_len - 1] == '\n');
		if (data_len >= 2) {
			allowed_offset += (buf[data_len - 2] == '\r');
		}
	}
	if ((int)data_len - allowed_offset != offset) {
		return false;
	}
	return true;
}