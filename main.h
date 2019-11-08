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
