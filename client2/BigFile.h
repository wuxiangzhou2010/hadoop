#ifndef _BIG_FILE
#define _BIG_FILE

#include <stdio.h>
#include <unistd.h>
#include <string>

// 支持按照块进行读写
class BigFile {
public:
    int write_slice(int slicenum, char * value, int len, char * path, char * readPath, bool flag);
    int get_slice_num();
    //   int read_with_slice(int slicenum, char* value, int len);
    BigFile(const char * path, char type, int slice_size);
    ~BigFile();
private:
    FILE * m_file;
    int m_type;
    int m_slice_size;
    std::string m_file_name;
};

#endif // ifndef _BIG_FILE
