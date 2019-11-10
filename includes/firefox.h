#ifndef FIREFOX_H
#define FIREFOX_H

#define MAX_PATH 300

int dump_firefox(struct arguments *args);
int get_firefox_creds(char *profile_path, char *logins_path, char *output_file, char *master_password); 
int get_profile(char* profiles_ini_path, char* profile);
int load_firefox_paths(char *firefox_path, char *profiles_ini_path);
int decrypt_cipher(char *ciphered, char **plaintext);
int nss_authenticate(char *profile_path, void *key_slot, char *master_password);
void free_pk11_nss(void *key_slot);

#endif // FIREFOX_H
