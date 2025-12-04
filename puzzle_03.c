#include "common.c"

struct range
{
	int start;
	int end;
};

struct range calc_view(int left, int needed, int len)
{
	return (struct range){left, len - needed + 1};
}

struct max_digit {
	int value;
	int index;
};

struct max_digit max_in_view(const char* battery, const struct range* view)
{
	char m = 0;
	int idx = 0;
	for (int i = view->start; i < view->end; i++) {
		if (battery[i] > m) {
			m = battery[i];
			idx = i;
		}
	}
	return (struct max_digit) {
		m - '0',
		idx
	};
}

int64_t max_joltage(const char* battery, size_t len, int num_digits)
{
	int* digits = calloc(num_digits, sizeof(int));
	int lastidx = 0;
	
	for (int digit_index = 0; digit_index < num_digits; digit_index++)
	{
		struct range view = calc_view(lastidx, num_digits - digit_index, len);
		struct max_digit max_digit = max_in_view(battery, &view);
		lastidx = max_digit.index + 1;
		digits[digit_index] = max_digit.value;
	}
	
	int64_t joltage = 0;
	int64_t mul = 1;
	for (int i = 0; i < num_digits - 1; i++) mul *= 10;

	for (int i = 0; i < num_digits; i++)
	{
		joltage += (int64_t)digits[i] * mul;
		mul /= 10;
	}

	free(digits);

	return joltage;
}

int max_joltage1(const char* battery, size_t len)
{
	return max_joltage(battery, len, 2);
}

int64_t max_joltage2(const char* battery, size_t len)
{
	return max_joltage(battery, len, 12);
}

void part1(char* data, size_t data_length, char** lines, size_t line_count)
{
	const char* test_input[] = {
		"987654321111111",
		"811111111111119",
		"234234234234278",
		"818181911112111",
	};
	const size_t test_input_count = sizeof(test_input) / sizeof(test_input[0]);

	// const char** input = (const char**)(&test_input[0]);
	// const size_t input_count = test_input_count;
	char** input = lines;
	const size_t input_count = line_count;

	const int reflen = strlen(input[0]);
	int joltage = 0;
	for (size_t i = 0; i < input_count; i++)
	{
		size_t len = strlen(input[i]);
		if (len < reflen)
			break;
		joltage += max_joltage1(input[i], strlen(input[i]));
	}
	printf("joltage: %d\n", joltage);
}

void part2(char* data, size_t data_length, char** lines, size_t line_count)
{
	const char* test_input[] = {
		"987654321111111",
		"811111111111119",
		"234234234234278",
		"818181911112111",
	};
	const size_t test_input_count = sizeof(test_input) / sizeof(test_input[0]);

#if 0
	const char** input = (const char**)(&test_input[0]);
	const size_t input_count = test_input_count;
#else
	char** input = lines;
	const size_t input_count = line_count;
#endif

	const int reflen = strlen(input[0]);
	int64_t joltage = 0;
	for (size_t i = 0; i < input_count; i++)
	{
		size_t len = strlen(input[i]);
		if (len < reflen)
			break;
		joltage += max_joltage2(input[i], strlen(input[i]));
	}
	printf("joltage: %lli\n", joltage);
}

int main()
{
	typedef void (*part_func)(char*, size_t, char**, size_t);
	part_func parts[2] = {part1, part2};

	for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); i++)
	{
		size_t data_length;
		char data_file_name[64];
		printf("\n---- Part %d ----\n", i + 1);
		snprintf(data_file_name, 64, "puzzle_03p%d.txt", i + 1);
		char* data = read_entire_file(data_file_name, &data_length);
		char** lines = NULL;

		if (data)
			lines = split_lines(data, data_length);

		parts[i](data, data_length, lines, arrlenu(lines));

		freetok(lines);
		free(data);
	}

	return 0;
}