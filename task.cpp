#include "command_list.h"
#if debug_helpers
#include "vld/include/vld.h"
#endif

int apply_command(huge_database<list1>* dbb, FILE* input = stdin, FILE* output = stdout) {
	char buf[LEN], air[LEN];
	int amount = 0;
	command* comm = new command();
	if (!comm) {
		printf("Not enough memory!\n");
		return ERROR_MEMORY;
	}
	command* extra_cmd = new command(command_type::del, condition::eq, condition::eq, condition::eq, operation::land);
	if (!extra_cmd) {
		printf("Nor enough memory for extra command!\n");
		delete comm;
		return ERROR_MEMORY;
	}
	comm->get_spaces(" \t\r\n");
	int test = 1;
	while (fgets(buf, LEN, input)) {
		size_t i = 0, str_len = strlen(buf);
		while (i < str_len && comm->get_type() != command_type::quit) {
			if (buf[i] == '\n' || buf[i] == '\r') break;
#if debug_helpers
			printf("Test %d\n", test);
#endif
			if (!comm->parse(buf, str_len, i, air)) {
				printf("Parse error in line: %s\n", buf);
			}
			else {
#if debug_helpers
				comm->print();
#endif
				command_list::apply(comm, extra_cmd, dbb, output, amount);
#if debug_helpers
				printf("Curr database:\n");
				dbb->print();
#endif
			}
			if (comm->get_type() == command_type::select) fputc('\n', output);
		}
		test++;
#if debug_helpers
		if (comm->get_type() == command_type::quit)
		{
			break;
		}
#endif
	}
	delete comm;
	delete extra_cmd;
	return amount;
}
int safe_strcmp(const char* x, const char* y)
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

int read_config()
{
	FILE* fcf;
	fcf = fopen("config.txt", "r");
	if (!fcf) {
		printf("Can't open config.txt\n");
		return ERROR_OPEN;
	}
	char buf[LEN];
	int k = 0;
	while (fgets(buf, LEN, fcf)) {
		if (buf[0] && buf[0] == '#') {
			continue;
		}
		if (record::scan_int(buf, &k)) {
			fclose(fcf);
			return k;
		}
	}
	fclose(fcf);
	return ERROR_CONFIG;
}

int main(int argc, char* argv[]) {
	double time = 0;
	char* file_STU = 0;
	if (!(argc == 2)) {
		printf("Usage %s: file\n", argv[0]);
		return ERROR_INPUT;
	}
	int k = read_config();
	if (k <= 0) {
		printf("Unexpected config.txt content\n");
		return ERROR_CONFIG;
	}
	file_STU = argv[1];
	FILE* STU;
	STU = fopen(file_STU, "r+");
	if (!STU) {
		printf("Cannot open %s\n", file_STU);
		return ERROR_OPEN;
	}
	huge_database<list1>* dbb = new huge_database<list1>(k);
	if (!dbb) {
		printf("Not enough memory!\n");
		fclose(STU);
		return ERROR_MEMORY;
	}
	io_status reading_res = dbb->read(STU);
	if (reading_res != io_status::success) {
		delete dbb;
		fclose(STU);
		return ERROR_READ;
	}
#if debug_helpers
	dbb->print();
#endif
	fclose(STU);
	int amount = 0;
	time = clock();
	amount = apply_command(dbb);
	time = (clock() - time) / CLOCKS_PER_SEC;
	printf("%s : Result = %d Elapsed = %.2f\n", argv[0], amount, time);
	delete dbb;
	return SUCCESS;
}
