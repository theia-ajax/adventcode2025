#include "common.c"

#define TEST 1

#if TEST
#define SELECT(test, prod) (test)
#else
#define SELECT(test, prod) (prod)
#endif

const char* k_test_data = "";

void part1(char* data, size_t data_length, char** lines, size_t line_count)
{
	//
}

void part2(char* data, size_t data_length, char** lines, size_t line_count)
{
	//
}

int main()
{
	size_t test_data_length = strlen(k_test_data);
	char* test_data = calloc(test_data_length + 1, sizeof(char));
	strcpy(test_data, k_test_data);
	
	typedef void (*part_func)(char*, size_t, char**, size_t);
	part_func parts[2] = {part1, part2};

	for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); i++)
	{
		size_t data_length;
		char data_file_name[64];
		printf("\n---- Part %d ----\n", i + 1);
		snprintf(data_file_name, 64, "<puzzle_id>p%d.txt", i + 1);

		char* data = read_entire_file(data_file_name, &data_length);
		char** lines = NULL;

		char* input = SELECT(test_data, data);
		size_t input_length = SELECT(test_data_length, data_length);

		if (input)
		{
			lines = split_lines(input, input_length);
			parts[i](input, input_length, lines, arrlenu(lines));
			freetok(lines);
		}
		free(data);
	}

	free(test_data);

	return 0;
}