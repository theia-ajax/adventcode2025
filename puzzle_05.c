#include "common.c"

#define TEST 0

#if TEST
#define SELECT(test, prod) (test)
#else
#define SELECT(test, prod) (prod)
#endif

const char* k_test_data = "3-5\n"
						  "10-14\n"
						  "16-20\n"
						  "12-18\n"
						  "\n"
						  "1\n"
						  "5\n"
						  "8\n"
						  "11\n"
						  "17\n"
						  "32\n";

struct range
{
	uint64_t start, end;
};

bool is_fresh(uint64_t id, const struct range* ranges, size_t ranges_count)
{
	for (size_t i = 0; i < ranges_count; i++)
	{
		const struct range* range = &ranges[i];
		if (id >= range->start && id <= range->end)
		{
			return true;
		}
	}
	return false;
}

void part1(char* data, size_t data_length, char** lines, size_t line_count)
{
	struct range* ranges = NULL;
	arrsetcap(ranges, line_count);

	int fresh_count = 0;

	for (size_t i = 0; i < line_count; i++)
	{
		char* line = lines[i];

		if (strstr(line, "-"))
		{
			struct range r = {0};
			sscanf(line, "%llu-%llu", &r.start, &r.end);
			arrput(ranges, r);
			// printf("Added range [%llu-%llu]\n", r.start, r.end);
		}
		else
		{
			uint64_t id;
			if (sscanf(line, "%llu", &id) == 1)
			{
				// printf("Testing id %llu ", id);
				if (is_fresh(id, ranges, arrlen(ranges)))
				{
					// printf("FRESH\n");
					fresh_count++;
				}
				else
				{

					// printf("NOT FRESH\n");
				}
			}
		}
	}

	printf("Fresh count: %d\n", fresh_count);

	//
}

bool range_contains(const struct range* r, uint64_t v)
{
	return v >= r->start && v <= r->end;
}

bool range_intersect(const struct range* r1, const struct range* r2)
{
	return range_contains(r1, r2->start) || range_contains(r1, r2->end);
}

void part2(char* data, size_t data_length, char** lines, size_t line_count)
{
	struct range* ranges = NULL;
	arrsetcap(ranges, line_count);

	uint64_t total_fresh_count = 0;

	for (size_t i = 0; i < line_count; i++)
	{
		char* line = lines[i];
		struct range* delete_ranges = NULL;
		arrsetcap(delete_ranges, arrlen(ranges) * 2);

		if (strstr(line, "-"))
		{
			struct range r;
			sscanf(line, "%llu-%llu", &r.start, &r.end);
			bool fully_consumed = false;

			struct range* new_ranges = NULL;

			printf("Range [%llu-%llu] (%llu)\n", r.start, r.end, r.end - r.start + 1);
			for (size_t j = 0, jc = arrlen(ranges); j < jc; j++)
			{
				const struct range* r2 = &ranges[j];

				// existing range consumes entire new range
				if (range_contains(r2, r.start) && range_contains(r2, r.end))
				{
					fully_consumed = true;
					printf("\tSwalloed whole by [%llu-%llu]\n", r2->start, r2->end);
					break;
				}

				if (range_intersect(&r, r2))
				{
					if (range_contains(&r, r2->start) && range_contains(&r, r2->end))
					{
						arrput(delete_ranges, *r2);
					}
					else
					{
						if (range_contains(&r, r2->start))
						{
							struct range d = {r2->start, r.end};
							arrput(delete_ranges, ((struct range){r2->start, r.end}));
						}
						else if (range_contains(&r, r2->end))
						{
							arrput(delete_ranges, ((struct range){r.start, r2->end}));
						}
					}
				}
			}

			if (!fully_consumed)
			{
				// To finish:
				// 1) combine delete ranges that intersect
				// 2) sum sizes of merged delete ranges
				// 3) subtract that sum from the total size of the current range

				printf("\tdelete ranges: %d\n", arrlen(delete_ranges));
				for (intptr_t di = 0, dc = arrlen(delete_ranges); di < dc - 1; di++)
				{
					for (intptr_t dj = di + 1; dj < dc; dj++)
					{
						struct range *d1 = &delete_ranges[di], *d2 = &delete_ranges[dj];
						if (range_intersect(d1, d2))
						{
							struct range t1 = *d1, t2 = *d2;
							d1->start = MIN(d1->start, d2->start);
							d1->end = MAX(d1->end, d2->end);
							d2->start = d2->end = 0;
							printf(
								"\t\tMERGE [%llu-%llu] and [%llu-%llu] -> [%llu-%llu]\n",
								t1.start,
								t1.end,
								t2.start,
								t2.end,
								d1->start,
								d1->end);
						}
					}
				}

				uint64_t del_count = 0;
				for (intptr_t di = 0, dc = arrlen(delete_ranges); di < dc; di++)
				{
					const struct range* d = &delete_ranges[di];
					if (d->start > 0 && d->end > 0)
					{
						uint64_t range_size = d->end - d->start + 1;
						printf("\t\tDELETE [%llu-%llu] (%llu)\n", d->start, d->end, range_size);
						del_count += range_size;
					} else {
						printf("\t\tIGNORE [%llu-%llu]\n", d->start, d->end);
					}
				}

				uint64_t fresh_count = r.end - r.start + 1 - del_count;

				total_fresh_count += fresh_count;
				printf("\tFresh %llu\n\tTotal %llu\n", fresh_count, total_fresh_count);
				arrput(ranges, r);
			}

			arrfree(delete_ranges);
		}
		else
		{
			break;
		}
	}

	printf("Total fresh count: %llu\n", total_fresh_count);
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
		snprintf(data_file_name, 64, "puzzle_05p%d.txt", i + 1);

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