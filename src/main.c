#include <stdio.h>
#include <stdlib.h>

#include "argtable3.h"
#include "main.h"
#include "firefox.h"
#include "chrome.h"


struct arg_lit *verbose, *help, *version, *all, *firefox, *chrome, *specific;
struct arg_str *master;
struct arg_file *output;
struct arg_end *end;

int main(int argc, char** argv) {
	void *argtable[] = {
		help     = arg_litn("h", "help", 0, 1, "display this help and exit"),
		version  = arg_litn("V", "version", 0, 1, "display version info and exit"),
		verbose  = arg_litn("v", "verbose", 0, 1, "verbose output"),
		all      = arg_litn("a", "all", 0, 1, "Harvest all browsers credentials"),
		firefox  = arg_litn("f", "firefox", 0, 1, "Harvest Firefox credentials"),
		chrome   = arg_litn("c", "chrome", 0, 1, "Harvest Chrome-like credentials"),
		specific = arg_litn("s", "specific", 0, 1, "Harvest OS Specific browsers credentials (No Specific OS browser for Linux for now)"),
		master   = arg_strn("m", "master", "password", 0, 1, "Master password to decrypt passwords (Firefox only for now)"),
		output   = arg_filen("o", "output", "myfile", 0, 1, "Ouput file"),
		end      = arg_end(20),	
	};

	char *progname = "Silent_Pass";
	char *progversion = "0.1";
	char *progdate = "2019";
	char *author_name = "SilentVoid";

	output->filename[0] = NULL;
	master->sval[0] = NULL;
	
	int nerrors;
	nerrors = arg_parse(argc, argv, argtable);

	if(help->count > 0) {
		printf("Usage: %s", progname);
		arg_print_syntax(stdout, argtable, "\n");
		arg_print_glossary(stdout, argtable, "  %-25s %s\n");
		exit(0);
	}
	
	if(version->count > 0) {
		printf("%s %s\n", progname, progversion);
		printf("Copyright (C) %s %s\n", progdate, author_name);
		exit(0);
	}

	if(nerrors > 0) {
		arg_print_errors(stdout, end, progname);
		printf("Try '%s --help' for more informations.\n", progname);
		exit(1);
	}

	if(all->count > 0) {
		puts("[*] All mode");
		if(dump_firefox(verbose->count, output->filename[0], master->sval[0]) == -1) {
			fprintf(stderr, "dump_firefox() failure\n");
		}
		if(dump_chrome(verbose->count, output->filename[0]) == -1) {
			fprintf(stderr, "dump_chrome() failure\n");
		}
		if(dump_specific(verbose->count, output->filename[0]) == -1) {
			fprintf(stderr, "dump_specific() failure\n");
		}
	}
	else if (chrome->count > 0) {
		puts("[*] Chrome mode");
		if(dump_chrome(verbose->count, output->filename[0]) == -1) {
			fprintf(stderr, "dump_chrome() failure\n");
		}

	}
	else if (firefox->count > 0) {
		puts("[*] Firefox mode");
		if(dump_firefox(verbose->count, output->filename[0], master->sval[0]) == -1) {
			fprintf(stderr, "dump_firefox() failure\n");
		}
	}
	else if (specific->count > 0) {
		puts("[*] Specific mode");
		if(dump_specific(verbose->count, output->filename[0]) == -1) {
			fprintf(stderr, "dump_specific() failure\n");
		}
	}
	else {
		printf("Usage: %s", progname);
		arg_print_syntax(stdout, argtable, "\n");
		arg_print_glossary(stdout, argtable, "  %-25s %s\n");
		exit(0);
	}

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 0;
}
