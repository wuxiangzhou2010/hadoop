#include <netinet/in.h> // for sockaddr_in
#include <sys/types.h>  // for socket
#include <sys/socket.h> // for socket
#include <stdio.h>      // for printf
#include <stdlib.h>     // for exit
#include <string.h>     // for bzero
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "QueryXML.h"

#define HELLO_WORLD_SERVER_PORT 6666
#define LENGTH_OF_LISTEN_QUEUE  20
#define BUFFER_SIZE             1024
#define INFORMATION_MAX         512

int main(int argc, char * argv[])
{
    char COMMAND[2000];
    char torrent_URL[10000];
    char IP[20];
    char Port[10];
    char HDFSPath[100];
    // daemon(1,0);
    // 设置一个socket地址结构server_addr,代表服务器internet地址, 端口
    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr)); // 把一段内存区的内容全部设置为0
    server_addr.sin_family      = AF_INET;// AF=ADDRESS FAMILY 地址族
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);// INADDR_ANY 代表了本地任何地址.  一般指: 0.0.0.0
    server_addr.sin_port        = htons(HELLO_WORLD_SERVER_PORT);

    // 创建用于internet的流协议(TCP)socket,用server_socket代表服务器socket
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);// PF_INET=PROTOCOL FAMILY 协议族 SOCK_STREAM=提供面向连接的稳定数据传输，即TCP协议
    if (server_socket < 0) {
        printf("Create Socket Failed!");
        exit(1);
    }
    {
        int opt = 1;
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }

    // 把socket和socket地址结构联系起来
    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        printf("Server Bind Port : %d Failed!", HELLO_WORLD_SERVER_PORT);
        exit(1);
    }

    // server_socket用于监听
    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE) ) {
        printf("Server Listen Failed!");
        exit(1);
    }
    while (1) { //服务器端要一直运行
        // 定义客户端的socket地址结构client_addr
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);

        // 接受一个到server_socket代表的socket的一个连接
        // 如果没有连接请求,就等待到有连接请求--这是accept函数的特性
        // accept函数返回一个新的socket,这个socket(new_server_socket)用于同连接到的客户的通信
        // new_server_socket代表了服务器和客户端之间的一个通信通道
        // accept函数把连接到的客户端信息填写到客户端的socket地址结构client_addr中
        int new_server_socket = accept(server_socket, (struct sockaddr *) &client_addr, &length);
        if (new_server_socket < 0) {
            printf("Server Accept Failed!\n");
            break;
        }

        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        printf("我在等待信息\n");
        length = recv(new_server_socket, buffer, BUFFER_SIZE, 0);
        if (length < 0) {
            printf("Server Recieve Data Failed!\n");
            break;
        } else if (length == 0) {
            ;
        } else {
            char Information[INFORMATION_MAX + 1];
            bzero(Information, INFORMATION_MAX + 1);
            strncpy(Information, buffer, strlen(buffer) > INFORMATION_MAX ? INFORMATION_MAX : strlen(buffer));

            ////接受到的client端信息strcat(Information," /home/stu/音乐/");
            printf("%s\n", Information);
            int len = strlen(Information), N = 0, m = 0;
            for (int i = 0; i < len; i++) {
                if (N == 2) { HDFSPath[m++] = Information[i]; } else {
                    if (Information[i] == ',') {
                        if (N == 0) IP[m] = '\0';
                        else if (N == 1) Port[m] = '\0';
                        m = 0;
                        N++;
                    } else {
                        if (N == 0) IP[m++] = Information[i];
                        else if (N == 1) Port[m++] = Information[i];
                    }
                }
            }
            HDFSPath[m] = '\0';

            printf("IP=%s\nPort=%s\nHDFSPath=%s\n", IP, Port, HDFSPath);
            QueryXML QueryXML;
            char * torrent_url = QueryXML.QueXML(HDFSPath);

            strcpy(torrent_URL, torrent_url);
            strcat(torrent_URL, "\0");
            delete[] torrent_url;
            char Rt[30];
            strcpy(Rt, "正在制作种子...\0");
            printf("%s\n", torrent_URL);

            strcpy(COMMAND, "./server ");
            strcat(COMMAND, IP);
            strcat(COMMAND, " ");
            strcat(COMMAND, Port);
            strcat(COMMAND, " ");
            strcat(COMMAND, HDFSPath);
            strcat(COMMAND, " /home/stu/音乐/\0");
            if (!strcmp(torrent_URL, Rt)) {
                bzero(buffer, BUFFER_SIZE);
                strncpy(buffer, Rt, strlen(Rt) > BUFFER_SIZE ? BUFFER_SIZE : strlen(Rt));
                buffer[strlen(Rt)] = '\0';
                send(new_server_socket, buffer, BUFFER_SIZE, 0);
                pid_t pid;
                pid = fork();
                switch (pid) {
                    case -1:
                        perror("fork error");
                        exit(1);
                    case 0:
                        printf("23421412341242134\n");
                        system(COMMAND);
                        break;
                    default:
                        break;
                }
            } else {
                char * p = strtok(torrent_URL, ";");
                while (p != NULL) {
                    bzero(buffer, BUFFER_SIZE);
                    strncpy(buffer, p, strlen(p) > BUFFER_SIZE ? BUFFER_SIZE : strlen(p));
                    buffer[strlen(p)] = '\0';
                    printf("我在发送%s\n", p);
                    send(new_server_socket, buffer, BUFFER_SIZE, 0);
                    p = strtok(NULL, ";");
                    sleep(0.01);
                }
            }
        }
        // 关闭与客户端的连接
        close(new_server_socket);
    }
    // 关闭监听用的socket
    close(server_socket);
    return 0;
} // main
