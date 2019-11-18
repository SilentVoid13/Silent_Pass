#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <wincrypt.h>
#include <shlobj.h>
#include <prtypes.h> 

#include "main.h"
#include "explorer.h"

int dump_specific(int verbose, const char *output_file) {
	if(dump_explorer(verbose, output_file) == -1) {
		fprintf(stderr, "dump_explorer() failure\n");
	}

	return 1;
}
