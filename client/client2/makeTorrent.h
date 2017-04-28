#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/torrent_info.hpp"
#include "libtorrent/file.hpp"
#include "libtorrent/storage.hpp"
#include "libtorrent/hasher.hpp"
#include "libtorrent/create_torrent.hpp"
#include "libtorrent/file.hpp"
#include "libtorrent/file_pool.hpp"
#include <boost/bind.hpp>

class MakeTorrent {
public:
    static int make(char * path, char * outFile, char * readPath, const char *track);
    static int make(char * readPath, const char * track);
private:
    static void print_progress(int i, int num);
    static bool file_filter1(std::string const& f);
    static bool file_filter2(std::string const& f);
};
