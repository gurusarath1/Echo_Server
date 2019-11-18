#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<stdlib.h>

#define MAX_LINE 256

/*
ECEN 602 - Computer Communications and Networks
Assignment 1
Echo Client code

Doyoung Kwak - 927000467
Guru Sarath Thangamni - 829009551

*/

int main(int argc , char *argv[])
{
	int socket_desc;
	struct sockaddr_in server;
	char *host;
	char buf[MAX_LINE];
	// int s;
	int len;
	// char *message , server_reply[2000];
	char server_reply[2000];
	
	if (argc==3){
	host = argv[1];
	}
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	server.sin_addr.s_addr = inet_addr(host);
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2]));

	//Connect to remote server
	if ( connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	
	puts("Connected\n");
	
	//Send some data
	// message = "GET / HTTP/1.1\r\n\r\n"
	while (fgets(buf, sizeof(buf), stdin)){
	buf[MAX_LINE-1] = '\0';
	len = strlen(buf) + 1;
	if( send(socket_desc, buf , len , 0) < 0)
	{
		puts("Send failed");
		return 1;
	}

	if( recv(socket_desc, server_reply , 2000 , 0) < 0)
	{
		puts("recv failed");
	}

	puts(server_reply);
	}
	
	return 0;
}
