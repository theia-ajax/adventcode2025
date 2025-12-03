#include "common.c"

bool is_invalid_id1(const char* idstr, size_t len)
{
	// ids of odd length can never be invalid
	if (len % 2 != 0)
	{
		return false;
	}
	size_t hlen = len / 2;
	for (size_t i = 0; i < hlen; i++)
	{
		if (idstr[i] != idstr[i + hlen])
		{
			return false;
		}
	}

	return true;
}

bool is_invalid_id2(const char* idstr, size_t len)
{
	for (size_t view_size = 1; view_size <= len / 2; view_size++)
	{
		if (len % view_size != 0)
			continue;

		bool is_valid = false;

		char chunk_buf[256] = {0};
		for (size_t chunk = 0; chunk < len / view_size; chunk++)
		{
			for (size_t i = 0; i < view_size; i++)
			{
				if (chunk == 0)
				{
					chunk_buf[i] = idstr[i];
				}
				else
				{
					if (chunk_buf[i] != idstr[chunk * view_size + i])
					{
						is_valid = true;
						break;
					}
				}
			}
			if (is_valid)
				break;
		}

		if (!is_valid) {
			return true;
		}
	}

	return false;
}

uint64_t sum_invalid_ids_in_range(uint64_t first, uint64_t last, bool (*is_invalid)(const char*, size_t))
{
	uint64_t sum = 0;

	for (uint64_t id = first; id <= last; id++)
	{
		char idstr[64];
		snprintf(idstr, 64, "%llu", id);
		if (is_invalid(idstr, strlen(idstr)))
		{
			sum += id;
		}
	}

	return sum;
}

void process_ids(char** tokens, bool (*is_invalid)(const char*, size_t))
{
	uint64_t sum = 0;
	for (int index = 0; index < arrlen(tokens); index++)
	{
		uint64_t first, last;
		sscanf(tokens[index], "%llu-%llu", &first, &last);
		uint64_t range_sum = sum_invalid_ids_in_range(first, last, is_invalid);
		sum += range_sum;
	}
	printf("Sum of invalid ids: %llu\n", sum);
}

int main()
{
	size_t data_len;
	char* data = read_entire_file("puzzle_02p1.txt", &data_len);
	char** lines = split_lines(data, data_len);

	const char* test_input =
		"11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124";

	const char* input = data;
	char** tokens = strsplit(input, strlen(input), ",");

	// is_invalid_id2("1010", 4);

	printf("--- Part 1 --------\n");
	process_ids(tokens, is_invalid_id1);
	printf("--- Part 2 --------\n");
	process_ids(tokens, is_invalid_id2);

	freetok(tokens);
	freetok(lines);
	free(data);

	return 0;
}