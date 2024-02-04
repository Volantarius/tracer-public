#ifndef DEBUG_OVERLAY_H
#define DEBUG_OVERLAY_H
#ifdef _WIN32
#pragma once
#endif

#include "vector.h"

namespace debug {
struct debug_line {
	v3 position;
	v3 color;
};

void init();

debug_line* get_lines();
void get_line_count(unsigned int &out);

void add_line(v3 &a, v3 &b, const v3 &color);
//extern inline void add_line(v3 &a, v3 &b, v3 color);

void reset();

void shutdown();
};

#endif
