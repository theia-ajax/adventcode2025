#include "common.c"

#define TEST 0

#if TEST
#define SELECT(test, prod) (test)
#else
#define SELECT(test, prod) (prod)
#endif

const char* k_test_data = "162,817,812\n"
						  "57,618,57\n"
						  "906,360,560\n"
						  "592,479,940\n"
						  "352,342,300\n"
						  "466,668,158\n"
						  "542,29,236\n"
						  "431,825,988\n"
						  "739,650,466\n"
						  "52,470,668\n"
						  "216,146,977\n"
						  "819,987,18\n"
						  "117,168,530\n"
						  "805,96,715\n"
						  "346,949,466\n"
						  "970,615,88\n"
						  "941,993,340\n"
						  "862,61,35\n"
						  "984,92,344\n"
						  "425,690,689\n";

struct junction
{
	int x, y, z;
	double* distances;
	int circuit;
};

struct junction_link
{
	int j1, j2;
	double dist;
};

struct circuit_size
{
	int circuit;
	int size;
};

int junction_link_dist_cmp(const void* a, const void* b)
{
	const struct junction_link* j1 = (const struct junction_link*)a;
	const struct junction_link* j2 = (const struct junction_link*)b;
	if (j1->dist < j2->dist)
	{
		return -1;
	}
	else if (j1->dist > j2->dist)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

double junction_distance(const struct junction* j1, const struct junction* j2)
{
	double dx = (double)(j2->x - j1->x);
	double dy = (double)(j2->y - j1->y);
	double dz = (double)(j2->z - j1->z);
	return sqrt(dx * dx + dy * dy + dz * dz);
}

void connect_junctions(struct junction* junctions, int id1, int id2, int* next_circuit_id)
{
	struct junction* j1 = &junctions[id1];
	struct junction* j2 = &junctions[id2];

	if (j1->circuit == 0 && j2->circuit == 0)
	{
		j1->circuit = *next_circuit_id;
		j2->circuit = *next_circuit_id;
		*next_circuit_id = *next_circuit_id + 1;
	}
	else if (j1->circuit != j2->circuit)
	{
		int chosen_circuit = 0;
		int other_circuit = 0;
		if (j1->circuit != 0)
		{
			chosen_circuit = j1->circuit;
			other_circuit = j2->circuit;
		}
		else if (j2->circuit != 0)
		{
			chosen_circuit = j2->circuit;
			other_circuit = j1->circuit;
		}

		j1->circuit = chosen_circuit;
		j2->circuit = chosen_circuit;
		if (other_circuit != 0)
		{
			for (int i = 0; i < arrlen(junctions); i++)
			{
				if (junctions[i].circuit == other_circuit)
				{
					junctions[i].circuit = chosen_circuit;
				}
			}
		}
	}
}

int count_circuit_size(struct junction* junctions, int circuit)
{
	int count = 0;
	for (int i = 0; i < arrlen(junctions); i++)
	{
		if (junctions[i].circuit == circuit)
		{
			count++;
		}
	}
	return count;
}

bool all_same_circuit(struct junction* junctions)
{
	int circuit = junctions[0].circuit;
	if (circuit == 0) return false;

	for (int i = 1; i < arrlen(junctions); i++)
	{
		if (junctions[i].circuit != circuit)
		{
			return false;
		}
	}

	return true;
}

int circuit_size_cmp(const void* a, const void* b)
{
	const struct circuit_size* c1 = (const struct circuit_size*)a;
	const struct circuit_size* c2 = (const struct circuit_size*)b;
	return c2->size - c1->size;
}

void part1(char* data, size_t data_length, char** lines, size_t line_count)
{
	//

	struct junction* junctions = NULL;
	struct junction_link* possible_links = NULL;
	arrsetcap(junctions, line_count);

	for (int i = 0; i < line_count; i++)
	{
		struct junction junction = {0};
		sscanf(lines[i], "%d,%d,%d", &junction.x, &junction.y, &junction.z);
		arrput(junctions, junction);
	}

	for (int i = 0; i < arrlen(junctions) - 1; i++)
	{
		for (int j = i + 1; j < arrlen(junctions); j++)
		{
			double dist = junction_distance(&junctions[i], &junctions[j]);
			struct junction_link link = {i, j, dist};
			arrput(possible_links, link);
		}
	}

	qsort(possible_links, arrlen(possible_links), sizeof(struct junction_link), junction_link_dist_cmp);

	int iterations = 1000;
#if TEST
	iterations = 10;
#endif

	for (int i = 0; i < MIN(arrlen(possible_links), iterations); i++)
	{
		printf("link: %d<->%d dist: %f\n", possible_links[i].j1, possible_links[i].j2, possible_links[i].dist);
	}

	int next_circuit = 1;
	int iter = 0;
	while (1)
	{
		struct junction_link next_link = possible_links[iter];
		connect_junctions(junctions, next_link.j1, next_link.j2, &next_circuit);

		if (iter == iterations)
		{
			struct circuit_size* circuits = NULL;
			arrsetcap(circuits, next_circuit);

			for (int i = 1; i < next_circuit; i++)
			{
				int size = count_circuit_size(junctions, i);
				struct circuit_size circuit = {i, size};
				arrput(circuits, circuit);
			}

			qsort(circuits, arrlen(circuits), sizeof(struct circuit_size), circuit_size_cmp);

			int cs1 = circuits[0].size;
			int cs2 = circuits[1].size;
			int cs3 = circuits[2].size;
			printf("%d * %d * %d = %d\n", cs1, cs2, cs3, cs1 * cs2 * cs3);
		}

		if (all_same_circuit(junctions)) {
			int64_t x1 = junctions[next_link.j1].x;
			int64_t x2 = junctions[next_link.j2].x;

			printf("%d<->%d  x1: %d x2: %d  prod: %llu\n", next_link.j1, next_link.j2, x1, x2, x1 * x2);

			break;
		}

		iter++;
	}

	printf("Iterations: %d\n", iter);

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
		snprintf(data_file_name, 64, "puzzle_08p%d.txt", i + 1);

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