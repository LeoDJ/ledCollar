#pragma once
#include <stdint.h>
#include <stddef.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

// legacy printf function with fixed buffer length
void _printf(const char* fmt, ...);

uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);

uint32_t linearInterpolate(uint32_t inputValue, const uint32_t lookupX[], const uint32_t lookupY[], size_t lookupSize);
