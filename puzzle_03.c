#include "common.c"

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
	typedef void(*part_func)(char*, size_t, char**, size_t);
	part_func parts[2] = {
		part1, part2
	};

	for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); i++) {
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