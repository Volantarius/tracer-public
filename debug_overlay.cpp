#include "general.h"
#include "debug_overlay.h"

#include "vector.h"

#include <assert.h>

#include <iostream>
#include <cstdio>

#define MAX_LINES 1024

namespace debug {
debug_line *lines;
unsigned int line_index;

void init() {
	lines = (debug_line*)malloc(sizeof(debug_line) * MAX_LINES);
	
	if (lines == NULL) {
		printf("Can't allocate area portals!\n");
		assert(lines != NULL);
	};
	
	line_index = 0;
};

debug_line* get_lines() {
	return lines;
};

void get_line_count(unsigned int &out) {
	out = line_index;
};

void add_line(v3 &a, v3 &b, const v3 &color) {
	if ((line_index + 2) <= MAX_LINES) {
		lines[line_index].position = a;
		lines[line_index].color = color;
		line_index++;
		lines[line_index].position = b;
		lines[line_index].color = color;
		line_index++;
	};
};

void reset() {
	line_index = 0;
};

void shutdown() {
	free(lines);
};

};// namespace debug
