#include "util.h"
#include "globals.h"
#include <errno.h>
#include <sys/unistd.h>
#include <stdarg.h>
#include <string.h>

// enable printf functionality on PRINTF_UART
int _write(int file, char *data, int len) {
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit(&PRINTF_UART, (uint8_t *)data, len, 1000);
    return (status == HAL_OK ? len : 0);
}

void _printf(const char* fmt, ...) {
  char buf[512];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  HAL_UART_Transmit(&PRINTF_UART, (uint8_t*)buf, strlen(buf), 1000);
  va_end(args);
}