#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/torrent_info.hpp"
#include "libtorrent/lazy_entry.hpp"
#include "libtorrent/magnet_uri.hpp"
#include "dump_torrent.h"
#include "Download.h"
#include "UpServer.h"
#include <vector>
#include <fstream>

extern char * IP;
extern int Port;
extern int PER_LEN;
extern char * hdfs_path;


int load_file(std::string const& filename, std::vector<char>& v,
  libtorrent::error_code& ec, int limit = 8000000)
{
    ec.clear();
    FILE * f = fopen(filename.c_str(), "rb");
    if (f == NULL) {
        ec.assign(errno, boost::system::system_category());
        return -1;
    }

    int r = fseek(f, 0, SEEK_END);
    if (r != 0) {
        ec.assign(errno, boost::system::system_category());
        fclose(f);
        return -1;
    }
    long s = ftell(f);
    if (s < 0) {
        ec.assign(errno, boost::system::system_category());
        fclose(f);
        return -1;
    }

    if (s > limit) {
        fclose(f);
        return -2;
    }

    r = fseek(f, 0, SEEK_SET);
    if (r != 0) {
        ec.assign(errno, boost::system::system_category());
        fclose(f);
        return -1;
    }

    v.resize(s);
    if (s == 0) {
        fclose(f);
        return 0;
    }

    r = fread(&v[0], 1, v.size(), f);
    if (r < 0) {
        ec.assign(errno, boost::system::system_category());
        fclose(f);
        return -1;
    }

    fclose(f);

    if (r != s) return -3;

    return 0;
} // load_file
/*
std::vector<char> load_file(std::string const& filename)
{
    std::vector<char> ret;
    std::fstream in;
    in.exceptions(std::ifstream::failbit);
    in.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    in.seekg(0, std::ios_base::end);
    size_t const size = in.tellg();
    in.seekg(0, std::ios_base::beg);
    ret.resize(size);
    in.read(ret.data(), ret.size());
    return ret;
}
*/
struct TorrentData {
    char FilePath[200], TorrentPath[200];
};

int DumpTorrent::dump(char * TorrentPath)
{
    using namespace libtorrent;

    int item_limit  = 1000000;
    int depth_limit = 1000;


    // int size = file_size(TorrentPath);
    // if (size > 40 * 1000000) {
    //     fprintf(stderr, "file too big (%d), aborting\n", size);
    //     return 1;
    // }
    std::vector<char> buf/*(size)*/;
    error_code ec;
    int ret = load_file(TorrentPath, buf, ec, 40 * 1000000);
    if (ret != 0) {
        fprintf(stderr, "failed to load file: %s\n", ec.message().c_str());
        return 1;
    }
    lazy_entry e;
    int pos;
    ret = lazy_bdecode(&buf[0], &buf[0] + buf.size(), e, ec, &pos,
        depth_limit, item_limit);


    torrent_info t(e, ec);
    // print info about torrent

    typedef std::vector<std::pair<std::string, int> > node_vec;
    node_vec const& nodes = t.nodes();

    char ih[41];
    to_hex((char const *) &t.info_hash()[0], 20, ih);
    printf("种子文件个数为: %d\n种子包含文件为\n", t.num_files());
    int index = 0;
    if (t.num_files() == 1) {
        bool flag   = false;
        char * name = new char[100];
        for (torrent_info::file_iterator i = t.begin_files(); i != t.end_files(); ++i, ++index) {
            printf("%s\n", t.files().file_path(*i).c_str());
            strcpy(name, t.files().file_path(*i).c_str());
            printf("%s\n", name);
            int len = strlen(name);
            printf("%d\n", len);
            if (name[len - 2] == '+') {
                if (name[len - 1] == '1') {
                    name[len - 2] = '\0';
                    printf("123414234234234\n");
                    flag = true;
                } else { break; }
            }
            printf("213423423423423@@%s\n", name);
            Download Down(IP, Port, PER_LEN);
            char * directory = new char[100];
            char * HDFSPath  = new char[100];
            strcpy(directory, "./");
            strcat(directory, name);
            strcpy(HDFSPath, hdfs_path);
            strcat(HDFSPath, name);
            Down.down(directory, HDFSPath, flag);
            delete[] directory, HDFSPath, name;
        }
    } else   {
        for (torrent_info::file_iterator i = t.begin_files(); i != t.end_files(); ++i, ++index) {
            Download Down(IP, Port, PER_LEN);
            char * directory = new char[100];
            char * HDFSPath  = new char[100];
            strcpy(directory, "./");
            strcat(directory, t.files().file_path(*i).c_str());
            strcpy(HDFSPath, hdfs_path);
            strcat(HDFSPath, t.files().file_path(*i).c_str());
            Down.down(directory, HDFSPath, false);
            delete[] directory, HDFSPath;
        }
    }
    pthread_t thread;
    struct TorrentData * Data = (TorrentData *) malloc(sizeof(TorrentData));
    strcpy(Data->FilePath, "./");
    strcpy(Data->TorrentPath, TorrentPath);
    pthread_create(&thread, NULL, &UpServer::StartUp, (void *) (Data));


    return 0;
} // DumpTorrent::dump
