#include "filezilla.h"
#include "main.h"

#include "log.h"
#include "xml.h"
#include "s_base64.h"
#include "s_curve.h"
#include "s_digest.h"
#include "s_pbkdf.h"
#include "s_aead.h"
#include "functions.h"

int parse_sitemanager_xml(const char *output_file, const char *master_password, char *path) {
	xmlDocPtr doc;
	xmlNodePtr cur;

	if(init_xml_file(&doc, &cur, path) == -1) {
		log_error("init_xml_file() failure");
		return -1;
	}

	cur = cur->xmlChildrenNode;
	cur = retrieve_xml_node(cur, "Servers");
	if(cur == NULL) {
		log_error("retrieve_xml_node() failure");
		return -1;
	}

	cur = cur->xmlChildrenNode;
	char *node_name = "Server";
	while(cur != NULL) {
		if(xmlStrcmp(cur->name, (const xmlChar *) node_name) == 0) {
			parse_server_xml(doc, cur->xmlChildrenNode, output_file, master_password);
		}
		cur = cur->next;
	}
    printf("\n");

	xmlFreeDoc(doc);
	xmlCleanupParser();

	return 1;
}

int parse_recentservers_xml(const char *output_file, const char *master_password, char *path) {
	xmlDocPtr doc;
	xmlNodePtr cur;

	if(init_xml_file(&doc, &cur, path) == -1) {
		log_error("init_xml_file() failure");
		return -1;
	}

	cur = cur->xmlChildrenNode;
	cur = retrieve_xml_node(cur, "RecentServers");
	if(cur == NULL) {
		log_error("retrieve_xml_node() failure");
		return -1;
	}
	
	cur = cur->xmlChildrenNode;
	char *node_name = "Server";
	while(cur != NULL) {
		if(xmlStrcmp(cur->name, (const xmlChar *) node_name) == 0) {
			parse_server_xml(doc, cur->xmlChildrenNode, output_file, master_password);
		}
		cur = cur->next;
	}
	printf("\n");

	xmlFreeDoc(doc);
	xmlCleanupParser();

	return 1;
}

int parse_server_xml(xmlDocPtr doc, xmlNodePtr cur, const char *output_file, const char *master_password) {
    // For warning
    (void) master_password;

	xmlChar *key;
	char *host = NULL;
	char *username = NULL;
	unsigned char *cipher_password = NULL;
	unsigned char *plaintext_password = NULL;
	char port[6] = {-1};
	int decryption_result = 0;

	while(cur != NULL) {
		if (cur->type == XML_ELEMENT_NODE) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			size_t key_len;
			if(strcmp(cur->name, "User") == 0) {
                key_len = strlen(key);
				username = malloc(key_len+1);
				if(username == NULL) {
				    free(username);
				    log_error("malloc() failure");
				    return -1;
				}
				safe_strcpy(username, key, key_len);
			}
			else if(strcmp(cur->name, "Pass") == 0) {
			    decryption_result = decrypt_filezilla_password(cur, key, strlen(key), &plaintext_password, master_password);
			}
			else if(strcmp(cur->name, "Host") == 0) {
                key_len = strlen(key);
				host = malloc(key_len+1);
                if(host == NULL) {
                    free(host);
                    log_error("malloc() failure");
                    return -1;
                }
				safe_strcpy(host, key, key_len);
			}
			else if(strcmp(cur->name, "Port") == 0) {
                key_len = strlen(key);
				safe_strcpy(port, key, key_len);
			}
		}
		cur = cur->next;
	}

	// We only add to input file when we have full creds (Maybe change that ?)
	if(output_file != NULL && host != NULL && username != NULL && decryption_result == 1) {
		FILE *output_fd = fopen(output_file, "ab");
		fprintf(output_fd, "\"%s\",\"%s\",\"%s\"\n", 
			host,
			username,
			plaintext_password);
		fclose(output_fd);
	}

	printf("\n");
	if(host != NULL) {
		log_success("Host : %s", host);
		free(host);
	}
	if(port[0] != -1) {
		log_success("Port : %s", port);
	}
	if(username != NULL) {
		log_success("Username: %s", username);
		free(username);
	}
	if(decryption_result == 1) {
		log_success("Password : %s", plaintext_password);
		free(plaintext_password);
	}

	return 1;
}

int decrypt_filezilla_password(xmlNodePtr cur, unsigned char *ciphertext, size_t ciphertext_len, unsigned char **plaintext_password, const char *master_password) {
    xmlChar *encoding;
    encoding = xmlGetProp(cur, "encoding");
    if(encoding == NULL) {
        log_error("Couldn't retrieve XML attribute");
        return -1;
    }

    if(strcmp(encoding, "base64") == 0) {
        if(s_base64_decode(ciphertext, ciphertext_len, plaintext_password, 1) == -1) {
            log_error("base64_decode() failure");
            return -1;
        }
    }
    else if(strcmp(encoding, "crypt") == 0) {
        if(master_password == NULL) {
            log_error("Password is encrypted, try with --master-filezilla option");
            return -1;
        }

        xmlChar *pubkey;
        pubkey = xmlGetProp(cur, "pubkey");
        if(pubkey == NULL) {
            log_error("Couldn't retrieve XML attribute");
            return -1;
        }

        unsigned char *ciphertext_decoded;
        size_t ciphertext_decoded_len;
        if((ciphertext_decoded_len = s_base64_decode(ciphertext, ciphertext_len, &ciphertext_decoded, 1)) == -1u) {
            log_error("base64_decode() failure");
            return -1;
        }

        unsigned char *public_key;
        int public_key_len;
        if((public_key_len = s_base64_decode(pubkey, strlen(pubkey), &public_key, 0)) == -1) {
            free(ciphertext_decoded);
            log_error("base64_decode() failure");
            return -1;
        }

        size_t cipher_password_len = ciphertext_decoded_len - 32 - 32 - 16;

        unsigned char *e_key            =   &(ciphertext_decoded[F_KEY_OFFSET]);
        unsigned char *e_salt           =   &(ciphertext_decoded[F_SALT_OFFSET]);
        unsigned char *cipher_password  =   &(ciphertext_decoded[F_CIPHER_PASSWORD_OFFSET]);
        unsigned char *tag              =   &(ciphertext_decoded[ciphertext_decoded_len-F_TAG_LENGTH]);

        unsigned char *public_key_key  =   &(public_key[F_KEY_OFFSET]);
        unsigned char *public_key_salt  =   &(public_key[F_SALT_OFFSET]);

        int iterations = 100000;
        unsigned char private_key_key[32];
        if(s_pbkdf2_hmac_derive(master_password, strlen(master_password), public_key_salt, 32, iterations, "sha256", private_key_key, 32) == -1) {
            log_error("s_pbkdf2_hmac_derive() failure");
            return -1;
        }
        private_key_key[0] &= 248; // NOLINT(hicpp-signed-bitwise)
        private_key_key[31] &= 127; // NOLINT(hicpp-signed-bitwise)
        private_key_key[31] |= 64; // NOLINT(hicpp-signed-bitwise)

        unsigned char *shared_secret;
        size_t shared_secret_len;
        if((shared_secret_len = s_curve_shared_secret("X25519", e_key, 32, private_key_key, 32, &shared_secret)) == -1u) {
            free(ciphertext_decoded);
            log_error("s_curve_shared_secret() failure");
            return -1;
        }

        char *digest_mode = "sha256";
        EVP_MD_CTX *digester;
        if((digester = s_digest_init(digest_mode)) == NULL) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            log_error("s_digest_init() failure");
            return -1;
        }

        // Getting aes_key digest
        if((digester = s_digest_update(digester, e_salt, 32)) == NULL) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            log_error("s_digest_update() failure");
            return -1;
        }
        if((digester = s_digest_update(digester, "\0", 1)) == NULL) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            log_error("s_digest_update() failure");
            return -1;
        }
        if((digester = s_digest_update(digester, shared_secret, shared_secret_len)) == NULL) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            log_error("s_digest_update() failure");
            return -1;
        }
        if((digester = s_digest_update(digester, e_key, 32)) == NULL) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            log_error("s_digest_update() failure");
            return -1;
        }
        if((digester = s_digest_update(digester, public_key_key, 32)) == NULL) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            log_error("s_digest_update() failure");
            return -1;
        }
        if((digester = s_digest_update(digester, public_key_salt, 32)) == NULL) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            log_error("s_digest_update() failure");
            return -1;
        }

        unsigned char *aes_key;
        int aes_key_len;
        if((aes_key_len = s_digest_digest(digester, digest_mode, &aes_key)) == -1) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            log_error("s_digest_digest() failure");
            return -1;
        }
        /*
        log_verbose("aes_key : ");
        for(int i = 0; i < aes_key_len; i++)
            printf("%02x", aes_key[i]);
        puts("");
        */

        if((digester = s_digest_init(digest_mode)) == NULL) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            free(aes_key);
            log_error("s_digest_init() failure");
            return -1;
        }

        // Getting iv digest
        if((digester = s_digest_update(digester, e_salt, 32)) == NULL) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            free(aes_key);
            log_error("s_digest_update() failure");
            return -1;
        }
        if((digester = s_digest_update(digester, "\2", 1)) == NULL) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            free(aes_key);
            log_error("s_digest_update() failure");
            return -1;
        }
        if((digester = s_digest_update(digester, shared_secret, shared_secret_len)) == NULL) {
            free(ciphertext_decoded);
            free(shared_secret);
            free(public_key_key);
            free(aes_key);
            log_error("s_digest_update() failure");
            return -1;
        }
        free(shared_secret);

        if((digester = s_digest_update(digester, e_key, 32)) == NULL) {
            free(ciphertext_decoded);
            free(public_key_key);
            free(aes_key);
            log_error("s_digest_update() failure");
            return -1;
        }
        if((digester = s_digest_update(digester, public_key_key, 32)) == NULL) {
            free(ciphertext_decoded);
            free(public_key_key);
            free(aes_key);
            log_error("s_digest_update() failure");
            return -1;
        }
        free(public_key_key);

        if((digester = s_digest_update(digester, public_key_salt, 32)) == NULL) {
            free(ciphertext_decoded);
            free(aes_key);
            log_error("s_digest_update() failure");
            return -1;
        }

        unsigned char *iv;
        int iv_len;
        if((iv_len = s_digest_digest(digester, digest_mode, &iv)) == -1) {
            free(ciphertext_decoded);
            free(aes_key);
            log_error("s_digest_digest() failure");
            return -1;
        }
        // 12
        iv_len = F_TAG_LENGTH - 4;

        if(s_aead_aes_256_gcm_decrypt(cipher_password, cipher_password_len, NULL, 0, aes_key, iv, iv_len, plaintext_password, tag) == -1) {
            free(ciphertext_decoded);
            free(aes_key);
            free(iv);
            log_error("s_aead_aes_256_gcm_decrypt() failure");
            return -1;
        }
        free(aes_key);
        free(iv);
    }
    else {
        log_error("Invalid XML attribute value");
        return -1;
    }

    return 1;
}

int dump_filezilla(const char *output_file, const char *master_password) {
	log_info("Starting FileZilla dump ...\n");

	char filezilla_sitemanager_path[MAX_PATH_SIZE];
	char filezilla_recentservers_path[MAX_PATH_SIZE];

	load_filezilla_paths(filezilla_sitemanager_path, filezilla_recentservers_path);

	int result = 0;

	if(access(filezilla_recentservers_path, 0) != -1) {
        log_verbose("FileZilla recentservers path : %s", filezilla_recentservers_path);
        log_info("Starting FileZilla recentservers dump ...\n");
		result = parse_recentservers_xml(output_file, master_password, filezilla_recentservers_path);
	}
    if(access(filezilla_sitemanager_path, 0) != -1) {
        log_verbose("FileZilla sitemanager path : %s", filezilla_sitemanager_path);
        log_info("Starting FileZilla sitemanager dump ...\n");
        result = parse_sitemanager_xml(output_file, master_password, filezilla_sitemanager_path);
    }

	if(result == 0) {
		log_error("Couldn't find any FileZilla installation");
		return -1;
	}
	else if (result == -1) {
		log_error("An error occured");
		return -1;
	}

	return 1;
}
