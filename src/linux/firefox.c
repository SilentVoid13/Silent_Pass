#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nss.h"
#include "nssb64.h"
#include "pk11pub.h"
#include "pk11sdr.h"
#include "iniparser.h"

#include "args.h"
#include "main.h"
#include "firefox.h"
#include "firefox_linux.h"

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

int load_firefox_paths(char *firefox_path, char *profiles_ini_path) {
	char *home = getenv("HOME");
	snprintf(firefox_path, MAX_PATH, "%s/.mozilla/firefox", home);
	snprintf(profiles_ini_path, MAX_PATH, "%s/profiles.ini", firefox_path);

	return 1;
}

int decrypt_firefox_cipher(char *ciphered, char **plaintext) {
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

void free_pk11_nss(void *key_slot) {
	PK11_FreeSlot(&key_slot);
	NSS_Shutdown();
}
