#include "common.c"

#define TEST 0

#if TEST
#define SELECT(test, prod) (test)
#else
#define SELECT(test, prod) (prod)
#endif

const char* k_test_data = "123 328  51 64 \n"
						  " 45 64  387 23 \n"
						  "  6 98  215 314\n"
						  "*   +   *   +  \n";

void part1(char* data, size_t data_length, char** lines, size_t line_count)
{
	int64_t** inputs = NULL;
	arrsetcap(inputs, 10);

	int problem_count = 0;
	int64_t result = 0;

	for (size_t line_index = 0; line_index < line_count; line_index++)
	{
		printf("%s\n", lines[line_index]);
		char* check = lines[line_index];
		while (*check == ' ')
			check++;

		if (isdigit(*check))
		{
			int64_t* input_line = NULL;
			arrsetcap(input_line, 500);

			char* c = lines[line_index];
			char* end;
			do
			{
				int64_t parsed = strtoll(c, &end, 10);
				arrput(input_line, parsed);
				c = end;
				while (*c == ' ')
					c++;
			} while (*c);

			if (line_index > 0)
			{
				if (arrlen(input_line) != arrlen(inputs[0]))
				{
					printf("ERROR!!!!\n");
					exit(1);
				}
			}

			// parse the line here

			arrput(inputs, input_line);
		}
		else
		{
			char* c = lines[line_index];
			int problem_index = 0;
			while (*c)
			{
				if (*c == '+')
				{
					int64_t pr = 0;
					for (int ii = 0, ic = arrlen(inputs); ii < ic; ii++)
					{
						pr += inputs[ii][problem_index];
					}
					problem_index++;
					result += pr;
				}
				else if (*c == '*')
				{
					int64_t pr = inputs[0][problem_index];
					for (int ii = 1, ic = arrlen(inputs); ii < ic; ii++)
					{
						pr *= inputs[ii][problem_index];
					}
					problem_index++;
					result += pr;
				}

				c++;
			}
			break;
		}
	}

	printf("Result: %llu\n", result);
}

void part2(char* data, size_t data_length, char** lines, size_t line_count)
{
	//

	int64_t** inputs = NULL;
	arrsetcap(inputs, 100);

	int problem_count = 0;
	size_t op_line = 0;

	for (size_t line_index = 0; line_index < line_count; line_index++)
	{
		printf("%s\n", lines[line_index]);
		char* check = lines[line_index];
		while (*check == ' ')
			check++;

		if (*check == '*' || *check == '+')
		{
			op_line = line_index;
			break;
		}
	}

	int64_t* values = NULL;
	arrsetcap(values, 5);

	size_t last_column = strlen(lines[0]);
	int64_t place = 1;
	for (int64_t col = last_column - 1; col >= 0; col--)
	{
		int64_t number = 0;
		for (int line_index = op_line - 1; line_index >= 0; line_index--)
		{
			char c = lines[line_index][col];
			int64_t digit = 0;

			if (c != ' ')
			{
				digit = c - '0';
				number += digit * place;
				place *= 10;
			}
		}
		place = 1;
		if (number != 0)
		{
			arrput(values, number);
		}

		if (number == 0 || col == 0)
		{
			arrput(inputs, values);
			values = NULL;
			arrsetcap(values, 5);
		}
		// printf("%llu\n", number);
	}

	char* c = lines[op_line] + strlen(lines[op_line]);
	int problem_index = 0;
	int64_t result = 0;
	char* operators = NULL;
	int64_t* problem_results = NULL;
	arrsetcap(operators, arrlen(inputs[0]));
	arrsetcap(problem_results, arrlen(inputs[0]));

	while (c >= lines[op_line])
	{
		if (*c == '+')
			arrput(operators, '+');
		if (*c == '*')
			arrput(operators, '*');
		c--;
	}

	for (int pi = arrlen(inputs) - 1; pi >= 0; pi--)
	{
		if (operators[pi] == '+')
		{
			int64_t pr = 0;
			for (int ii = 0, ic = arrlen(inputs[pi]); ii < ic; ii++)
			{
				pr += inputs[pi][ii];
			}
			arrput(problem_results, pr);
			result += pr;
		}
		else if (operators[pi] == '*')
		{
			int64_t pr = 1;
			for (int ii = 0, ic = arrlen(inputs[pi]); ii < ic; ii++)
			{
				pr *= inputs[pi][ii];
			}
			arrput(problem_results, pr);
			result += pr;
		}
	}

	for (int i = arrlen(inputs) - 1; i >= 0; i--)
	{
		for (int j = 0, jc = arrlen(inputs[i]); j < jc; j++)
		{
			if (j < jc - 1)
			{
				printf("%llu %c ", inputs[i][j], operators[i]);
			}
			else
			{
				printf("%llu = %llu", inputs[i][j], problem_results[i]);
			}
		}
		printf("\n");
	}

	printf("Result: %llu\n", result);
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
		snprintf(data_file_name, 64, "puzzle_06p%d.txt", i + 1);

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