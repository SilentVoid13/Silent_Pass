#ifndef EXPLORER_H
#define EXPLORER_H

#include <windows.h>
#include <wincrypt.h>
#include <shlobj.h>
#include <prtypes.h> 
#include <wchar.h>

#include <urlhist.h>

#define URL_MAX_SIZE 150
#define MAX_HASH_SIZE 1024
#define MAX_URL_HISTORY 50
#define CIPHER_SIZE_MAX 5000

typedef HANDLE HVAULT;

enum VAULT_ELEMENT_TYPE {
	ElementType_Boolean = 0,
	ElementType_Short = 1,
	ElementType_UnsignedShort = 2,
	ElementType_Integer = 3,
	ElementType_UnsignedInteger = 4,
	ElementType_Double = 5,
	ElementType_Guid = 6,
	ElementType_String = 7,
	ElementType_ByteArray = 8,
	ElementType_TimeStamp = 9,
	ElementType_ProtectedArray = 0xA,
	ElementType_Attribute = 0xB,
	ElementType_Sid = 0xC,
	ElementType_Last = 0xD,
	ElementType_Undefined = 0xFFFFFFFF
};

typedef struct _VAULT_BYTE_BUFFER {
	DWORD Length;
	PBYTE Value;
} VAULT_BYTE_BUFFER, *PVAULT_BYTE_BUFFER;

typedef struct _VAULT_ITEM_DATA {
	DWORD SchemaElementId;
	DWORD unk0;
	enum VAULT_ELEMENT_TYPE Type;
	DWORD unk1;
	union {
		BOOL Boolean;
		SHORT Short;
		WORD UnsignedShort;
		LONG Int;
		ULONG UnsignedInt;
		DOUBLE Double;
		GUID Guid;
		LPWSTR String;
		VAULT_BYTE_BUFFER ByteArray;
		VAULT_BYTE_BUFFER ProtectedArray;
		DWORD Attribute;
		DWORD Sid;
	} data;
} VAULT_ITEM_DATA, *PVAULT_ITEM_DATA;

typedef struct _VAULT_ITEM_8 {
	GUID SchemaId;
	PWSTR FriendlyName;
	PVAULT_ITEM_DATA Resource;
	PVAULT_ITEM_DATA Identity;
	PVAULT_ITEM_DATA Authenticator;
	PVAULT_ITEM_DATA PackageSid;
	FILETIME LastWritten;
	DWORD Flags;
	DWORD cbProperties;
	PVAULT_ITEM_DATA Properties;
} VAULT_ITEM, *PVAULT_ITEM;

typedef struct _VAULT_ITEM_7 {
	GUID SchemaId;
	PWSTR FriendlyName;
	PVAULT_ITEM_DATA Resource;
	PVAULT_ITEM_DATA Identity;
	PVAULT_ITEM_DATA Authenticator;
	FILETIME LastWritten;
	DWORD Flags;
	DWORD cbProperties;
	PVAULT_ITEM_DATA Properties;
} VAULT_ITEM_7, *PVAULT_ITEM_7;

typedef DWORD(WINAPI *VaultEnumerateVaults_t)(DWORD flags, PDWORD count, GUID **guids);
typedef DWORD(WINAPI *VaultEnumerateItems_t)(HVAULT handle, DWORD flags, PDWORD count, PVOID *items);
typedef DWORD(WINAPI *VaultOpenVault_t)(GUID *id, DWORD flags, HVAULT *handle);
typedef DWORD(WINAPI *VaultCloseVault_t)(HVAULT handle);
typedef DWORD(WINAPI *VaultFree_t)(PVOID mem);
//typedef unsigned int (__stdcall *VaultGetItem_t)(void *VaultHandle, GUID *pSchemaId, vault_entry_s *pResource, vault_entry_s *pIdentity, vault_entry_s *pPackageSid, HWND *hwndOwner, unsigned int dwFlags, vault_cred_s **ppItem);
typedef DWORD(WINAPI * VaultGetItem_t) (HANDLE vault, LPGUID SchemaId, PVAULT_ITEM_DATA Resource, PVAULT_ITEM_DATA Identity, PVAULT_ITEM_DATA PackageSid, HWND hWnd, DWORD Flags, PVAULT_ITEM * pItem);

typedef struct {
	wchar_t utf_url[URL_MAX_SIZE];
	char url[URL_MAX_SIZE];
} IEUrl;

int load_ie_vault_libs();
int get_ie_vault_creds(const char *output_file);
int dump_explorer(const char *output_file);
int dpapi_decrypt_entropy(char *cipher_data, int len_cipher_data, wchar_t *entropy_password, int len_entropy_password, char **plaintext_data);
int get_registry_history(IEUrl *urls, int *n_urls, int nHowMany);
void get_url_hash(wchar_t *wstrURL, char *strHash, int dwSize); 
int get_ie_history();
void add_known_websites(IEUrl *urls, int *n_urls);
int get_ie_registry_creds(const char *output_file);
int print_decrypted_data(char *decrypted_data, char *url, const char *output_file);



#endif // EXPLORER_H
