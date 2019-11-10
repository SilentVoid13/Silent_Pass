#include <stdio.h>
#include <stdlib.h>
#include <argp.h>

#include "args.h"
#include "main.h"
#include "firefox.h"
#include "chrome.h"

int main(int argc, char** argv) {
	struct arguments arguments;

	// Default value
	arguments.mode = FIREFOX_MODE;
	arguments.verbose = 0;
	arguments.output_file = NULL;
	arguments.master_password = NULL;

	if(parse_arguments(argc, argv, &arguments) != 1) {
		fprintf(stderr, "parse_arguments() failure\n");
	}

	printf("Output_file: %s\n", arguments.output_file);
	printf("Master password: %s\n", arguments.master_password);

	if(arguments.mode == ALL_MODE) {
		puts("[*] All mode");
		if(dump_firefox(&arguments) == -1) {
			fprintf(stderr, "dump_firefox() failure\n");
		}
		if(dump_chrome(&arguments) == -1) {
			fprintf(stderr, "dump_chrome() failure\n");
		}
	}
	else if (arguments.mode == CHROME_MODE) {
		puts("[*] Chrome mode");
		if(dump_chrome(&arguments) == -1) {
			fprintf(stderr, "dump_chrome() failure\n");
		}

	}
	else if (arguments.mode == FIREFOX_MODE) {
		puts("[*] Firefox mode");
		if(dump_firefox(&arguments) == -1) {
			fprintf(stderr, "dump_firefox() failure\n");
		}
	}

	return 0;
}
