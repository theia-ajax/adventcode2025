#include "common.c"

#define TEST 0

#if TEST
#define SELECT(test, prod) (test)
#else
#define SELECT(test, prod) (prod)
#endif

const char* k_test_data = "..@@.@@@@.\n"
						  "@@@.@.@.@@\n"
						  "@@@@@.@.@@\n"
						  "@.@@@@..@.\n"
						  "@@.@@@@.@@\n"
						  ".@@@@@@@.@\n"
						  ".@.@.@.@@@\n"
						  "@.@@@.@@@@\n"
						  ".@@@@@@@@.\n"
						  "@.@.@@@.@.";

char* build_grid(char** lines, size_t line_count)
{
	int width = strlen(lines[0]);
	int height = line_count;

	char* grid = malloc(width * height + 1);
	memset(grid, '_', width * height);

	for (int i = 0; i < line_count; i++) {
		strncpy(grid + i * width, lines[i], width);
	}

	grid[width * height] = '\0';
	return grid;
}

void print_grid(FILE* f, char* grid, int width, int height)
{
	for (int i = 0; i < height; i++) {
		char buffer[1024] = {0};
		snprintf(buffer, width + 1, "%s", grid + (i * width));
		fprintf(f, "%s\n", buffer);
	}
}

int count_open(char* data, int width, int height, int x, int y)
{
	int count = 0;
	for (int ri = -1; ri <= 1; ri++)
	{
		int yi = y + ri;
		if (yi < 0 || yi >= height) continue;

		for (int ci = -1; ci <= 1; ci++)
		{
			if (ri == 0 && ci == 0)
				continue;

			int xi = x + ci;

			if (xi < 0 || xi >= width)
				continue;

			if (data[yi * width + xi] == '@')
			{
				count++;
			}
		}
	}
	return count;
}

int remove_rolls(char* src_grid, char* dst_grid, int width, int height)
{
	int removed = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (src_grid[x + y * width] == '@')
			{
				int open = count_open(src_grid, width, height, x, y);
				if (open < 4)
				{
					dst_grid[x + y * width] = '.';
					removed++;
				}
			}
		}
	}

	return removed;
}

void part1(char* data, size_t data_length, char** lines, size_t line_count)
{
	char* grid = build_grid(lines, line_count);
	char* copy = build_grid(lines, line_count);

	int width = strlen(lines[0]);
	int height = line_count;

	int count = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (grid[x + y * width] == '@')
			{
				int open = count_open(grid, width, height, x, y);
				copy[x + y * width] = open + '0';
				if (open < 4)
					count++;
			}
		}
	}

	printf("Num accessible: %d\n", count);
}

void part2(char* data, size_t data_length, char** lines, size_t line_count)
{
	char* grid = build_grid(lines, line_count);
	char* next = build_grid(lines, line_count);

	int width = strlen(lines[0]);
	int height = line_count;

	int count = 0;
	do {
		int removed = remove_rolls(grid, next, width, height);
		if (removed == 0) break;
		count += removed;
		strcpy(grid, next);
	} while(1);

	printf("Num removed: %d\n", count);
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
		snprintf(data_file_name, 64, "puzzle_04p%d.txt", i + 1);

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