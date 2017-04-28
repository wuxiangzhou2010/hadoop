#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/torrent_info.hpp"
#include "libtorrent/lazy_entry.hpp"
#include "libtorrent/magnet_uri.hpp"
class DumpTorrent{
public:
	int dump(char* TorrentPath);
};

