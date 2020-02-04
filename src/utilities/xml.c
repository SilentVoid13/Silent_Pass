#include "xml.h"
#include "main.h"

int init_xml_file(xmlDocPtr *doc, xmlNodePtr *cur, char *path) {
	*doc = xmlParseFile(path);
	if(*doc == NULL) {
		fprintf(stderr, "xmlReaderForFile() failure\n");
		return -1;
	}

	*cur = xmlDocGetRootElement(*doc);
	if(*cur == NULL) {
		fprintf(stderr, "xmlDocGetRootElement() failure\n");
		return -1;
	}

	return 1;
}

xmlNodePtr retrieve_xml_node(xmlNodePtr cur, char *node_name) {
	int valid = 0;
	while(cur != NULL) {
		if(xmlStrcmp(cur->name, (const xmlChar *) node_name) == 0) {
			valid = 1;
			break;
		}
		cur = cur->next;
	}

	if(valid)
		return cur;
	else
		return NULL;
}

xmlNodePtr retrieve_xml_attribute(xmlNodePtr cur, char *attribute_name, char *attribute_value) {
	xmlChar *attribute;
	int valid = 0;
	while(cur != NULL) {
		attribute = xmlGetProp(cur, attribute_name);
		if(attribute != NULL && strcmp(attribute, attribute_value) == 0) {
			valid = 1;
			break;
		}

		cur = cur->next;
	}
	
	if(valid)
		return cur;
	else
		return NULL;
}
