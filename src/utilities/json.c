#include <stdio.h>
#include <stdlib.h>
#include "json.h"

int parse_json(char *path, char **json) {
	FILE *f = fopen(path, "rb"); 
	if(f == NULL) {
		fprintf(stderr, "fopen() failure\n");
		return -1;
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	printf("[*] Size of file: %ld bytes\n\n", fsize);

	*json = malloc(fsize + 1);
	if(*json == 0) {
		fprintf(stderr, "malloc() failure\n");
		free(*json);
		return -1;
	}
	fread(*json, 1, fsize, f);
	fclose(f);
	
	return 1;
}