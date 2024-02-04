#ifndef TIMESPLITTERS_H
#define TIMESPLITTERS_H
#ifdef _WIN32
#pragma once
#endif

unsigned char* ts_load_texture(int *width, int *height, const char *filename);

//unsigned char* ts_loadLevel(const char* filename);
bool ts_load_level(const char *filename);

//asd
#endif