#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

#include "ModifyXML.h"
extern char * hdfs_path;

int ModifyXML::ModXml(char * fileurl)
{
    xmlDocPtr doc;
    xmlNodePtr cur, ASA;
    int i, j;
    char szFile[512], szBuf[1024], key[256];

    sprintf(szFile, "torrent.xml");

    // 将目录分割为节点
    char bk_dir[30][256];
    char * dir = hdfs_path;
    if (dir[0] == '/')
        dir++;
    int len = strlen(dir), m = 0, k;
    int n = 0;
    if (dir[len - 1] == '/') dir[len - 1] = '\0';
    len = strlen(dir);
    for (k = 0; k < len; k++) {
        if (dir[k] != '/') {
            bk_dir[n][m++] = dir[k];
        } else {
            bk_dir[n][m] = '\0';
            n++;
            m = 0;
        }
    }
    bk_dir[n][m] = '\0';
    n++;
    strcpy(bk_dir[n], "torrent_url");

    // 读取XML
    doc = xmlReadFile(szFile, "UTF-8", XML_PARSE_RECOVER);

    if (NULL == doc) {
        fprintf(stderr, "open file failed!\n");
        return -1;
    }

    cur = xmlDocGetRootElement(doc);
    ASA = cur;
    if (NULL == cur) {
        fprintf(stderr, "Document not parsed sucessfully!\n");
        xmlFreeDoc(doc);
        return -1;
    }

    if (cur->xmlChildrenNode != NULL)
        cur = cur->xmlChildrenNode;

    bool flag = true;
    // 遍历XML
    for (i = 0; i < 5 && flag; i++) {
        while (strcmp((char *) cur->name, bk_dir[i])) {
            cur = cur->next;
            if (cur == NULL) { flag = false; break; }
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

    // 修改XML内容
    char * Content;
    xmlNodePtr node;
    if (strcmp((char *) ASA->name, "torrent_url")) {
        for (; i <= n; i++) {
            node = xmlNewChild(ASA, NULL, BAD_CAST bk_dir[i], NULL);
            ASA  = node;
        }
        xmlNodeSetContent(ASA, BAD_CAST fileurl);
    } else   {
        char content[5000];
        Content = (char *) xmlNodeGetContent(ASA);
        strcpy(content, Content);
        strcat(content, ";");
        strcat(content, fileurl);
        xmlNodeSetContent(ASA, BAD_CAST content);
    }

    // 保存XML
    xmlSaveFormatFileEnc(szFile, doc, "UTF-8", 1);
    delete[] Content;
    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump(); // debug memory for regression tests
    // delete [] szKey, szAttr,ASA,node;
    return 0;
} // ModifyXML::ModXml
