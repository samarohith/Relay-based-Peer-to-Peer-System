#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define buffer_size 1024

int main(int argc, char *argv[])
{
 	int i = 0, sockfd;
 	char buffer[buffer_size];
	if(argc != 2)
	{
		printf("\nInput format: %s <Server port number> , terminating! \n", argv[0]);       // invalid input by user
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);       // socket file descriptor for server
	if(sockfd < 0)
	{ 						 // failed to get valid socket file descriptor
		perror("Socket error, terminating! \n");
		return -1;
	}

	struct sockaddr_in serv_addr;		   	// initializing server address attributes
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Unable to bind, terminating!\n");		//bind failed
		return -1;
	}

	printf("Server established with server port number = %d\n\n",ntohs(serv_addr.sin_port));    // server running

	listen(sockfd,5); 		// accepting connections, backlog=5

	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	int peer_count = 0;
	int peer_ids[10];
	struct sockaddr_in peer_addresses[10];
	

	for (i = 0; i >= 0; i++)     // infinite loop, server doesn't close unless program terminated by user or by error
	{ 
	
		memset(buffer,'\0',sizeof(buffer));

		int client_id = accept(sockfd, (struct sockaddr *)&client_addr , (socklen_t *)&client_addr_len);   // clients that are requesting connection with the server

		if(client_id < 0)
		{
			perror("Accept error, couldn't accept connection\n");
			return -1;
		}
		printf("Connection accepted \n");

		if(recv(client_id,buffer,buffer_size,0) < 0)     // message received from client
		{
			perror("Message not received by server\n");
			return -1;
		}
		
		char *type = strtok(buffer, "#");
		char *temp = strtok(NULL,"#");
		printf("Message: %s \n",temp);
		temp = strtok(NULL,"#");
		
		int portno;
		if(temp != NULL)
			portno = atoi(temp);

		char msg[] = "Hi there! This is the server.";	
		int msg_len = strlen(msg);	


		if(*type == '0')
		{
			// Client
			char ip_addr[buffer_size];

			printf("Peer client port: %d\n",ntohs(client_addr.sin_port));
			inet_ntop(AF_INET, &(client_addr.sin_addr), ip_addr, buffer_size);
			printf("Peer client IP: %s\n",ip_addr);

			sprintf(msg,"%s$%d",msg,peer_count);
			printf("Number of peer nodes = %d\n",peer_count);

			for(i = 0; i < peer_count; i++)    // fetching details of all peer nodes
			{
				inet_ntop(AF_INET, &(peer_addresses[i].sin_addr), ip_addr, buffer_size);
				sprintf(msg,"%s:%s:%d",msg,ip_addr,peer_addresses[i].sin_port);
			}

			//sending details of all the peer nodes to the peer client
			if(send(client_id, msg , strlen(msg), 0) != strlen(msg))
			{
				perror("Message not sent\n");
				exit(-1);
			}
		}
		else if(*type == '1')
		{
			// Peer node
			peer_ids[peer_count] = client_id;
			peer_addresses[peer_count].sin_family = client_addr.sin_family;
			peer_addresses[peer_count].sin_port = portno;
			peer_addresses[peer_count].sin_addr.s_addr = client_addr.sin_addr.s_addr;

			printf("Peer node port: %d\n",portno);
			printf("Peer node IP: %s\n",inet_ntop(AF_INET, &(client_addr.sin_addr), temp, buffer_size));

			// server sends connection confirmation
			if(send(client_id,msg,msg_len,0) != msg_len)
			{
				perror("Message not sent from server\n");
				return -1;
			}

			peer_count++;
		}

		else
		{
			perror("Invalid Client request\n");  // neither peer node nor peer client
			return -1;
		}

		printf("\n");
	}

	close(sockfd); //close socket
}

