#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 256
#define BACKLOG 3
#define NO_SOCK 0

/*
ECEN 602 - Computer Communications and Networks
Assignment 1
Echo Server code

Guru Sarath Thangamni - 829009551
Doyoung Kwak - 927000467

*/

void __attribute__((destructor)) CloseSocket();
int writen(int port, char c[], int len);
int readline();

int Socket_Global = NO_SOCK; // This global variable is used to close a socket

int main(int argc, char* argv[])
{
    int portNum, pid;

    puts("\n\n#### Echo Server Application ####\n");

	// Check if port number is passed from command line
    if(argc < 2)
    {
        printf("Insufficient arguments !\nPort nmber required\n");
        return 1;
    } else {
		
        portNum = atoi(argv[1]);
    }

    struct sockaddr_in ServerAddr_in, clientX;
    int sd, sd_connection;
    struct addrinfo hints, *serverinfo, *servP;
    char receiveBuffer[MAX_BUFFER_SIZE];
    int rcv_len = 0;
	
	// Set all memory locations to zero
    memset(&ServerAddr_in, 0, sizeof(ServerAddr_in));

    ServerAddr_in.sin_family = AF_INET; //IPv4
    ServerAddr_in.sin_addr.s_addr = INADDR_ANY;
    ServerAddr_in.sin_port = htons( portNum ); // Host to network order conversion


    if( (sd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) // Create socket
    {
        puts("Could not create socket");
        return 1;
    } else {

        Socket_Global = sd;

    }

    if(bind(sd, (struct sockaddr *)&ServerAddr_in, sizeof(ServerAddr_in)) < 0) // Bind to socket
    {
        puts("Bind action failed !");
        return 1;
    }



    // This loop listens for new connections
    puts("Listening ... ... ... ");
    do
    {

        if(listen(sd, BACKLOG) < 0)
        {
            puts("Listen Error !");
            return 1;
        }

        int sizeX = sizeof(struct sockaddr_in);
        sd_connection = accept(sd, (struct sockaddr *)&clientX, (socklen_t*)&sizeX);

        if(sd_connection < 0)
        {
            puts("Connection Failed !");
            return 1;
        }

        // Make a system call to create a child process
        pid = fork();

        if(pid > 0)
        {
            // Executes in parent process.
            printf("New Child Process created SD = %d\n", sd_connection);

        } else if (pid < 0){
            // error
            puts("Failed to create new process");
        }

    } while (pid > 0); // Exit loop only in child process



    /* Code Below is only executed in a child process */

    puts("New Client Connected !");
    Socket_Global = sd_connection;
    while(1)
    {
        //rcv_len = recv(sd_connection, receiveBuffer, sizeof(receiveBuffer), 0);
		rcv_len = readline(sd_connection, receiveBuffer, sizeof(receiveBuffer));

        // If connection is terminated by the client
        if(rcv_len == 0)
        {
            // Destructor function is called before return
            // Destructor function closes the socket
            break;

        } else if(rcv_len > 0) { // If data was received from client

            //int sentLen = send(sd_connection, receiveBuffer, rcv_len, 0);

            int sentLen = writen(sd_connection, receiveBuffer, rcv_len);

			// Send until all data is sent
            while(sentLen < rcv_len)
				sentLen = writen(sd_connection, receiveBuffer, rcv_len);
        }

    }

    return 0;

}

void __attribute__((destructor)) CloseSocket()
{
    if (Socket_Global != NO_SOCK)
    {
        close(Socket_Global);
        printf("Socket %d closed\n", Socket_Global);
    }
}

int writen(int sock_d, char Buff[], int len)
{
    int n;
    n = send(sock_d, Buff, len, 0);
    return n;
}

int readline(int sock_d, char Buff[], int len)
{
	int n;
	n = recv(sock_d, Buff, len, 0);
	return n;
}