#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nss.h"
#include "nssb64.h"
#include "pk11pub.h"
#include "pk11sdr.h"

// To parse the profiles.ini file
#include "iniparser.h"
// To parse the JSON
#include "lib/cJSON.h"

#include "main.h"
#include "firefox.h"

// TODO:
// - Choose the profile we want and not the default one (Profile0)
// - Enable Master Password

int get_profile(char* profiles_ini_path, char* profile) {
	dictionary* ini;
	// We parse the ini file
	ini = iniparser_load(profiles_ini_path);

	if(ini == NULL) {
		fprintf(stderr, "Cannot parse file: %s", profiles_ini_path);
		return -1;
	}
	
	const char* s = iniparser_getstring(ini, "Profile0:Path", NULL);
	int length = strlen(s);
	strncpy(profile, s, length+1);

	// Free all memory associated to the dictionary
	iniparser_freedict(ini);
	return 1;
}

int decrypt_cipher(char *ciphered, char **plaintext) {
	SECItem *request;
	SECItem *response;
	unsigned int len = strlen(ciphered);

	response = SECITEM_AllocItem(NULL, NULL, 0);
	request = NSSBase64_DecodeBuffer(NULL, NULL, ciphered, len);
	PK11SDR_Decrypt(request, response, NULL);

	*plaintext = malloc(response->len + 1);
	if(*plaintext == 0) {
		fprintf(stderr, "malloc() failure\n");
		free(*plaintext);
		return -1;
	}
	strncpy(*plaintext, (const char * restrict)response->data, response->len);
	(*plaintext)[response->len] = '\0';

	SECITEM_FreeItem(request, TRUE);
	SECITEM_FreeItem(response, TRUE); 

	return 1;
}

int nss_authenticate(char *profile_path, void *key_slot, char *master_password) {
	// We initialise the NSS 
	if(NSS_Init(profile_path) != SECSuccess) {
		fprintf(stderr, "NSS Initialisation failed\n");
		fflush(stderr);
		return -1;
	}

	// We get the key[3-4].db file
	if((key_slot = PK11_GetInternalKeySlot()) == NULL) {
		fprintf(stderr, "PK11_GetInternalKeySlot() failed\n");
		fflush(stderr);
		return -1;
	}

	if(master_password != NULL) {
		if(PK11_CheckUserPassword(key_slot, master_password) != SECSuccess) {
			fprintf(stderr, "PK11_CheckUserPassword() failed, Wrong master password\n");
			fflush(stderr);
			return -1;
		}
	} else {
		// We check if we can open it with no password
		if(PK11_CheckUserPassword(key_slot, "") != SECSuccess) {
			fprintf(stderr, "PK11_CheckUserPassword() failed, Try with -m <password> option\n");
			fflush(stderr);
			return -1;
		}
	}

	if(PK11_Authenticate(key_slot, TRUE, NULL) != SECSuccess) {
		fprintf(stderr, "PK11_Authenticate() failed\n");
		fflush(stderr);
		return -1;
	}

	return 1;
}

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

int get_firefox_creds(char *profile_path, char *logins_path, char *output_file, char *master_password) {
	void* key_slot; 
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

		if (
				cJSON_IsString(cipher_username) && 
				cJSON_IsString(cipher_password) && 
				cJSON_IsString(hostname)
		) {

			decrypt_cipher(cipher_username->valuestring, &username);
			decrypt_cipher(cipher_password->valuestring, &password);

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

	// We free the memory of everything.	
	if(output_file != NULL) {
		fclose(output);
	}
	free(json);
	PK11_FreeSlot(key_slot);
	NSS_Shutdown();

	return 1;
}

int dump_firefox(struct arguments *args) {
	char* path_separator = "/";

	int result = 0;
	char firefox_path[MAX_PATH];
	char profiles_ini_path[MAX_PATH];
	char profile[MAX_PATH];
	char profile_path[MAX_PATH];
	//char signons_path[MAX_PATH];
	char logins_path[MAX_PATH];

	// Only for Windows
	//load_firefox_libs();
	
	// For Linux
	char *home = getenv("HOME");
	snprintf(firefox_path, MAX_PATH, "%s/.mozilla/firefox", home);
	snprintf(profiles_ini_path, MAX_PATH, "%s/profiles.ini", firefox_path);
	// End for linux

	if(get_profile(profiles_ini_path, profile) == -1) {
		fprintf(stderr, "get_profile() failure");
		return -1;
	}
	snprintf(profile_path, MAX_PATH, "%s%s%s", firefox_path, path_separator, profile);
	snprintf(logins_path, MAX_PATH, "%s/logins.json", profile_path);
	//snprintf(signons_path, MAX_PATH, "%s/signons.sqlite", profile_path);
	
	if(access(logins_path, F_OK) != -1) {
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
