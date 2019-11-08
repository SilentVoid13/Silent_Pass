

#define MAX_PATH 300
#define TRUE PR_TRUE

#ifdef LINUX

#endif // LINUX

int get_profile(char* profiles_ini_path, char* profile);
int decrypt_cipher(char *ciphered, char **plaintext);
int dump_firefox(struct arguments *args);
