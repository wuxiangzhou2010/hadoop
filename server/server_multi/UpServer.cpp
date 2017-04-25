#include <stdlib.h>
#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/session.hpp"
#include "UpServer.h"
#include <time.h>

struct TorrentData {
    char FilePath[200], TorrentPath[200];
};

void * UpServer::StartUp(void * args)
{
    struct TorrentData * Data = (TorrentData *) malloc(sizeof(TorrentData));

    Data = (struct TorrentData *) args;
    char COMMAND[100] = "./upServer ";
    strcat(COMMAND, Data->TorrentPath);
    strcat(COMMAND, " -s ");
    strcat(COMMAND, Data->FilePath);
    system(COMMAND);
}
