#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#define MAX_ARGUMENT 500

#define RED "\033[0;31m"
#define BOLD_RED "\033[1;31m"
#define GREEN "\033[0;32m"
#define BOLD_GREEN "\033[1;32m"
#define BLUE "\033[0;34m"
#define BOLD_BLUE "\033[1;34m"
#define YELLOW "\033[0;33m"
#define BOLD_YELLOW "\033[1;33m"
#define RESET "\033[0m"

void log_success(char *msg, ...);
void log_info(char *msg, ...);
void log_error(char *msg, ...);
void log_format(const char* message, va_list args);

#endif // LOG_H
