#ifndef ARGS_H
#define ARGS_H

struct arguments {
	enum {
		ALL_MODE,
		FIREFOX_MODE,
		CHROME_MODE
	} mode;
	int verbose;
	char *output_file;
	char *master_password;
}; 

#endif // ARGS_H
