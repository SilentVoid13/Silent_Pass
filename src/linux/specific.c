#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "main.h"

int dump_specific(struct arguments *args) {
	puts("[*] Starting OS Specific dump...");
	puts("[-] Linux does not have any OS Specific browsers");

	return 1;
}
