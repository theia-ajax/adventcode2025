#pragma once

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb_ds.h"

char* read_entire_file(const char* file_name, size_t* out_length);
char** split_lines(const char* buffer, size_t buffer_length);
void free_lines(char** lines);
