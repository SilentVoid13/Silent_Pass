#ifndef CHROME_WIN_H
#define CHROME_WIN_H

#include <winsock2.h>
#include <windows.h>
#include <wincrypt.h>
#include <shlobj.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/kdf.h>
#include <openssl/bio.h>

#include "lib/cJSON/cJSON.h"

int dpapi_decrypt(char *cipher_password, int len_cipher_password, char **plaintext_password);
int aead_decrypt(char *cipher_password, int len_cipher_password, char *key, char *iv, int len_iv, char **plaintext_password);
int get_json_base64_key(char **b64_key);
int get_base64_dpapi_key(char **key, int *key_len);

#endif // CHROME_WIN_H
