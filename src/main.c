#include "main.h"
#include "firefox.h"
#include "chrome.h"

#include "log.h"

struct arg_lit *verbose, *help, *version, *all, *firefox, *chrome, *specific, *filezilla;
struct arg_str *master_firefox, *master_filezilla;
struct arg_file *output;
struct arg_end *end;

/** 
 * Main function that parse the args and calls the different sub_functions
 *
 * @return 0
 */
int main(int argc, char** argv) {
	void *argtable[] = {
		help     = arg_litn("h", "help", 0, 1, "Display this help and exit"),
		version  = arg_litn("V", "version", 0, 1, "Display version info and exit"),
		verbose  = arg_litn("v", "verbose", 0, 1, "Verbose output"),
		all      = arg_litn("a", "all", 0, 1, "Harvest all softwares credentials"),
		firefox  = arg_litn("f", "firefox", 0, 1, "Harvest Firefox credentials"),
		chrome   = arg_litn("c", "chrome", 0, 1, "Harvest Chrome-like credentials"),
		specific = arg_litn("s", "specific", 0, 1, "Harvest OS Specific browsers credentials (e.g: IE for Windows)"),
		filezilla = arg_litn("F", "filezilla", 0, 1, "Harvest FileZilla credentials"),
		master_firefox   = arg_strn(NULL, "master-firefox", "password", 0, 1, "Master password to decrypt passwords for Firefox"),
		master_filezilla = arg_strn(NULL, "master-filezilla", "password", 0, 1, "Master password to decrypt passwords for FileZilla"),
		output   = arg_filen("o", "output", "filename", 0, 1, "Output file"),
		end      = arg_end(20),	
	};

	char *progname = "Silent_Pass";
	char *progversion = "0.2";
	char *progdate = "2020";
	char *author_name = "SilentVoid";

	output->filename[0] = NULL;
	master_firefox->sval[0] = NULL;
	master_filezilla->sval[0] = NULL;
	
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

	// We remove the file if it already exists to write to it (with append mode)
	if(output->filename[0] != NULL) {
		remove(output->filename[0]);
	}

	if(all->count > 0) {
		log_info("All mode\n");
		if(dump_firefox(verbose->count, output->filename[0], master_firefox->sval[0]) == -1) {
			log_error("dump_firefox() failure");
		}
		if(dump_chrome(verbose->count, output->filename[0]) == -1) {
			log_error("dump_chrome() failure");
		}
		if(dump_specific(verbose->count, output->filename[0]) == -1) {
			log_error("dump_specific() failure");
		}
		if(dump_filezilla(verbose->count, output->filename[0], master_filezilla->sval[0]) == -1) {
			log_error("dump_filezilla() failure");
		}
	}
	else if (chrome->count > 0) {
		log_info("Chrome mode\n");
		if(dump_chrome(verbose->count, output->filename[0]) == -1) {
			log_error("dump_chrome() failure");
		}

	}
	else if (firefox->count > 0) {
		log_info("Firefox mode\n");
		if(dump_firefox(verbose->count, output->filename[0], master_firefox->sval[0]) == -1) {
			log_error("dump_firefox() failure");
		}
	}
	else if (specific->count > 0) {
		log_info("Specific mode\n");
		if(dump_specific(verbose->count, output->filename[0]) == -1) {
			log_error("dump_specific() failure");
		}
	}
	else if (filezilla->count > 0) {
		log_info("FileZilla mode\n");
		if(dump_filezilla(verbose->count, output->filename[0], master_filezilla->sval[0]) == -1) {
			log_error("dump_filezilla() failure");
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
