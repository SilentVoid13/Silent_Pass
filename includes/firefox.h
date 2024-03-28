#ifndef FIREFOX_H
#define FIREFOX_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cJSON.h>

int dump_firefox(const char *output_file, const char *master_password);
int get_firefox_creds(char *profile_path, char *logins_path, const char *output_file, const char *master_password); 
int get_profile(char* profiles_ini_path, char* profile);
int load_firefox_paths(char *firefox_path, char *profiles_ini_path);
int decrypt_firefox_cipher(char *ciphered, char **plaintext);
int nss_authenticate(char *profile_path, void *key_slot, const char *master_password);
void free_pk11_nss(void *key_slot);

#endif // FIREFOX_H
