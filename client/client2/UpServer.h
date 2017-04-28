#include <stdlib.h>
#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/session.hpp"

class UpServer{
	public:
		static void* StartUp(void *args);
};