#ifndef LOG_WIN_H
#define LOG_WIN_H

#define _GNU_SOURCE

#include <stdio.h>
#include <io.h>
#include <windows.h>
#include <errno.h>

#ifndef FOREGROUND_MASK
#define FOREGROUND_MASK (FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY)
#endif

#ifndef BACKGROUND_MASK
#define BACKGROUND_MASK (BACKGROUND_RED|BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_INTENSITY)
#endif

int __write_w32(FILE* fp, const char* buf);
int _fprintf_w32(FILE* fp, const char* format, ...);

#define printf(...) _fprintf_w32(stdout, __VA_ARGS__)

// For errors
#define fprintf(...) _fprintf_w32(__VA_ARGS__)


#endif // LOG_WIN_H
