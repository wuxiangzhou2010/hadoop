#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "BigFile.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "makeTorrent.h"

BigFile::BigFile(const char * path, char type, int slice_size)
{
    m_file      = NULL;
    m_file_name = path;

    switch (type) {
        case 'r':
            m_type = 0;
            m_file = fopen(path, "rb");
            break;
        case 'w':
            m_type = 1;
            // m_file = fopen(path, "wb");
            break;
defalut:    break;
    }

    m_slice_size = slice_size;
}

BigFile::~BigFile()
{
    if (m_file != NULL) {
        fclose(m_file);
        m_file = NULL;
    }
}

int BigFile::get_slice_num()
{
    struct stat file_stat;
    int ret = stat(m_file_name.c_str(), &file_stat);

    if (ret != 0) {
        printf("[Notice]stat file %s erro:%d", m_file_name.c_str(), errno);
        return -1;
    }

    int num = file_stat.st_size / m_slice_size;

    if ((file_stat.st_size) % m_slice_size != 0) {
        num = num + 1;
    }

    return num;
}

/*
 * int BigFile::read_with_slice(int slice_num, char* value, int len)
 * {
 *  int ret = 0;
 *  if (NULL == m_file){
 *      fprintf(stderr, "[read_with_slice]m_file is NULL\n");
 *      return -1;
 *  }
 *
 *  int seek = fseek(m_file, (slice_num)*m_slice_size, SEEK_SET);
 *  if (seek != 0){
 *      int num = get_slice_num();
 *      fprintf(stderr, "[read_with_slice]seek fail:%d %d:%d:%d\n",errno,slice_num,num,m_slice_size);
 *      return -1;
 *  }
 *
 *  if (len > m_slice_size){
 *      ret = fread(value,1,m_slice_size,m_file);
 *  }else{
 *      ret = fread(value, 1, len, m_file);
 *  }
 *  return ret;
 * }*/

int BigFile::write_slice(int slice_num, char * value, int len, char * path, char * readPath, bool flag)
{
    m_file = fopen(path, "wb");

    if (NULL == m_file) {
        // fprintf(stderr, "[write_slice]m_file is NULL\n");
        perror("fopen");
        return -1;
    }

    int ret = fseek(m_file, 0, SEEK_END);

    if (ret != 0) {
        fprintf(stderr, "[write_slice]seek fail:%d\n", errno);
        return -1;
    }

    // 字符串处理
    char torrentPath[1000];
    strcpy(torrentPath, path);
    int length = strlen(torrentPath);
    strcpy(torrentPath + length, ".torrent");


    // append to the file
    ret = fwrite(value, 1, len, m_file);

    // 单个文件是否分块作种
    const char * tracker = "udp://tracker.bitcomet.net:8080/announce";
    if (flag) {
        MakeTorrent::make(path, torrentPath, readPath, tracker);
    }

    // delete[] value,path,readPath;
    return ret;
} // BigFile::write_slice
