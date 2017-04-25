#include <netinet/in.h> // for sockaddr_in
#include <sys/types.h>  // for socket
#include <sys/socket.h> // for socket
#include <stdio.h>      // for printf
#include <stdlib.h>     // for exit
#include <string.h>     // for bzero
#include <arpa/inet.h>  // for inet_aton

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
class UpTorrentToTrack {
public:
    char * Send(char * torrent_Path);
};
