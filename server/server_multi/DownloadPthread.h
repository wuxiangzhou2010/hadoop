#include "hdfs.h"
#include "BigFile.h"
#include <string.h>
#include <sys/time.h>
#include <assert.h>

class DownloadPthread
{
public:
    static void * down(void * args);
private:
    static int get_random(int rand_max);
    static int CreateDir(const char * sPathName);
};
