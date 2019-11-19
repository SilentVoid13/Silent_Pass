#include <stdio.h>
#include <stdlib.h>

#include "main.h"

/**
 * Specific functions wrapper that sets up everything we need
 *
 * @return 1 on success, -1 on failure 
 */
int dump_specific(int verbose, const char *output_file) {
	puts("[*] Starting OS Specific dump...");
	puts("[-] Linux does not have any OS Specific browsers");

	return 1;
}
