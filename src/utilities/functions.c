#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/**
 * Get the base64 plaintext len with a base64 string
 *
 * @return plaintext size
 */
size_t calc_base64_length(const char* b64input) { //Calculates the length of a decoded string
	size_t len = strlen(b64input),padding = 0;

	if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
		padding = 2;
	else if (b64input[len-1] == '=') //last char is =
		padding = 1;

	return (len*3)/4 - padding;
}
