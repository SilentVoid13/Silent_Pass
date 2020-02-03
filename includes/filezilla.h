#ifndef FILEZILLA_H
#define FILEZILLA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libxml/xmlreader.h>

xmlNodePtr retrieve_xml_node(xmlNodePtr cur, char *node_name);
xmlNodePtr retrieve_xml_attribute(xmlNodePtr cur, char *attribute_name, char *attribute_value);

#endif // FILEZILLA_H
