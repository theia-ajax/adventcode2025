#include "common.h"

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

char** strsplit(const char* str, size_t len, const char* delim)
{
	char** list = NULL;
	arrsetcap(list, 1024);

	char* str_copy = calloc(len + 1, sizeof(char));
	strncpy(str_copy, str, len);

	char* next = strtok(str_copy, delim);
	while (next) {
		size_t next_len = strlen(next);
		char* next_copy = calloc(next_len + 1, sizeof(char));
		strncpy(next_copy, next, next_len);
		arrput(list, next_copy);
		next = strtok(NULL, delim);
	}

	free(str_copy);

	return list;
}

char** split_lines(const char* buffer, size_t buffer_length)
{
	return strsplit(buffer, buffer_length, "\n");
}

void freetok(char** tokens)
{
	for (int i = 0; i < arrlen(tokens); i++) {
		free(tokens[i]);
	}
	arrfree(tokens);
}