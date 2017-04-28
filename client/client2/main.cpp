#include "dump_torrent.h"
#include <hdfs.h>
char * IP = new char[20];
char * PORT = new char[20];
char * hdfs_path = new char[1000];
int PER_LEN, m = 0, n = 0, size[10000], Port;

std::string branch_path(std::string const& f)
{
    if (f.empty()) return f;

#ifdef TORRENT_WINDOWS
    if (f == "\\\\") return "";
#endif
    if (f == "/") return "";

    int len = int(f.size());
    // if the last character is / or \ ignore it
    if (f[len-1] == '/' || f[len-1] == '\\') --len;
    while (len > 0)
    {
        --len;
        if (f[len] == '/' || f[len] == '\\')
            break;
    }

    if (f[len] == '/' || f[len] == '\\') ++len;
    return std::string(f.c_str(), len);
}



int Block(int size)
{
    if (size <= 300) return size;
    else if (size <= 3000) return 300;
    else if (size <= 20000) return size / 10;
    else return 2000;
}

int figureUpDirSize(char * path, hdfsFS fs)
{
    hdfsFileInfo * pt_hdfs_file_info = hdfsGetPathInfo(fs, path);

    if (pt_hdfs_file_info->mKind == 'F') {
        return pt_hdfs_file_info->mSize / 1000 / 1000;
    } else {
        int m = n++;
        hdfsFileInfo * fileList = 0;
        int numEntries = 0;
        if ((fileList = hdfsListDirectory(fs, path, &numEntries)) != NULL) {
            int i = 0;
            for (i = 0; i < numEntries; i++) {
                size[m] += figureUpDirSize(fileList[i].mName, fs);
            }
        }
        return size[m];
    }
}

int main(int argc, char ** argv)
{
    int ret = 0;

    if (argc < 3) {
        printf("Usage:<programe> <IP> <port> <hdfs_path> <Torrent_path1> <Torrent_path2> ....\n");
        exit(1);
    }

    strcpy(IP, argv[1]);
    strcpy(PORT, argv[2]);
    Port = atoi(PORT);
    hdfsFS fs = hdfsConnect(IP, Port);
    PER_LEN = figureUpDirSize(argv[3], fs);
    PER_LEN = Block(PER_LEN);
    hdfsDisconnect(fs);
    printf("总大小为%d：分块大小为%d\n\n", size[0], PER_LEN);

    strcpy(hdfs_path, branch_path(argv[3]).c_str());
    DumpTorrent DumpTorrent;

    for (int i = 4; i < argc; i++) {
        DumpTorrent.dump(argv[i]);
    }
    char c;
    scanf("%c", &c);
}
