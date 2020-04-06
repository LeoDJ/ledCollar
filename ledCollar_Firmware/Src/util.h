#pragma once

int _write(int file, char *data, int len);

// legacy printf function with fixed buffer length
void _printf(const char* fmt, ...);