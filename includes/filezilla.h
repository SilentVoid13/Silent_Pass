#ifndef FILEZILLA_H
#define FILEZILLA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "xml.h"

#define F_KEY_OFFSET 0
#define F_SALT_OFFSET 32
#define F_CIPHER_PASSWORD_OFFSET 64
#define F_TAG_LENGTH 16

int parse_sitemanager_xml(const char *output_file, const char *master_password, char *path);
int parse_recentservers_xml(const char *output_file, const char *master_password, char *path);

int parse_server_xml(xmlDocPtr doc, xmlNodePtr cur, const char *output_file, const char *master_password);

int decrypt_filezilla_password(xmlNodePtr cur, unsigned char *ciphertext, size_t ciphertext_len, unsigned char **plaintext_password, const char *master_password);

int load_filezilla_paths(char *filezilla_sitemanager_path, char *filezilla_recentservers_path);

int dump_filezilla(const char *output_file, const char *master_password);

#endif // FILEZILLA_H
