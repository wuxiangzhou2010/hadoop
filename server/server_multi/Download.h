#include "hdfs.h"
#include "BigFile.h"
#include <string.h>
#include <sys/time.h>
#include <assert.h>

class Download
{
public:
    int down(char * dec_file, char * writePath, bool flag);
    Download(char * IP, int Port, int PER_LEN);
private:
    int get_random(int rand_max);
    int CreateDir(const char * sPathName);
    char * D_IP;
    int D_Port;
    int D_PER_LEN;
};
