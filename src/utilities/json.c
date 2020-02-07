#include "json.h"
#include "log.h"

/**
 * Get a JSON file data
 *
 * @return 1 on success, -1 on failure 
 */
int parse_json(char *path, char **json) {
	FILE *f = fopen(path, "rb"); 
	if(f == NULL) {
		log_error("fopen() failure");
		return -1;
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	log_info("Size of file: %ld bytes\n", fsize);

	*json = malloc(fsize + 1);
	if(*json == 0) {
		log_error("malloc() failure");
		free(*json);
		fclose(f);
		return -1;
	}
	fread(*json, 1, fsize, f);
	fclose(f);
	
	return 1;
}
