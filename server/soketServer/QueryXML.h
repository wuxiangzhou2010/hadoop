#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

class QueryXML {
public:
    char * QueXML(char * input);
};

// (char*)xmlNodeGetContent(cur));
// xmlReplaceNode
