#include "hdfs.h"
#include "DownloadPthread.h"
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

struct data {
    char directory[200], readPath[200], IP[50];
    int  port, PER_LEN;
};
int DownloadPthread::get_random(int rand_max)
{
    assert(rand_max > 0);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int seed = tv.tv_usec;
    srand(seed);
    return rand_r(&seed) % rand_max;
}

int DownloadPthread::CreateDir(const char * sPathName)
{
    char DirName[256];

    strcpy(DirName, sPathName);
    int i, len = strlen(DirName);

    if (DirName[len - 1] != '/')
        strcat(DirName, "/");

    len = strlen(DirName);

    for (i = 1; i < len; i++) {
        if (DirName[i] == '/') {
            DirName[i] = 0;

            if (access(DirName, NULL) != 0) {
                if (mkdir(DirName, 0755) == -1) {
                    perror("mkdir error");
                    return -1;
                }
            }

            DirName[i] = '/';
        }
    }

    return 0;
}

void * DownloadPthread::down(void * args)
{
    struct data * Data = (data *) malloc(sizeof(data));

    Data = (struct data *) args;
    char dec_file[1000], readPath[1000];
    // int port,PER_LEN;
    strcpy(dec_file, Data->directory);
    strcpy(readPath, Data->readPath);
    //  strcpy(IP,Data->IP);
    //  port=Data->port;
    //  PER_LEN=Data->PER_LEN;
    int ret = 0;


    struct timeval start;
    struct timeval end;
    Data->PER_LEN = Data->PER_LEN * 1000 * 1000;
    hdfsFS fs = hdfsConnect(Data->IP, Data->port);

    if (NULL == fs) {
        printf("[Error]cann't connect to hdfs\n");
        exit(-1);
    }

    // 字符串处理
    char * name = strrchr(dec_file, '/');
    char Dir[200];
    int length = strlen(dec_file) - strlen(name);
    name++;
    strcpy(Dir, dec_file);
    Dir[length] = '\0';
    CreateDir(Dir);

    printf("开始下载 %s\n", readPath);

    hdfsFile readFile = hdfsOpenFile(fs, readPath, O_RDONLY, 0, 0, 0);

    if (!readFile) {
        fprintf(stderr, "Failed to open %s for writing!\n", readPath);
        exit(-2);
    }

    BigFile write_file(dec_file, 'w', Data->PER_LEN);
    int i         = 0;
    tSize curSize = 1;
    tSize seek    = 0;

    struct timeval read_start;
    struct timeval read_end;

    char * recv_buff = new char[Data->PER_LEN];
    gettimeofday(&start, NULL);

    while (curSize > 0) {
        char num_blocker[50], NUM[10];

        // 是否分割文件

        strcpy(num_blocker, dec_file);


        gettimeofday(&read_start, NULL);
        curSize = hdfsPread(fs, readFile, seek, (void *) recv_buff, Data->PER_LEN);
        gettimeofday(&read_end, NULL);

        if (curSize > 0) {
            ret = write_file.write_slice(i++, recv_buff, curSize, num_blocker, readPath, false);

            if (ret < 0) {
                delete[] recv_buff;
                exit(-1);
            }
        }

        seek = seek + curSize;
    }

    gettimeofday(&end, NULL);
    // printf("[Trace]use %d us in read process\n",(end.tv_sec - start.tv_sec)*1000*1000+(end.tv_usec - start.tv_usec));

    // begin to test seek
    for (int k = 0; k < i - 1; k++) {
        int pos = get_random((i - 1));
        gettimeofday(&read_start, NULL);
        curSize = hdfsPread(fs, readFile, Data->PER_LEN * (pos), (void *) recv_buff, Data->PER_LEN);
        gettimeofday(&read_end, NULL);

        // printf("[Trace_read_rand]use %d us in read process %d pos\n",(read_end.tv_sec - read_start.tv_sec)*1000*1000+(read_end.tv_usec - read_start.tv_usec),pos);
    }

    hdfsCloseFile(fs, readFile);
    delete[] recv_buff/*, name*/;
    return 0;
} // DownloadPthread::down
