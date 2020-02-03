#include "functions.h"

/**
 * Safe wrapper to use strncpy()
 *
 * @return
 */
void safe_strcpy(char *dst, char *src, int len) {
	strncpy(dst, src, len);
	dst[len] = '\0';
}
