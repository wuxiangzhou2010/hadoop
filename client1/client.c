#include <stdio.h>

#define BUFFER_SIZE (2048)
#define TORRENT_URL_MAX (512)

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Usage: ./%s Server IP address\n",argv[0] );
		exit(1);
	}

	char Information[TORRENT_URL_MAX +1];
	bzero(Information, TORRENT_URL_MAX +1);
	printf("Please input file name on server:\t\n");

	scanf("%s", Information);

	int flag = 1;
	struct sockaddr_in client_addr;
	bzero(&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htons(INADDR_ANY);
	client_addr.sin_port = htons(0);
	while(flag)
	{
		int client_socket = socket(PF_INET,  SOCK_STREAM, 0 );
		if(client_socket < 0 )
		{
			printf("Create socket Failed\n");
			exit(2);
		}
		if(bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
		{
			printf("Client bind port failed\n");
			exit(2);
		}
		struct sockaddr_in server_addr;
		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		if(inet_aton(argv[1], &server_addr.sin_addr) ==0)
		{
			printf("server IP error");
			exit(6);
		}
		server_addr.sin_port = htons(6666);
		socklen_t server_addr_length = sizeof(server_addr);
		if(connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length)<0)
		{
			printf("Can not connect to server %s\n", argv[1]);
			exit(7);
		}
		char buffer[BUFFER_SIZE];
		bzero(buffer, BUFFER_SIZE);
		strncpy(buffer, Information, strlen(Information)>BUFFER_SIZE ? BUFFER_SIZE : strlen(Information));
		if(strstr(mess, "end") == NULL)
			send(client_socket, buffer, BUFFER_SIZE,0);
		char TORRENT_URL[TORRENT_URL_MAX +1];
		int length = 0;
		bzero(buffer, BUFFER_SIZE);
		int n = 0;
		while(length = recv(client_socket,buffer, BUFFER_SIZE,0))
		{
			if(length < 0)
			{
				printf("Server Receive data failed\n");
				break;
			}
			else
			{
				bzero(TORRENT_URL, TORRENT_URL_MAX +1);
				strncpy(Torrent_URL, buffer, strlen(buffer)>TORRENT_URL_MAX ? TORRENT_URL_MAX : strlen(buffer));
				printf("%s_%d\n", Torrent_URL, strcmp(Torrent_URL, "end"));
			}
			n++;
			if(!strcmp(Torrent_URL, "making torrent..."))
			{
				printf("server is making torrent\n");
				break;
			}
			if(!strcmp(Torrent_URL, "end"))
			{
				flag = 0;
			}
			if(strstr(mess, Torrent_URL) == NULL)
			{
				strcat(mess, Torrent_URL);
				size++;
				char COMMAND[100];
				strcpy(COMMAND, "./CLIENT_BT ");
				strcat(COMMAND, buffer);
				pid_t pid;
				pid = fork();
				switch(pid)
				{
				case -1:
					perror("fork error");
					exit(7);
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
	ts=time(NULL);
	te = time(NULL);
	while(te-ts<10)
		te = time(NULL);
}