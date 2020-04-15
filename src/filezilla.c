#include "filezilla.h"
#include "main.h"

#include "log.h"
#include "xml.h"
#include "s_base64.h"
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
			parse_xml_password(doc, cur->xmlChildrenNode, output_file, master_password);	
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
			parse_xml_password(doc, cur->xmlChildrenNode, output_file, master_password);	
		}
		cur = cur->next;
	}
	printf("\n");

	xmlFreeDoc(doc);
	xmlCleanupParser();

	return 1;
}

int parse_xml_password(xmlDocPtr doc, xmlNodePtr cur, const char *output_file, const char *master_password) {
    // For warning
    (void) master_password;

	xmlChar *key;
	char *host = NULL;
	char *username = NULL;
	char *cipher_password = NULL;
	unsigned char *plaintext_password = NULL;
	char port[6] = {-1};

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
                key_len = strlen(key);
				cipher_password = malloc(key_len+1);
                if(cipher_password == NULL) {
                    free(cipher_password);
                    log_error("malloc() failure");
                    return -1;
                }
				safe_strcpy(cipher_password, key, key_len);
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

	puts("here4");

	if(cipher_password != NULL) {
		int cipher_password_len = (int)strlen(cipher_password);
		if(s_base64_decode(cipher_password, cipher_password_len, &plaintext_password) == -1) {
			free(username);
			free(cipher_password);
			free(host);
			log_error("base64_decode() failure");
			return -1;
		}
		free(cipher_password);
	}

	puts("here44");
	// We only add to input file when we have full creds (Maybe change that ?)
	if(output_file != NULL && host != NULL && username != NULL && plaintext_password != NULL) {
		FILE *output_fd = fopen(output_file, "ab");
		fprintf(output_fd, "\"%s\",\"%s\",\"%s\"\n", 
			host,
			username,
			plaintext_password);
		fclose(output_fd);
	}

    puts("here45");

	printf("\n");
    puts("here46");
	if(host != NULL) {
        puts("here54");
		log_success("Host : %s", host);
		puts("here55");
		free(host);
	}
	puts("here6");
	if(port[0] != -1) {
		log_success("Port : %s", port);
	}
    puts("here7");
	if(username != NULL) {
		log_success("Username: %s", username);
		free(username);
	}
    puts("here8");
	if(plaintext_password != NULL) {
		log_success("Password : %s", plaintext_password);
		free(plaintext_password);
	}

	return 1;
}

int dump_filezilla(const char *output_file, const char *master_password) {
	log_info("Starting FileZilla dump ...\n");

	char filezilla_sitemanager_path[MAX_PATH_SIZE];
	char filezilla_recentservers_path[MAX_PATH_SIZE];

	load_filezilla_paths(filezilla_sitemanager_path, filezilla_recentservers_path);

	int result = 0;

	if(access(filezilla_sitemanager_path, 0) != -1) {
	    log_verbose("FileZilla sitemanager path : %s", filezilla_sitemanager_path);
	    log_info("Starting FileZilla sitemanager dump ...");
		result = parse_sitemanager_xml(output_file, master_password, filezilla_sitemanager_path);
	}

	if(access(filezilla_recentservers_path, 0) != -1) {
        log_verbose("FileZilla recentservers path : %s", filezilla_recentservers_path);
        log_info("Starting FileZilla recentservers dump ...");
		result = parse_recentservers_xml(output_file, master_password, filezilla_recentservers_path);
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
