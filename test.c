#include "tools.c"

int main()
{
	size_t test_length;
	char* test_data = read_entire_file("test_data.txt", &test_length);
	char** lines = split_lines(test_data, test_length);
	
	printf("file contents:\n%s\n", test_data);

	printf("file lines:\n");
	for (int i = 0; i < arrlen(lines); i++) {
		printf("%04d: %s\n", i, lines[i]);
	}

	free_lines(lines);

	return 0;
}