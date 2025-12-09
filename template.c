#include "common.c"

#define TEST 1

#if TEST
#define SELECT(test, prod) (test)
#else
#define SELECT(test, prod) (prod)
#endif

const char* k_test_data = "";

struct context
{
	char* data;
	size_t data_length;
	char** lines;
	size_t line_count;
};

void part1(struct context* ctx)
{
	//
}

void part2(struct context* ctx)
{
	//
}

int main()
{
	size_t test_data_length = strlen(k_test_data);
	char* test_data = calloc(test_data_length + 1, sizeof(char));
	strcpy(test_data, k_test_data);

	typedef void (*part_func)(struct context*);
	part_func parts[2] = {part1, part2};

	size_t data_length;
	char* data = read_entire_file("<puzzle_id>.txt", &data_length);
	char** lines = NULL;

	char* input = SELECT(test_data, data);
	size_t input_length = SELECT(test_data_length, data_length);

	if (input)
	{
		lines = split_lines(input, input_length);

		struct context ctx = {
			.data = data,
			.data_length = data_length,
			.lines = lines,
			.line_count = arrlenu(lines),
		};

		for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); i++)
		{
			printf("\n---- Part %d ----\n", i + 1);
			parts[i](&ctx);
		}

		freetok(lines);
	}

	free(data);
	free(test_data);

	return 0;
}