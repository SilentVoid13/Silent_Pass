#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <wincrypt.h>
#include <shlobj.h>
#include <prtypes.h> 

#include "args.h"
#include "main.h"
#include "explorer.h"

int dump_specific(struct arguments *args) {
	puts("[*] Starting OS Specific dump...");

	if(dump_explorer(args) == -1) {
		fprintf(stderr, "dump_explorer() failure\n");
	}

	return 1;
}
