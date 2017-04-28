#include <netinet/in.h> // for sockaddr_in
#include <sys/types.h>  // for socket
#include <sys/socket.h> // for socket
#include <stdio.h>      // for printf
#include <stdlib.h>     // for exit
#include <string.h>     // for bzero
#include <arpa/inet.h>  // for inet_aton
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define HELLO_WORLD_SERVER_PORT 6666
#define BUFFER_SIZE             1024
#define TORRENT_URL_MAX         512
int flag         = 1;
char mess[20000] = "23";
int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("Usage: ./%s ServerIPAddress\n", argv[0]);
        exit(1);
    }

    char Information[TORRENT_URL_MAX + 1];
    bzero(Information, TORRENT_URL_MAX + 1);
    printf("Please Input File Name On Server:\t");
    scanf("%s", Information);
    int size = 0;
    time_t ts, te;

    // 设置一个socket地址结构client_addr,代表客户机internet地址, 端口
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr) ); // 把一段内存区的内容全部设置为0
    client_addr.sin_family      = AF_INET;     // internet协议族
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);// INADDR_ANY表示自动获取本机地址
    client_addr.sin_port        = htons(0); // 0表示让系统自动分配一个空闲端口

    while (flag) {
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (client_socket < 0) {
            printf("Create Socket Failed!\n");
            exit(1);
        }

        // 把客户机的socket和客户机的socket地址结构联系起来
        if (bind(client_socket, (struct sockaddr *) &client_addr, sizeof(client_addr) ) ) {
            printf("Client Bind Port Failed!\n");
            exit(1);
        }

        // 设置一个socket地址结构server_addr,代表服务器的internet地址, 端口
        struct sockaddr_in server_addr;
        bzero(&server_addr, sizeof(server_addr) );
        server_addr.sin_family = AF_INET;

        if (inet_aton(argv[1], &server_addr.sin_addr) == 0) {
            printf("Server IP Address Error!\n");
            exit(1);
        }

        server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
        socklen_t server_addr_length = sizeof(server_addr);

        // 向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
        if (connect(client_socket, (struct sockaddr *) &server_addr, server_addr_length) < 0) {
            printf("Can Not Connect To %s!\n", argv[1]);
            exit(1);
        }

        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        strncpy(buffer, Information, strlen(Information) > BUFFER_SIZE ? BUFFER_SIZE : strlen(Information) );

        // 向服务器发送buffer中的数据
        if (strstr(mess, "end") == NULL)
            send(client_socket, buffer, BUFFER_SIZE, 0);

        char Torrent_URL[TORRENT_URL_MAX + 1];

        int length = 0;
        bzero(buffer, BUFFER_SIZE);
        int n = 0;

        while (length = recv(client_socket, buffer, BUFFER_SIZE, 0) ) {
            if (length < 0) {
                printf("Server Recieve Data Failed!\n");
                break;
            } else   {
                bzero(Torrent_URL, TORRENT_URL_MAX + 1);
                strncpy(Torrent_URL, buffer, strlen(buffer) > TORRENT_URL_MAX ? TORRENT_URL_MAX : strlen(buffer) );
                // strcat(Torrent_URL,"\0");
                printf("%s_%d\n", Torrent_URL, strcmp(Torrent_URL, "end") );
                n++;

                if (!strcmp(Torrent_URL, "正在制作种子...") ) {
                    printf("服务器正在制作种子...\n");
                    break;
                }

                if (!strcmp(Torrent_URL, "end") ) {
                    flag = 0;
                }

                if (strstr(mess, Torrent_URL) == NULL) {
                    strcat(mess, Torrent_URL);
                    size++;
                    char COMMAND[100];
                    strcpy(COMMAND, "./Client_BT ");
                    strcat(COMMAND, buffer);
                    pid_t pid;
                    pid = fork();

                    switch (pid) {
                        case -1:
                            perror("fork error");
                            exit(1);
                        case 0:
                            system(COMMAND);
                            break;
                        default:
                            break;
                    }
                }
            }

            bzero(buffer, BUFFER_SIZE);
            usleep(100000);
        }

        close(client_socket);
        time_t ts, te;
        ts = time(NULL);
        te = time(NULL);

        while (te - ts < 10) {
            te = time(NULL);
        }
    }

    return 0;
} // main
