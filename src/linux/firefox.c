#include "firefox_linux.h"
#include "firefox.h"
#include "main.h"

#include "log.h"

/**
 * Get the Linux Firefox profile
 *
 * @return 1 on success, -1 on failure 
 */
int get_profile(char* profiles_ini_path, char* profile) {
	dictionary* ini;
	// We parse the ini file
	ini = iniparser_load(profiles_ini_path);

	if(ini == NULL) {
		log_error("Cannot parse file: %s", profiles_ini_path);
		return -1;
	}
	
	const char* s = iniparser_getstring(ini, "Profile0:Path", NULL);
	size_t length = strlen(s);
	strncpy(profile, s, length+1);

	// Free all memory associated to the dictionary
	iniparser_freedict(ini);
	return 1;
}

/**
 * Load linux Firefox paths
 *
 * @return 1 on success, -1 on failure 
 */
int load_firefox_paths(char *firefox_path, char *profiles_ini_path) {
	char *home = getenv("HOME");
	snprintf(firefox_path, MAX_PATH_SIZE, "%s/.mozilla/firefox", home);
	snprintf(profiles_ini_path, MAX_PATH_SIZE, "%s/profiles.ini", firefox_path);

	return 1;
}

/**
 * Decrypt the Firefox ciphered password
 *
 * @return 1 on success, -1 on failure 
 */
int decrypt_firefox_cipher(char *ciphered, char **plaintext) {
	SECItem *request;
	SECItem *response;
	unsigned int len = strlen(ciphered);

	response = SECITEM_AllocItem(NULL, NULL, 0);
	request = NSSBase64_DecodeBuffer(NULL, NULL, ciphered, len);
	PK11SDR_Decrypt(request, response, NULL);

	*plaintext = malloc(response->len + 1);
	if(*plaintext == 0) {
		log_error("malloc() failure");
		free(*plaintext);
		return -1;
	}
	strncpy(*plaintext, (const char * restrict)response->data, response->len);
	(*plaintext)[response->len] = '\0';

	SECITEM_FreeItem(request, TRUE);
	SECITEM_FreeItem(response, TRUE); 

	return 1;
}

/**
 * Authenticate via NSS to be able to query passwords
 *
 * @return 1 on success, -1 on failure
 */
int nss_authenticate(char *profile_path, void *key_slot, const char *master_password) {
	if(NSS_Init(profile_path) != SECSuccess) {
		log_error("NSS Initialisation failed");
		fflush(stderr);
		return -1;
	}

	// We get the key[3-4].db file
	if((key_slot = PK11_GetInternalKeySlot()) == NULL) {
		log_error("PK11_GetInternalKeySlot() failed");
		fflush(stderr);
		return -1;
	}

	if(master_password != NULL) {
		if(PK11_CheckUserPassword(key_slot, master_password) != SECSuccess) {
			log_error("PK11_CheckUserPassword() failed, Wrong master password");
			fflush(stderr);
			return -1;
		}
	} else {
		// We check if we can open it with no password
		if(PK11_CheckUserPassword(key_slot, "") != SECSuccess) {
			log_error("PK11_CheckUserPassword() failed, Try with -m <password> option");
			fflush(stderr);
			return -1;
		}
	}

	if(PK11_Authenticate(key_slot, TRUE, NULL) != SECSuccess) {
		log_error("PK11_Authenticate() failed");
		fflush(stderr);
		return -1;
	}

	return 1;
}

/**
 * Free PK11 / NSS Functions
 *
 * @return 
 */
void free_pk11_nss(void *key_slot) {
	PK11_FreeSlot((PK11SlotInfo *) &key_slot);
	NSS_Shutdown();
}
