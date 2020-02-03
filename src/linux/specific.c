#include "specific_linux.h"
#include "specific.h"
#include "main.h"

/**
 * Specific functions wrapper that sets up everything we need
 *
 * @return 1 on success, -1 on failure 
 */
int dump_specific(int verbose, const char *output_file) {
	puts("[*] Starting OS Specific dump...");
	puts("[-] Currently no OS Specific browsers implemented for Linux");

	return 1;
}
