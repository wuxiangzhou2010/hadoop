#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include "QueryXML.h"

char * QueryXML::QueXML(char * input)
{
    xmlDocPtr doc;
    xmlNodePtr cur, ASA;
    int i, j;
    char szFile[512], szBuf[1024], key[256];

    sprintf(szFile, "torrent.xml");

    // 将目录分割为节点
    char b[256][256];
    if (input[0] == '/')
        input++;
    int len = strlen(input), m = 0, k;
    int n = 0;
    if (input[len - 1] == '/') input[len - 1] = '\0';
    len = strlen(input);
    for (k = 0; k < len; k++) {
        if (input[k] != '/') {
            b[n][m++] = input[k];
        } else {
            b[n][m] = '\0';
            n++;
            m = 0;
        }
    }
    b[n][m] = '\0';
    n++;
    strcpy(b[n], "torrent_url");

    // 打开XML
    doc = xmlReadFile(szFile, "UTF-8", XML_PARSE_RECOVER);
    if (NULL == doc) {
        fprintf(stderr, "open file failed!\n");
        return "";
    }

    cur = xmlDocGetRootElement(doc);
    ASA = cur;
    if (NULL == cur) {
        fprintf(stderr, "Document not parsed sucessfully!\n");
        xmlFreeDoc(doc);
        return "";
    }

    // 遍历XML
    if (cur->xmlChildrenNode != NULL)
        cur = cur->xmlChildrenNode;
    bool flag = true;
    int l     = n + 1;
    for (i = 0; i < l && flag; i++) {
        while (strcmp((char *) cur->name, b[i])) {
            cur = cur->next;
            if (cur == NULL) {
                flag = false;
                break;
            }
        }
        if (cur != NULL) {
            ASA = cur;
            if (cur->xmlChildrenNode != NULL)
                cur = cur->xmlChildrenNode;
            else break;
        }
    }
    if (cur == NULL) i--;
    else i++;
    char * q      = (char *) xmlNodeGetContent(ASA);
    char * chaxun = new char[1000];

    xmlNodePtr node;
    strcpy(chaxun, "正在制作种子...\0");
    if (!strcmp(q, "") || strcmp((char *) ASA->name, "torrent_url")) {
        for (; i <= n; i++) {
            node = xmlNewChild(ASA, NULL, BAD_CAST b[i], NULL);
            ASA  = node;
        }
        xmlNodeSetContent(ASA, BAD_CAST ";");
        xmlSaveFile(szFile, doc);
        /*free the document */
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        delete[] q;
        return chaxun;
    } else {
        strcpy(chaxun, q);
        xmlSaveFile(szFile, doc);
        /*free the document */
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        delete[] q;
        return chaxun;
    }
    // debug memory for regression tests
} // QueryXML::QueXML

// (char*)xmlNodeGetContent(cur));
// xmlReplaceNode
