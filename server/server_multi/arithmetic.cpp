#include <stdio.h>
#include <vector>
#include <string.h>
#include <hdfs.h>
#include <pthread.h>
#include <signal.h>
#include "makeTorrent.h"
#include "ModifyXML.h"
#include "Download.h"
#include "DownloadPthread.h"

char * IP = new char[20];
char * PORT = new char[20];
char * hdfs_path = new char[100];
char * dec_file = new char[100];
int PER_LEN, port, NumBoundaries = 0, m = 0, n = 0, size[10000], N = 0;
char Record[100000];
char * Name          = new char[100];
float SizeBoundaries = 0;
hdfsFS fs;
pthread_t t[100];
struct data {
    char directory[200], readPath[200], IP[50];
    int  port, PER_LEN;
};

int Block(int size)
{
    if (size <= 300) return size;
    else if (size <= 3000) return 300;
    else if (size <= 20000) return size / 10;
    else return 2000;
}

void PriorityDown(char * path)
{
    char * directory = new char[1000], * temp = new char[1000];
    int Length;
    bool flag1;

    hdfsFS fs = hdfsConnect(IP, port);
    hdfsFileInfo * pt_hdfs_file_info = hdfsGetPathInfo(fs, path);

    temp   = strrchr(path, ':');
    Length = strlen(PORT) + 1;

    while (Length--) temp++;
    Length = strlen(hdfs_path);

    while (Length--) temp++;
    strcpy(directory, dec_file);

    if (strlen(temp)) strcat(directory, temp);

    if (pt_hdfs_file_info->mKind == 'F') {
        hdfsFS fs = hdfsConnect(IP, port);
        Download down(IP, port, PER_LEN);

        // 父目录满足条件 核对文件大小是否自动作种
        if (pt_hdfs_file_info->mSize / 1000 / 1000 >= PER_LEN) {
            flag1 = true;
            hdfsDisconnect(fs);
            down.down(directory, path, flag1);
        }
    } else {
        hdfsFS fs = hdfsConnect(IP, port);
        hdfsFileInfo * fileList = 0;
        int numEntries = 0, i = 0;

        if ((fileList = hdfsListDirectory(fs, path, &numEntries)) != NULL) {
            for (i = 0; i < numEntries; ++i) {
                PriorityDown(fileList[i].mName);
            }
        }
    }
    delete[] directory;
} // PriorityDown

void Ergodic(char * path, hdfsFS fs)
{
    char * directory = new char[1000], * temp = new char[1000];
    int Length;
    bool flag1 = true;
    int pthread_kill_err;
    hdfsFileInfo * pt_hdfs_file_info = hdfsGetPathInfo(fs, path);

    // 字符串处理
    temp   = strrchr(path, ':');
    Length = strlen(PORT) + 1;

    while (Length--) temp++;
    Length = strlen(hdfs_path);

    while (Length--) temp++;
    strcpy(directory, dec_file);

    if (strlen(temp)) strcat(directory, temp);

    if (pt_hdfs_file_info->mKind == 'F') {
        // hdfsFS fs = hdfsConnect(IP,port);
        struct data * Data = (data *) malloc(sizeof(data));
        strcpy(Data->IP, IP);
        strcpy(Data->readPath, path);
        Data->port    = port;
        Data->PER_LEN = PER_LEN;

        // Download down(IP,port,PER_LEN);
        // 父目录满足条件 核对文件大小是否自动作种
        if (pt_hdfs_file_info->mSize / 1.0 / 1000 / 1000 < PER_LEN) {
            if ((SizeBoundaries + pt_hdfs_file_info->mSize / 1000 / 1000 > PER_LEN) ||
              (NumBoundaries >= 500))
            {
                for (int i = 0; i < 2; i++) pthread_join(t[i], NULL);
                MakeTorrent make;
                make.make(Name, "udp://tracker.bitcomet.net:8080/announce");
                Record[0]      = '\0';
                SizeBoundaries = 0;
                NumBoundaries  = 0;
                N = 0;

                // memset(Record,0,sizeof(Record));
            }
            strcpy(Name, directory);
            strcat(Record, directory);
            strcat(Record, "/");
            SizeBoundaries += pt_hdfs_file_info->mSize / 1.0 / 1000 / 1000;
            NumBoundaries++;

            // hdfsDisconnect(fs);
            strcpy(Data->directory, directory);

            if (N > 1) {
                while (flag1) {
                    if (pthread_kill(t[0], 0) == ESRCH) {
                        pthread_create(&t[0], NULL, &DownloadPthread::down, (void *) (Data));
                        flag1 = false;
                    } else if (!pthread_kill(t[1], 0) == ESRCH) {
                        pthread_create(&t[1], NULL, &DownloadPthread::down, (void *) (Data));
                        flag1 = false;
                    }
                }
            } else {
                pthread_create(&t[N++], NULL, &DownloadPthread::down,
                  (void *) (Data));
            }

            // pthread_create(&t[N++],NULL,&DownloadPthread::down,(void*)(Data));
            // down.down(directory,path,false);
        }
        n++;
    } else {
        n++;
        hdfsFS fs = hdfsConnect(IP, port);
        hdfsFileInfo * fileList = 0;
        int numEntries = 0, i = 0;

        if ((fileList = hdfsListDirectory(fs, path, &numEntries)) != NULL) {
            for (i = 0; i < numEntries; ++i) Ergodic(fileList[i].mName, fs);
        }

        // hdfsDisconnect(fs);
    }
    delete[] directory;
} // Ergodic

int figureUpDirSize(char * path, hdfsFS fs)
{
    hdfsFileInfo * pt_hdfs_file_info = hdfsGetPathInfo(fs, path);

    if (pt_hdfs_file_info->mKind == 'F') {
        return pt_hdfs_file_info->mSize / 1.0 / 1000 / 1000;
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
    time_t ts, te;

    ts = time(NULL);

    if (argc < 3) {
        printf("Usage:<programe> <IP> <port> <hdfs_path> <des_path> \n");
        exit(1);
    }

    // 字符串处理
    strcpy(IP, argv[1]);
    strcpy(PORT, argv[2]);
    strcpy(hdfs_path, argv[3]);
    strcpy(dec_file, argv[4]);

    int len = strlen(hdfs_path);

    if (hdfs_path[len - 1] == '/') hdfs_path[len - 1] = '\0';
    len = strlen(dec_file);

    if (dec_file[len - 1] == '/') dec_file[len - 1] = '\0';
    strcat(dec_file, strchr(hdfs_path, '/'));
    port = atoi(PORT);

    printf("[Trace]hdfs:%s dec:%s LEN:%dm\n", dec_file, hdfs_path, PER_LEN);

    hdfsFS fs = hdfsConnect(IP, port);

    if (NULL == fs) {
        printf("[Error]cann't connect to hdfs\n");
        exit(-1);
    }
    ret = hdfsExists(fs, hdfs_path);

    if (ret != 0) {
        fprintf(stderr, "[Error]%s is not exist in hdfs\n", hdfs_path);
        exit(-1);
    }
    hdfsFileInfo * pt_hdfs_file_info = hdfsGetPathInfo(fs, hdfs_path);

    // 计算各目录大小
    PER_LEN = figureUpDirSize(pt_hdfs_file_info->mName, fs);
    PER_LEN = Block(PER_LEN);
    printf("分块大小为：%d\n", PER_LEN);

    //下载+作种
    PriorityDown(pt_hdfs_file_info->mName);
    fs = hdfsConnect(IP, port);
    Ergodic(pt_hdfs_file_info->mName, fs);

    for (int i = 0; i < N; i++) pthread_join(t[i], NULL);
    hdfsDisconnect(fs);

    if (NumBoundaries > 0) {
        for (int i = 0; i < 2; i++) pthread_join(t[i], NULL);
        fs = hdfsConnect(IP, port);
        MakeTorrent make;
        make.make(Name, "udp://tracker.bitcomet.net:8080/announce");
        hdfsDisconnect(fs);
    }
    char * End = new char[5];
    strcpy(End, "end;\0");
    ModifyXML ModifyXML;
    ModifyXML.ModXml(End);
    te = time(NULL);
    printf("%d_默认开启上传两小时...\n", te - ts);
    delete[] IP, PORT, hdfs_path, dec_file, Record, Name, End;

    /*ts=time(NULL);te=time(NULL);
     * while(te-ts<=10800)
     *      te=time(NULL);*/
    return 0;
} // main
