#include "specific_linux.h"
#include "specific.h"
#include "main.h"

#include "log.h"

/**
 * Specific functions wrapper that sets up everything we need
 *
 * @return 1 on success, -1 on failure 
 */
int dump_specific(const char *output_file) {
    // For Warning
    (void) output_file;

	log_info("Starting OS Specific dump ...\n");
	log_error("Currently no OS Specific browsers implemented for Linux\n");

	return 1;
}
