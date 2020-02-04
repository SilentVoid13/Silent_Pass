#ifndef FILEZILLA_H
#define FILEZILLA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "xml.h"

int parse_sitemanager_xml(int verbose, const char *output_file, const char *master_password, char *path);
int parse_recentservers_xml(int verbose, const char *output_file, const char *master_password, char *path);

int parse_xml_password(xmlDocPtr doc, xmlNodePtr cur);

int load_filezilla_paths(char *filezilla_sitemanager_path, char *filezilla_recentservers_path);

#endif // FILEZILLA_H
