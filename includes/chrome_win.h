#ifndef CHROME_WIN_H
#define CHROME_WIN_H

int dpapi_decrypt(char *cipher_password, int len_cipher_password, char **plaintext_password);
int aead_decrypt(char *cipher_password, int len_cipher_password, char *key, char *iv, int len_iv, char **plaintext_password);
int get_json_base64_key(char **b64_key);
size_t calcDecodeLength(const char* b64input); 
int get_base64_dpapi_key(char **key, int *key_len);

#endif // CHROME_WIN_H
