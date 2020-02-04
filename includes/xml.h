#ifndef XML_H
#define XML_H

#include <libxml/xmlreader.h>

xmlNodePtr retrieve_xml_node(xmlNodePtr cur, char *node_name);
xmlNodePtr retrieve_xml_attribute(xmlNodePtr cur, char *attribute_name, char *attribute_value);

int init_xml_file(xmlDocPtr *doc, xmlNodePtr *cur, char *path);

#endif // XML_H
