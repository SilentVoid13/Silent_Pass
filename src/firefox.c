#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cJSON.h"
#include "args.h"
#include "main.h"
#include "firefox.h"
#include "json.h"

// TODO:
// - Choose the profile we want and not the default one (Profile0)

int get_firefox_creds(char *profile_path, char *logins_path, char *output_file, char *master_password) {
	void* key_slot = NULL; 
	if((nss_authenticate(profile_path, key_slot, master_password)) == -1) {
		fprintf(stderr, "nss_authenticate failure()\n");
		return -1;
	}

	char *json;
	if(parse_json(logins_path, &json) == -1) {
		fprintf(stderr, "parse_json failed()\n");
		return -1;
	}

	cJSON* values = cJSON_Parse(json);
	if(values == NULL) {
		fprintf(stderr, "cJSON_Parse() failed\n");
		fflush(stderr);
		return -1;
	}

	cJSON *logins_array = NULL;
	cJSON *logins = NULL;
	cJSON *hostname = NULL; 	
	cJSON *cipher_username = NULL;
	cJSON *cipher_password = NULL;

	char *username, *password;

	logins_array = cJSON_GetObjectItemCaseSensitive(values, "logins");	

	FILE *output;
	if(output_file != NULL) {
		output = fopen(output_file, "wb");
	}

	cJSON_ArrayForEach(logins, logins_array) {
		hostname = cJSON_GetObjectItemCaseSensitive(logins, "hostname");	
		cipher_username = cJSON_GetObjectItemCaseSensitive(logins, "encryptedUsername");	
		cipher_password = cJSON_GetObjectItemCaseSensitive(logins, "encryptedPassword");	

		if (cJSON_IsString(cipher_username) && cJSON_IsString(cipher_password) && cJSON_IsString(hostname)) {
			if(strlen(hostname) > 0) {
				decrypt_firefox_cipher(cipher_username->valuestring, &username);
				decrypt_firefox_cipher(cipher_password->valuestring, &password);

				printf("[*] Website: %s\n[+] Username: %s\n[+] Password: %s\n\n", 
					hostname->valuestring,
					username,
					password);

				if(output_file != NULL) {
					fprintf(output, "\"%s\",\"%s\",\"%s\"\n", 
						hostname->valuestring,
						username,
						password);
				}

				free(username);
				free(password);
			}
		}
	}

	// We free the memory of everything.	
	if(output_file != NULL) {
		fclose(output);
	}
	free(json);
	free_pk11_nss(key_slot);
	
	return 1;
}

int dump_firefox(struct arguments *args) {
	printf("Master password: %s\n", args->master_password);
	int result = 0;
	char firefox_path[MAX_PATH];
	char profiles_ini_path[MAX_PATH];
	char profile[MAX_PATH];
	char profile_path[MAX_PATH];
	char logins_path[MAX_PATH];

	load_firefox_paths(firefox_path, profiles_ini_path);

	if(get_profile(profiles_ini_path, profile) == -1) {
		fprintf(stderr, "get_profile() failure");
		return -1;
	}

	snprintf(profile_path, MAX_PATH, "%s%s%s", firefox_path, "/", profile);
	snprintf(logins_path, MAX_PATH, "%s/logins.json", profile_path);
	
	// TODO: S_OK / F_OK
	if(access(logins_path, 0) != -1) {
		printf("[*] Starting Firefox credentials dump\n\n");
		result = get_firefox_creds(profile_path, logins_path, args->output_file, args->master_password);
	}

	if(result == -1) {
		fprintf(stderr, "[-] An error occured\n");
		return -1;
	}
	else if(result == 0) {
		fprintf(stderr, "[-] Couldn't find any Firefox installation\n");
		return -1;
	}
	return 1;
}
