#pragma once
#include <stdint.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

int _write(int file, char *data, int len);

// legacy printf function with fixed buffer length
void _printf(const char* fmt, ...);

uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);