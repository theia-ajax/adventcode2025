#include "common.c"

#define TEST 0

#if TEST
#define SELECT(test, prod) (test)
#else
#define SELECT(test, prod) (prod)
#endif

const char* k_test_data = "7,1\n"
						  "11,1\n"
						  "11,7\n"
						  "9,7\n"
						  "9,5\n"
						  "2,5\n"
						  "2,3\n"
						  "7,3\n";

struct point
{
	int x, y;
};

struct context
{
	char* data;
	size_t data_length;
	char** lines;
	size_t line_count;

	struct point* points;
	struct point minp, maxp;

	char* field;
	int field_w, field_h;
};

int64_t area(struct point p1, struct point p2)
{
	int x1 = MIN(p1.x, p2.x);
	int x2 = MAX(p1.x, p2.x);
	int y1 = MIN(p1.y, p2.y);
	int y2 = MAX(p1.y, p2.y);

	return (int64_t)(x2 - x1 + 1) * (int64_t)(y2 - y1 + 1);
}

void part1(struct context* ctx)
{
	//
	arrsetcap(ctx->points, ctx->line_count);

	struct point minp = {INT_MAX, INT_MAX};
	struct point maxp = {0};

	for (size_t i = 0; i < ctx->line_count; i++)
	{
		struct point p;
		sscanf(ctx->lines[i], "%d,%d", &p.x, &p.y);
		arrput(ctx->points, p);
		if (p.x < minp.x)
			minp.x = p.x;
		if (p.y < minp.y)
			minp.y = p.y;
		if (p.x > maxp.x)
			maxp.x = p.x;
		if (p.y > maxp.y)
			maxp.y = p.y;
	}

	ctx->minp = minp;
	ctx->maxp = maxp;

	int count = arrlen(ctx->points);
	int64_t max_area = 0;
	for (int i1 = 0; i1 < count - 1; i1++)
	{
		for (int i2 = i1 + 1; i2 < count; i2++)
		{
			int64_t a = area(ctx->points[i1], ctx->points[i2]);
			if (a > max_area)
				max_area = a;
		}
	}

	printf("Max area: %lli\n", max_area);
}

void print_field(char* field, int width, int height)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			putc(field[x + y * width], stdout);
		}
		putc('\n', stdout);
	}
}

void field_set(struct context* ctx, struct point p, char v)
{
	int px = p.x - ctx->minp.x;
	int py = p.y - ctx->minp.y;
	if (px < 0 || py < 0 || px >= ctx->field_w || py >= ctx->field_h)
	{
		return;
	}
	int64_t idx = (int64_t)px + (int64_t)py * (int64_t)ctx->field_w;
	ctx->field[idx] = v;
}

char field_get(struct context* ctx, struct point p)
{
	int px = p.x - ctx->minp.x;
	int py = p.y - ctx->minp.y;
	if (px < 0 || py < 0 || px >= ctx->field_w || py >= ctx->field_h)
	{
		return ' ';
	}
	int64_t idx = (int64_t)px + (int64_t)py * (int64_t)ctx->field_w;
	return ctx->field[idx];
}

void flood_fill(struct context* ctx, struct point p, char find, char with)
{
	if (field_get(ctx, p) == find)
	{
		field_set(ctx, p, with);
		flood_fill(ctx, (struct point){p.x + 1, p.y}, find, with);
		flood_fill(ctx, (struct point){p.x - 1, p.y}, find, with);
		flood_fill(ctx, (struct point){p.x, p.y - 1}, find, with);
		flood_fill(ctx, (struct point){p.x, p.y + 1}, find, with);
	}
}

bool is_green(char c)
{
	return c == '#' || c == 'X';
}

void field_hline(struct context* ctx, struct point p1, struct point p2, char find, char with)
{
	for (int x = p1.x; x < p2.x; x++)
	{
		struct point p = {x, p1.y};
		if (field_get(ctx, p) == find)
		{
			field_set(ctx, p, with);
		}
	}
}

void field_vline(struct context* ctx, struct point p1, struct point p2, char find, char with)
{
	for (int y = p1.y; y < p2.y; y++)
	{
		struct point p = {p1.x, y};
		if (field_get(ctx, p) == find)
		{
			field_set(ctx, p, with);
		}
	}
}

bool inside_green(struct context* ctx, struct point p)
{
	struct point r = {p.x + 1, p.y};
	struct point l = {p.x - 1, p.y};
	struct point u = {p.x, p.y - 1};
	struct point d = {p.x, p.y + 1};

	do
	{
		char c = field_get(ctx, r);
		if (c == ' ')
		{
			return false;
		}
		else if (is_green(c))
		{
			break;
		}
		r.x++;
	} while (1);
	do
	{
		char c = field_get(ctx, l);
		if (c == ' ')
		{
			return false;
		}
		else if (is_green(c))
		{
			break;
		}
		l.x--;
	} while (1);

	field_hline(ctx, l, r, '.', 'X');

	do
	{
		char c = field_get(ctx, d);
		if (c == ' ')
		{
			return false;
		}
		else if (is_green(c))
		{
			break;
		}
		d.y++;
	} while (1);
	do
	{
		char c = field_get(ctx, u);
		if (c == ' ')
		{
			return false;
		}
		else if (is_green(c))
		{
			break;
		}
		u.y--;
	} while (1);

	field_vline(ctx, u, d, '.', 'X');


	return true;
}

// returns 0 if rectangle is not all green/red
int64_t area2(struct context* ctx, struct point p1, struct point p2)
{
	int x1 = MIN(p1.x, p2.x);
	int x2 = MAX(p1.x, p2.x);
	int y1 = MIN(p1.y, p2.y);
	int y2 = MAX(p1.y, p2.y);

	for (int x = x1 + 1; x < x2; x++)
	{
		struct point p1 = (struct point){x, y1};
		struct point p2 = (struct point){x, y2};
		char v1 = field_get(ctx, p1);
		char v2 = field_get(ctx, p2);
		if (!is_green(v1) && !inside_green(ctx, p1))
		{
			return 0;
		}
		if (!is_green(v2) && !inside_green(ctx, p2))
		{
			return 0;
		}
	}

	for (int y = y1 + 1; y < y2; y++)
	{
		struct point p1 = (struct point){x1, y};
		struct point p2 = (struct point){x2, y};
		char v1 = field_get(ctx, p1);
		char v2 = field_get(ctx, p2);
		if (!is_green(v1) && !inside_green(ctx, p1))
		{
			return 0;
		}
		if (!is_green(v2) && !inside_green(ctx, p2))
		{
			return 0;
		}
	}

	return (int64_t)(x2 - x1 + 1) * (int64_t)(y2 - y1 + 1);
}

void part2(struct context* ctx)
{
	//

	ctx->field_w = ctx->maxp.x - ctx->minp.x + 2;
	ctx->field_h = ctx->maxp.y - ctx->minp.y + 2;

	size_t field_size = (size_t)ctx->field_w * (size_t)ctx->field_h;
	ctx->field = malloc(field_size);
	memset(ctx->field, '.', field_size);

	struct point last = {0};
	int count = arrlen(ctx->points);
	for (int i = 0; i < count + 1; i++)
	{
		struct point curr = ctx->points[i % count];
		field_set(ctx, curr, '#');

		if (last.x != 0 && last.y != 0)
		{
			if (last.x == curr.x)
			{
				int ystart = MIN(curr.y, last.y) + 1;
				int yend = MAX(curr.y, last.y);
				for (int y = ystart; y < yend; y++)
				{
					field_set(ctx, (struct point){curr.x, y}, 'X');
				}
			}
			else if (last.y == curr.y)
			{
				int xstart = MIN(curr.x, last.x) + 1;
				int xend = MAX(curr.x, last.x);
				for (int x = xstart; x < xend; x++)
				{
					field_set(ctx, (struct point){x, curr.y}, 'X');
				}
			}
		}

		last = ctx->points[i];
	}
	
	int64_t max_area = 0;
	for (int i1 = 0; i1 < count - 1; i1++)
	{
		printf("%d\n", i1);
		for (int i2 = i1 + 1; i2 < count; i2++)
		{
			int64_t a = area(ctx->points[i1], ctx->points[i2]);
			if (a > max_area)
			{
				int64_t a2 = area2(ctx, ctx->points[i1], ctx->points[i2]);
				if (a2 > max_area)
				max_area = a2;
			}
		}
	}
	
	// print_field(ctx->field, ctx->field_w, ctx->field_h);
	printf("Max area: %lli\n", max_area);
}

int main()
{
	size_t test_data_length = strlen(k_test_data);
	char* test_data = calloc(test_data_length + 1, sizeof(char));
	strcpy(test_data, k_test_data);

	typedef void (*part_func)(struct context*);
	part_func parts[2] = {part1, part2};

	size_t data_length;
	char* data = read_entire_file("puzzle_09.txt", &data_length);
	char** lines = NULL;

	char* input = SELECT(test_data, data);
	size_t input_length = SELECT(test_data_length, data_length);

	if (input)
	{
		lines = split_lines(input, input_length);

		struct context ctx = {
			.data = data,
			.data_length = data_length,
			.lines = lines,
			.line_count = arrlenu(lines),
		};

		for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); i++)
		{
			printf("\n---- Part %d ----\n", i + 1);
			parts[i](&ctx);
		}

		freetok(lines);
	}

	free(data);
	free(test_data);

	return 0;
}