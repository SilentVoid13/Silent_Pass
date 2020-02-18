#include "specific_win.h"
#include "specific.h"
#include "main.h"

#include "explorer.h"
#include "log.h"

/**
 * Specific functions wrapper that sets up everything we need
 *
 * @return 1 on success, -1 on failure 
 */
int dump_specific(const char *output_file) {
	if(dump_explorer(output_file) == -1) {
		log_error("dump_explorer() failure");
	}

	return 1;
}
