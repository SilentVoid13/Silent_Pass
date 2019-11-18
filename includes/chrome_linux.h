#ifndef CHROME_LINUX_H
#define CHROME_LINUX_H

const SecretSchema *get_chrome_like_schema(void);
int get_gnome_masterkey(char *login_data_path, char **masterkey); 
int aes_decrypt(EVP_CIPHER_CTX *ctx, char *cipher_password, int len_cipher_password, char **plaintext_password, char *iv, unsigned char *output_key);
int decrypt_gnome_cipher(char *cipher_password, int len_cipher_password, char **plaintext_password, char *masterkey); 

#endif // CHROME_LINUX_H
