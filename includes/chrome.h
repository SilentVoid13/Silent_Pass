#include "sqlite3.h"

#ifndef CHROME_H
#define CHROME_H

#define KEY_LENGTH 16
#define SECRET_QUERY_SIZE 30

int dump_chrome(int verbose, char *output_file);
int prepare_sqlite_statement(char *login_data_path, sqlite3 **db, sqlite3_stmt **stmt);
int fetch_sqlite_data(char **website, char **username, char **cipher_password, int *len_cipher_password, sqlite3_stmt **stmt);
int get_chrome_creds(char *login_data_path, char *output); 
int get_masterkey(char *login_data_path, char **masterkey);
int decrypt_chrome_cipher(char *cipher_password, int len_cipher_password, char **plaintext_password, char *masterkey);
int load_chrome_paths(char *chrome_path, char *chrome_login_data_path, char *chromium_path, char *chromium_login_data_path);

#endif // CHROME_H
