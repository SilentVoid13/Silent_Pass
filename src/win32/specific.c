#include "specific_win.h"
#include "specific.h"
#include "main.h"

#include "explorer.h"

/**
 * Specific functions wrapper that sets up everything we need
 *
 * @return 1 on success, -1 on failure 
 */
int dump_specific(int verbose, const char *output_file) {
	if(dump_explorer(verbose, output_file) == -1) {
		log_error("dump_explorer() failure");
	}

	return 1;
}
