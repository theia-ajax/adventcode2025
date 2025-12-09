#include "common.c"

#define TEST 0

#if TEST
#define SELECT(test, prod) (test)
#else
#define SELECT(test, prod) (prod)
#endif

const char* k_test_data = ".......S.......\n"
						  "...............\n"
						  ".......^.......\n"
						  "...............\n"
						  "......^.^......\n"
						  "...............\n"
						  ".....^.^.^.....\n"
						  "...............\n"
						  "....^.^...^....\n"
						  "...............\n"
						  "...^.^...^.^...\n"
						  "...............\n"
						  "..^...^.....^..\n"
						  "...............\n"
						  ".^.^.^.^.^...^.\n"
						  "...............";

struct manifold
{
	int width, height;
	int* splits;
	int64_t* counts;
	char* data;
};

char manifold_get(const struct manifold* self, int x, int y)
{
	if (x < 0 || y < 0 || x >= self->width || y >= self->height)
	{
		return ' ';
	}
	return self->data[x + y * self->width];
}

void part1(char* data, size_t data_length, char** lines, size_t line_count)
{
	//
	struct manifold manifold = {0};
	if (line_count == 0)
		return;

	manifold.width = strlen(lines[0]);
	manifold.height = line_count;
	manifold.data = calloc(manifold.width * manifold.height, sizeof(char));
	manifold.counts = calloc(manifold.width, sizeof(*manifold.counts));
	manifold.splits = calloc(manifold.width, sizeof(*manifold.splits));

	for (size_t y = 0; y < line_count; y++)
	{
		strcpy(manifold.data + y * manifold.width, lines[y]);
	}

	int split_count = 0;

	for (int y = 0; y < manifold.height; y++)
	{
		for (int x = 0; x < manifold.width; x++)
		{
			char c = manifold_get(&manifold, x, y);
			if (c == 'S')
			{
				manifold.splits[x] = 1;
				manifold.counts[x] = 1;
			}
			else if (c == '^')
			{
				if (manifold.splits[x] == 1)
				{
					manifold.splits[x - 1] = 1;
					manifold.splits[x + 1] = 1;
					manifold.splits[x] = 0;
					split_count++;
				}

				if (manifold.counts[x] > 0)
				{
					manifold.counts[x - 1] += manifold.counts[x];
					manifold.counts[x + 1] += manifold.counts[x];
					manifold.counts[x] = 0;
				}
			}
		}
	}

	int64_t total_count = 0;
	for (int x = 0; x < manifold.width; x++)
	{
		total_count += manifold.counts[x];
	}

	printf("Splits: %d\nCounts: %lli\n", split_count, total_count);
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
		snprintf(data_file_name, 64, "puzzle_07p%d.txt", i + 1);

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