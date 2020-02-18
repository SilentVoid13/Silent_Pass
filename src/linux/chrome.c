#include "chrome.h"
#include "chrome_linux.h"
#include "main.h"

#include "log.h"
#include "functions.h"

//const SecretSchema * get_chrome_like_schema (void) G_GNUC_CONST;
#define CHROME_LIKE_SCHEMA  get_chrome_like_schema ()

/**
 * Get the chrome libsecret schema
 *
 * @return
 */
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
		log_error("secret_password_lookup_sync() failure");
		g_error_free(error);
		return -1;
	}
	else if (password == NULL) {
		log_error("secret_password_lookup_sync() failure");
		return -1;
	}
	else {
		*masterkey = (char *)malloc(strlen(password)+1); 
		if(*masterkey == 0) {
			log_error("malloc() failure");
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
int aes_decrypt(EVP_CIPHER_CTX *ctx, char *cipher_password, int len_cipher_password, char **plaintext_password, char *iv, unsigned char *output_key) {
	int len;
	int plaintext_len;
	
	// We don't need the 3 first bytes (V10 or V11)
	char *good_cipher_password = &cipher_password[3];
	int len_good_cipher = len_cipher_password - 3;

	// The ciphertext is always greater or equal to the length of the plaintext
	*plaintext_password = (unsigned char *)malloc(len_good_cipher);
	if(*plaintext_password == 0) {
		log_error("malloc() failure");
		free(*plaintext_password);
		return -1;
	}

	log_verbose("Cipher text length: %d\n", len_good_cipher);

	if(!(ctx = EVP_CIPHER_CTX_new())) {
		log_error("EVP_CIPHER_CTX_new() failure");
		return -1;
	}
	if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, output_key, iv)) {
		log_error("EVP_DecryptInit_ex() failure");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	
	// Disabling padding is actually useless we already have a valid ciphertext length
	//EVP_CIPHER_CTX_set_padding(ctx, 0);

	if(1 != EVP_DecryptUpdate(ctx, *plaintext_password, &len, good_cipher_password, len_good_cipher)) {
		log_error("EVP_DecryptUpdate() failure");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	plaintext_len = len;

	if(1 != EVP_DecryptFinal_ex(ctx, *plaintext_password+len, &len)) {
		log_error("EVP_DecryptFinal_ex() failure");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	plaintext_len += len;
	(*plaintext_password)[plaintext_len] = '\0';
	EVP_CIPHER_CTX_free(ctx);

	return 1;
}

/**
 * Get the masterkey for the storage system in use
 *
 * @return 1 on success, -1 on failure
 */
int get_masterkey(char *login_data_path, char **masterkey) {
	if(get_gnome_masterkey(login_data_path, masterkey) == -1) {
		log_error("get_gnome_masterkey() failure");
		return -1;
	}

	return 1;
}

/** 
 * Decrypt the GNOME cipher password
 *
 * @return 1 on success, -1 on failure
 */
int decrypt_gnome_cipher(char *cipher_password, int len_cipher_password, char **plaintext_password, char *masterkey) {
	// TODO: No need to recalculate the key every time. 
	// 2 - We get the PBKDF2 key 
	unsigned char output_key[KEY_LENGTH];
	char *salt = "saltysalt";
	size_t masterkey_len = strlen(masterkey);
	size_t salt_len = strlen(salt);
	if(PKCS5_PBKDF2_HMAC(masterkey, masterkey_len, salt, salt_len, 1, EVP_sha1(), KEY_LENGTH, output_key) == 0) {
		log_error("PKCS5_PBKDF2_HMAC() failure");
		return -1;
	}

	log_verbose("PBKDF2 key: ");
	if(verbose) {
        for (int i = 0; i < KEY_LENGTH; i++) {
            printf("%02x", output_key[i]);
        }
        printf("\n\n");
    }

	EVP_CIPHER_CTX *ctx = NULL;
	char *iv = "                ";

	if(aes_decrypt(ctx, cipher_password, len_cipher_password, plaintext_password, iv, output_key) == -1) {
		log_error("aes_decrypt() failure");
		return -1;
	}

	return 1;
}

/**
 * Main function to decrypt chrome ciphered data
 *
 * @return 1 on success, -1 on failure
 */
int decrypt_chrome_cipher(char *cipher_password, int len_cipher_password, char **plaintext_password, char *masterkey) {
	// TODO: check for GNOME / KWallet implementation
	decrypt_gnome_cipher(cipher_password, len_cipher_password, plaintext_password, masterkey);

	return 1;
}

/**
 * Load linux chrome paths
 *
 * @return 1 on success, -1 on failure
 */
int load_chrome_paths(char *chrome_path, char *chrome_login_data_path, char *chromium_path, char *chromium_login_data_path) {
	char *home = getenv("HOME");
	snprintf(chrome_path, MAX_PATH_SIZE, "%s/.config/google-chrome/Default", home);
	snprintf(chrome_login_data_path, MAX_PATH_SIZE, "%s/Login Data", chrome_path);
	snprintf(chromium_path, MAX_PATH_SIZE, "%s/.config/chromium/Default", home);
	snprintf(chromium_login_data_path, MAX_PATH_SIZE, "%s/Login Data", chromium_path);
	//snprintf(brave_path, MAX_PATH_SIZE, "%s/.config/BraveSoftware/Brave-Browser/Default", home);
	//snprintf(brave_login_data_path, "%s/Login Data", brave_path);
	
	return 1;
}
