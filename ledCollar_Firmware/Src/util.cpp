#include "util.h"
#include "globals.h"
#include <errno.h>
#include <sys/unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

extern "C" {
    // enable printf functionality on PRINTF_UART
    int _write(int file, char *data, int len) {
        if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
            errno = EBADF;
            return -1;
        }

        HAL_StatusTypeDef status = HAL_UART_Transmit(&PRINTF_UART, (uint8_t *)data, len, 1000);
        return (status == HAL_OK ? len : 0);
    }
}

void _printf(const char* fmt, ...) {
    char buf[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    HAL_UART_Transmit(&PRINTF_UART, (uint8_t*)buf, strlen(buf), 1000);
    va_end(args);
}

uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint32_t interpolateSegment(uint32_t x, uint32_t x0, uint32_t x1, uint32_t y0, uint32_t y1) {
    if (x <= x0) {
        return y0;
    }
    if (x >= x1) {
        return y1;
    }
    
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0); // interpolation formula but rearranged to suit integer calculation without precision loss (hopefully)
}

uint32_t linearInterpolate(uint32_t inputValue, const uint32_t lookupX[], const uint32_t lookupY[], size_t lookupSize) {

    // clamp input value
    if (inputValue <= lookupX[0]) {
        return lookupY[0];
    }
    else if (inputValue >= lookupX[lookupSize - 1]) {
        return lookupY[lookupSize - 1];
    }

    // find segment in lookup table that includes the input value
    for (size_t i = 0; i < lookupSize - 1; i++) {
        uint32_t x0 = lookupX[i];
        uint32_t x1 = lookupX[i + 1];
        if (inputValue >= x0 && inputValue <= x1) {  // check if value is in range of current lookup segment
            // do LERP calc
            return interpolateSegment(inputValue, x0, x1, lookupY[i], lookupY[i + 1]);
        }
    }

    return 0;   // shouldn't be reached, but return 0 in case I made an error
}