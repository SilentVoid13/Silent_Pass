#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#define MAX_ARGUMENT 500

#define RED "\x1b[0;31m"
#define BOLD_RED "\x1b[1;31m"
#define GREEN "\x1b[0;32m"
#define BOLD_GREEN "\x1b[1;32m"
#define BLUE "\x1b[0;34m"
#define BOLD_BLUE "\x1b[1;34m"
#define YELLOW "\x1b[0;33m"
#define BOLD_YELLOW "\x1b[1;33m"
#define RESET "\x1b[0m"

void log_success(char *msg, ...);
void log_info(char *msg, ...);
void log_error(char *msg, ...);
void log_progress(char *msg, ...);

void log_format(const char* message, va_list args);
void log_format_error(const char *message, va_list args);

#endif // LOG_H
