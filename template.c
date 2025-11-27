#include "tools.c"

void part1(char* data, size_t data_length, char** lines)
{
	//
}

void part2(char* data, size_t data_length, char** lines)
{
	//
}

int main()
{
	typedef void(*part_func)(char*, size_t, char**);
	part_func parts[2] = {
		part1, part2
	};

	for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); i++) {
		size_t data_length;
		char data_file_name[64];
		snprintf(data_file_name, 64, "<puzzle_id>p%d.txt", i + 1);
		char* data = read_entire_file(data_file_name, &data_length);
		if (data)
		{
			char** lines = split_lines(data, data_length);
			parts[i](data, data_length, lines);
			free_lines(lines);
			free(data);
		}
		else
			printf("\nPart %d data not found!\n\n", i + 1);
	}
	
	return 0;
}