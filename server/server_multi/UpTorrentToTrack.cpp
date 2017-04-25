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
#include "UpTorrentToTrack.h"

#define HELLO_WORLD_SERVER_PORT 6666
#define BUFFER_SIZE             1024
#define TORRENT_URL_MAX         512

char * UpTorrentToTrack::Send(char * torrent_Path)
{
    char * trackIP = "211.87.230.222";
    // 设置一个socket地址结构client_addr,代表客户机internet地址, 端口
    struct sockaddr_in client_addr;

    bzero(&client_addr, sizeof(client_addr)); // 把一段内存区的内容全部设置为0
    client_addr.sin_family      = AF_INET;    // internet协议族
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);// INADDR_ANY表示自动获取本机地址
    client_addr.sin_port        = htons(0); // 0表示让系统自动分配一个空闲端口
    // 创建用于internet的流协议(TCP)socket,用client_socket代表客户机socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    // 把客户机的socket和客户机的socket地址结构联系起来
    if (bind(client_socket, (struct sockaddr *) &client_addr, sizeof(client_addr))) {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }

    // 设置一个socket地址结构server_addr,代表服务器的internet地址, 端口
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if (inet_aton(trackIP, &server_addr.sin_addr) == 0) { //服务器的IP地址来自程序的参数  inet_aton()是一个改进的方法来将一个字符串IP地址转换为一个32位的网络序列IP地址
        printf("Server IP Address Error!\n");
        exit(1);
    }
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);
    // 向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
    if (connect(client_socket, (struct sockaddr *) &server_addr, server_addr_length) < 0) {
        printf("Can Not Connect To %s!\n", trackIP);
        exit(1);
    }

    char * torrent_name = strrchr(torrent_Path, '/');
    torrent_name++;

    // 发送文件名
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strncpy(buffer, torrent_name, strlen(torrent_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(torrent_name));
    send(client_socket, buffer, BUFFER_SIZE, 0);

    bzero(buffer, BUFFER_SIZE);
    int length = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (length < 0) {
        printf("Server Recieve Data Failed!\n");
        return "";
    }
    char * URL = new char[TORRENT_URL_MAX + 1];
    bzero(URL, TORRENT_URL_MAX + 1);
    strncpy(URL, buffer, strlen(buffer) > TORRENT_URL_MAX ? TORRENT_URL_MAX : strlen(buffer));

    // 发送种子
    FILE * fp = fopen(torrent_Path, "r");
    if (NULL == fp) {
        printf("File:\t%s Not Found\n", torrent_name);
    } else {
        bzero(buffer, BUFFER_SIZE);
        int file_block_length = 0;
        //      while( (file_block_length = read(fp,buffer,BUFFER_SIZE))>0)
        while ( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) {
            // 发送buffer中的字符串到new_server_socket,实际是给客户端
            if (send(client_socket, buffer, file_block_length, 0) < 0) {
                printf("Send File:\t%s Failed\n", torrent_name);
                break;
            }
            bzero(buffer, BUFFER_SIZE);
        }
        //      close(fp);
        fclose(fp);
        printf("File:\t%s Transfer Finished\n", torrent_name);
    }

    close(client_socket);
    return URL;
} // UpTorrentToTrack::Send
