#include "common.c"

int turn(int value, int turn)
{
	value += turn;
	while (value < 0)
		value += 100;
	while (value > 0)
		value -= 100;
	return value;
}

int count_zeros(int* turns, size_t count)
{
	int zero_count = 0;
	int dial = 50;
	for (size_t i = 0; i < count; i++)
	{
		dial = turn(dial, turns[i]);
		if (dial == 0)
			zero_count++;
	}
	return zero_count;
}

int count_passing_zeros(int* turns, size_t count)
{
	int dial = 50;
	int pass = 0;

	for (size_t i = 0; i < count; i++)
	{
		int turn = turns[i];
		int passed = 0;
		for (int j = 0; j < abs(turn); j++)
		{
			if (turn < 0)
			dial--;
			else
			dial++;
			if (dial < 0)
			dial += 100;
			if (dial > 99)
			dial -= 100;
			if (dial == 0)
				passed++;
			
		}
		printf("turn: %03d, amount: %d, dial: %d, p: %d\n", i, turn, dial, passed);
		pass += passed;
	}
	return pass;
}

int* parse_turns(char** lines, size_t count)
{
	int* turns = NULL;
	arrsetcap(turns, count);

	for (int i = 0; i < count; i++)
	{
		char dir_char;
		int amount;
		sscanf(lines[i], "%c%d", &dir_char, &amount);

		if (dir_char == 'L')
			amount = -amount;
		arrput(turns, amount);
	}
	return turns;
}

void part1(char* data, size_t data_length, char** lines)
{
	//

	int* turns = parse_turns(lines, arrlen(lines));

	int zeros = count_zeros(turns, arrlen(turns));
	arrfree(turns);
	printf("p1 count: %d\n", zeros);
}

void part2(char* data, size_t data_length, char** lines)
{
	char* test_turns[] = {
		"L68",
		"L30",
		"R48",
		"L5",
		"R60",
		"L55",
		"L1",
		"L99",
		"R14",
		"L82",
	};

	//
	int* turns = parse_turns(lines, arrlen(lines));

	int zeros = count_passing_zeros(turns, arrlen(turns));
	arrfree(turns);
	printf("p2 count: %d\n", zeros);
}

int main()
{
	typedef void (*part_func)(char*, size_t, char**);
	part_func parts[2] = {part1, part2};

	printf("\n");

	for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); i++)
	{
		size_t data_length;
		char data_file_name[64];
		snprintf(data_file_name, 64, "puzzle_01p%d.txt", i + 1);
		char* data = read_entire_file(data_file_name, &data_length);
		char** lines = NULL;

		if (data)
			lines = split_lines(data, data_length);

		parts[i](data, data_length, lines);

		freetok(lines);
		free(data);
	}

	return 0;
}