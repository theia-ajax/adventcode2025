#include "tools.h"

#include <sys/stat.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

char* read_entire_file(const char* file_name, size_t* out_length)
{
	FILE* file = fopen(file_name, "r");
	char* buffer = NULL;

	if (!file)
		goto _on_error;

	struct stat file_stats;
	stat(file_name, &file_stats);
	size_t file_size = file_stats.st_size;

	if (file_size == 0)
		goto _on_error;

	buffer = calloc(file_size + 1, sizeof(char));

	if (!buffer)
		goto _on_error;

	size_t chars_read = fread(buffer, sizeof(char), file_size, file);
	if (chars_read < file_size)
	{
		int eof = feof(file);
		if (!eof)
			fprintf(stderr, "%s -- expected eof\n", file_name);
		int err = ferror(file);
		if (err)
			fprintf(stderr, "%s -- unexpected error %d\n", file_name, err);

		const bool has_error = !eof || err;
		if (has_error)
		{
			goto _on_error;
		}
	}

	if (out_length)
		*out_length = chars_read ? chars_read + 1 : 0;

	return buffer;

_on_error:
	free(buffer);
	free(file);
	return NULL;
}

char** split_lines(const char* buffer, size_t buffer_length)
{
	char** list = NULL;
	arrsetcap(list, 1024);

	char* buffer_copy = calloc(buffer_length, sizeof(char));
	strncpy(buffer_copy, buffer, buffer_length);

	const char* delim = "\n";
	char* line = strtok(buffer_copy, delim);
	while (line) {
		size_t line_length = strlen(line);
		char* line_copy = calloc(line_length + 1, sizeof(char));
		strncpy(line_copy, line, line_length);
		arrput(list, line_copy);
		line = strtok(NULL, delim);
	}

	free(buffer_copy);

	return list;
}

void free_lines(char** lines)
{
	for (int i = 0; i < arrlen(lines); i++) {
		free(lines[i]);
	}
	arrfree(lines);
}