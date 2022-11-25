
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <arpa/inet.h> 
#include <fcntl.h>

#define max_buffer_size 1024

 // Requesting response from relay server function ----------
void request(char *argv[],int socket_id){
    char  message[] = "1#Hi there! This is peer node." ;
    sprintf(message,"%s#%d",message,atoi(argv[3]));
    int message_len = strlen(message);
    
    if( send(socket_id, message, message_len,0)!= message_len)              //sending message from peer node to server
    {
	perror("Message not sent from peer node to server\n");
	return -1;
    }
}

int main(int argc, char *argv[])
{
    int _a = 4;
    if(argc != _a)           //Invalid input by user
    {
        printf("\nInput format: %s <Server IP peer_node_address> <Server port number> <Peer port number>  \n",argv[0]);
        return -1;
    } 

    int socket_id = socket(AF_INET, SOCK_STREAM, 0);    //socket file descriptor for peer node
    int _z = 0;
    if ( socket_id < _z)                                 // failed to get valid socket file descriptor
    {
        perror("\n Socket creation error for peer node, terminating! \n");
        return -1;
    }

    struct sockaddr_in server_address,peer_node_address;

    server_address.sin_family = AF_INET;                     //initializing peer_node_address attributes
    
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    
    server_address.sin_port = htons(atoi(argv[2]));

    char buffer[max_buffer_size]={_z};

    if(inet_pton(AF_INET, argv[1], &server_address.sin_addr)<=_z)
    {
        perror("\n Invalid server IP peer_node_address , terminating! \n"); //invalid IP peer_node_address provided by user  
        return -1;
    }
    
    if( connect(socket_id, (struct sockaddr *)&server_address, sizeof(server_address)) < _z)
    {
       perror("\n Connection of peer node to server failed, terminating! \n");
       return -1;
    }

    // Requesting response from relay server
    
     request(argv,socket_id);

    // Response received from relay sever---
    
    if(recv(socket_id,buffer,max_buffer_size,_z) < _z){
            perror("Message sent by server, not received by peer node\n");
      	    return -1;
        }
        
    printf("Server says: %s\n",buffer);

    // Close connection with server
    close(socket_id);

    //Phase three, accepting connections from Peer_Clients
    peer_node_address.sin_family = AF_INET;
    peer_node_address.sin_addr.s_addr = htonl(INADDR_ANY);
    peer_node_address.sin_port = htons(atoi(argv[3]));
    
    if ((socket_id = socket(AF_INET, SOCK_STREAM, _z)) < _z)
    {
        perror("\n Socket creation error for peer node in Phase three \n");
        return -1;
    }

    if(bind(socket_id, (struct sockaddr *)&peer_node_address, sizeof(peer_node_address)) < _z){
        perror("Unable to bind peer node in Phase three!");
        return -1;
    }

    listen(socket_id,10);                   //peer node waiting for connection request from peer client
    printf("Port number of peer node: %d\n", ntohs(peer_node_address.sin_port));
    
    int peer_node_address_len = sizeof(peer_node_address);
   
    for (int i=_z;i>=_z;i++){ //infinite loop, peer node doesn't close unless program terminated by user or by error
            
        char filename[max_buffer_size]={_z}; //reinitialize filename buffer for every request

        struct sockaddr_in client_address;
        int client_address_len = sizeof(client_address);
        int client_id;

        //accept connection request from peer client
        if((client_id=accept(socket_id,  (struct sockaddr *)&client_address , (socklen_t *)&client_address_len)) < _z)
        {
            perror("Client connection request couldn't be accepted\n"); 
            return -1;
        }

        //receive filename that peer client is requesting
        if(recv(client_id,filename,max_buffer_size,_z) < _z)
        {
            perror("Filename couldn't be received\n");
            return -1;
        }

        printf("Client server is requesting for file with filename = %s\n", filename);
        int file_descriptor =open(filename,O_RDONLY);

        char f_message[max_buffer_size];

        // File not found

        if(file_descriptor==-1)
        {
            sprintf(f_message, "0@");
            int f_messagelen = strlen(f_message);
            printf("File not found\n");

            //intimate the peer client that file was not found
            if(send(client_id,f_message,f_messagelen,_z) != f_messagelen){
                perror("Message not sent\n");
                return -1;
            }

            //move to next peer node
            continue;
        }

        // File found, sending file
        struct stat file_stats;

        if (fstat(file_descriptor, &file_stats) < _z)
        {
                fprintf(stderr, "Error in obtaining file stats : %s", strerror(errno)); //error in obtaining file stats

                exit(EXIT_FAILURE);
        }

        fprintf(stdout, "File size: %ld bytes\n", file_stats.st_size);

        sprintf(f_message, "1@%ld",file_stats.st_size);
        int f_message_len = strlen(f_message);
        int p = send(client_id,f_message,f_message_len,MSG_NOSIGNAL);               //sending file stats to peer client
        
        if(p != f_message_len){
            printf("File stats not sent!!\n");
            return -1;
        }
        
        off_t offset = _z;
        long int sent_bytes=_z,remain_data = file_stats.st_size;
        
        // Sending file data
        while (((sent_bytes = sendfile(client_id, file_descriptor, &offset, BUFSIZ)) > _z) && (remain_data > _z))
        {   
            remain_data -= sent_bytes;
            fprintf(stdout, "Server sent %ld bytes from file's data, offset = %ld and remaining data = %ld\n", sent_bytes, offset, remain_data);
        }
        printf("File transfer completed....\n\n");
    }

    close(socket_id); //close socket
    
    return 0;
}
