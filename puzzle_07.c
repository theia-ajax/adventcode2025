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

struct point
{
	int x, y;
};

enum side
{
	side_left,
	side_right
};

struct mtree_node
{
	char* name;
	int x, y;
	int id;
	struct mtree_node* l;
	struct mtree_node* r;
};

struct mtree_map
{
	uint32_t key;
	struct mtree_node* value;
};

uint32_t manifold_key(int x, int y)
{
	return (y & 0xFFFF) << 16 | (x & 0xFFFF);
}

struct manifold
{
	int width, height;
	struct point start;
	int iter;
	int splits;
	char* data;
	struct mtree_node* tree;
	struct mtree_node* exit;
	struct mtree_map* map;
};

char* _manifold_get_ptr(const struct manifold* self, int x, int y)
{
	if (x < 0 || y < 0 || x >= self->width || y >= self->height)
	{
		return NULL;
	}

	return &self->data[x + y * self->width];
}

char manifold_get(const struct manifold* self, int x, int y)
{
	char* c = _manifold_get_ptr(self, x, y);
	if (c)
	{
		return *c;
	}
	else
	{
		return ' ';
	}
}

struct mtree_node* manifold_get_node(struct manifold* self, int x, int y)
{
	uint32_t key = manifold_key(x, y);
	return hmget(self->map, key);
}

void manifold_set(struct manifold* self, int x, int y, char value)
{
	char* c = _manifold_get_ptr(self, x, y);
	if (c)
	{
		*c = value;
	}
}

void manifold_print(const struct manifold* self)
{
	for (int y = 0; y < self->height; y++)
	{
		for (int x = 0; x < self->width; x++)
		{
			putc(manifold_get(self, x, y), stdout);
		}
		putc('\n', stdout);
	}
	putc('\n', stdout);
}

void manifold_split(struct manifold* self, int sx, int sy)
{
	self->splits++;
	manifold_set(self, sx - 1, sy, '|');
	manifold_set(self, sx + 1, sy, '|');
}

bool manifold_iterate(struct manifold* self)
{
	if (self->iter == 0)
	{
		manifold_set(self, self->start.x, self->start.y + 1, '|');
		self->iter = 1;
		return true;
	}
	else if (self->iter == self->height - 1)
	{
		return false;
	}

	for (int x = 0; x < self->width; x++)
	{
		if (manifold_get(self, x, self->iter) == '|')
		{
			int y = self->iter + 1;
			char next = manifold_get(self, x, y);
			if (next == '.')
			{
				manifold_set(self, x, y, '|');
			}
			else if (next == '^')
			{
				manifold_split(self, x, y);
			}
		}
	}
	self->iter++;

	return true;
}

struct mtree_node* manifold_find_target(struct manifold* self, int x, int y)
{
	while (manifold_get(self, x, y) == '|')
	{
		y++;
	}

	if (manifold_get(self, x, y) == '^')
	{
		uint32_t key = manifold_key(x, y);
		return hmget(self->map, key);
	}
	return self->exit;
}

void manifold_build_tree(struct manifold* self)
{
	int* heights = calloc(self->width, sizeof(int));
	for (int x = 0; x < self->width; x++)
	{
		heights[x] = self->height;
	}

	for (int y = self->height - 1; y > 0; y--)
	{
		for (int x = 0; x < self->width; x++)
		{
			if (manifold_get(self, x, y) == '|')
			{
				int yy = y;
				do
				{
					if (manifold_get(self, x - 1, yy) == '^')
					{
						manifold_get_node(self, x - 1, yy)->r = manifold_find_target(self, x, yy);
					}
					if (manifold_get(self, x + 1, yy) == '^')
					{
						manifold_get_node(self, x + 1, yy)->l = manifold_find_target(self, x, yy);
					}
					yy--;
				} while (manifold_get(self, x, yy) != '.');
			}
		}
	}
}

static int64_t count_paths_iterations = 0;
static int count_paths_max_depth = 0;
static int count_paths_furthest_x = 0;
static int count_paths_furthest_y = 0;
static int64_t count_paths_num = 0;

struct mtree_node_seen
{
	uint32_t key;
	bool value;
};

struct mtree_node_seen* seen_add(struct mtree_node_seen* seen, struct mtree_node* node)
{
	uint32_t key = manifold_key(node->x, node->y);
	if (hmget(seen, key) == false)
	{
		hmput(seen, key, true);
		printf("new node seen: %s seen size %d\n", node->name, hmlen(seen));
	}
	return seen;
}

int64_t manifold_count_paths_helper(struct manifold* self, struct mtree_node* node, int depth, int64_t* memo)
{

	if (depth > count_paths_max_depth)
	{
		count_paths_max_depth = depth;
		printf("New max depth: %d\n", count_paths_max_depth);
	}

	if (node == self->exit)
	{
		return 1;
	}
	else
	{
		count_paths_iterations++;

		if (count_paths_iterations % 1000000000 == 0)
		{
			printf("Iterations: %llu current node: %s                      \r", count_paths_iterations, node->name);
		}
		if (node->y >= count_paths_furthest_y)
		{
			if (node->y > count_paths_furthest_y)
			{
				count_paths_furthest_x = 0;
			}
			if (node->x > count_paths_furthest_x)
			{
				count_paths_furthest_x = node->x;
				printf("Node furthest node: %s %lli\n", node->name, count_paths_num);
			}
			count_paths_furthest_y = node->y;
		}

		count_paths_num += 2;

		if (memo[node->id] == 0)
		{

			// struct mtree_node_seen* seen2 = seen_add(seen, node);
			// seen = seen2;
			memo[node->id] = manifold_count_paths_helper(self, node->l, depth + 1, memo) +
							 manifold_count_paths_helper(self, node->r, depth + 1, memo);

			printf("Node: %s contains %llu splits\n", node->name, memo[node->id]);
		}

		return memo[node->id];
	}
}

int64_t manifold_count_paths(struct manifold* self)
{
	struct mtree_node_seen* seen = NULL;
	hmdefault(seen, false);
	count_paths_iterations = 0;
	count_paths_max_depth = 0;
	count_paths_furthest_x = 0;
	count_paths_furthest_y = 0;
	count_paths_num = 0;
	int64_t* memo = calloc(hmlen(self->map), sizeof(int64_t));
	return manifold_count_paths_helper(self, self->tree, 0, memo);
}

void init_manifold(struct manifold* self, char** lines, size_t line_count)
{
	if (line_count == 0)
		return;

	memset(self, 0, sizeof(struct manifold));
	self->width = strlen(lines[0]);
	self->height = line_count;

	self->data = calloc(self->width * self->height, sizeof(char));
	self->exit = calloc(1, sizeof(struct mtree_node));
	memset(self->exit, 0, sizeof(struct mtree_node));
	self->exit->name = "__exit_";

	int node_id = 0;

	for (size_t y = 0; y < line_count; y++)
	{
		strcpy(self->data + y * self->width, lines[y]);

		for (int x = 0; x < self->width; x++)
		{
			if (lines[y][x] == 'S')
			{
				self->start.x = x;
				self->start.y = y;
				manifold_set(self, x, y, '.');
			}
			else if (lines[y][x] == '^')
			{
				struct mtree_node* node = calloc(1, sizeof(struct mtree_node));
				memset(node, 0, sizeof(struct mtree_node));
				uint32_t key = manifold_key(x, y);
				node->name = calloc(64, sizeof(char));
				node->x = x;
				node->y = y;
				node->id = node_id++;
				snprintf(node->name, 64, "%03d_%03d", x, y);
				hmput(self->map, key, node);
				if (self->tree == NULL)
				{
					self->tree = node;
				}
			}
		}
	}

	hmdefault(self->map, NULL);
}

void part1(char* data, size_t data_length, char** lines, size_t line_count)
{
	//
	printf("\n");

	struct manifold manifold;
	init_manifold(&manifold, lines, line_count);

	manifold_print(&manifold);

	while (manifold_iterate(&manifold))
	{

		// manifold_print(&manifold);
	}
	manifold_print(&manifold);

	printf("Splits: %d\n", manifold.splits);

	manifold_build_tree(&manifold);
	for (int i = 0; i < hmlen(manifold.map); i++)
	{
		struct mtree_node* node = manifold.map[i].value;

		printf(
			"Node %s: L = %s R = %s\n",
			node->name,
			node->l != NULL ? node->l->name : "null",
			node->r != NULL ? node->r->name : "null");
	}

	printf("Manifold node count: %d\n", hmlen(manifold.map));

	int64_t paths = manifold_count_paths(&manifold);
	printf("Paths: %llu\n", paths);
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