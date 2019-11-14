#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"

/**
 * Safe wrapper to use strncpy()
 *
 */
void safe_strcpy(char *dst, char *src, int len) {
	strncpy(dst, src, len);
	dst[len] = '\0';
}
