#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> 

#define buffer_size 1024

void process_file(int, struct sockaddr_in[]);



void process_file(int p_count, struct sockaddr_in p_address[])      // Phase 3
{
    int _z = 0;
    int sockfd, found = _z;
    char buffer[buffer_size], filename[buffer_size];
    
    printf("\nEnter the name of the file: ");
    scanf("%s",filename); // take filename as input from user
    printf("\n");
    
    for(int i = _z; i < p_count; i++)  // search in all peer nodes
    { 
        char ip_addr[buffer_size];
        printf("Peer node number:%d\n",i+1);
        printf("Peer node port: %d\n",ntohs(p_address[i].sin_port));
        printf("Peer node IP: %s\n",inet_ntop(AF_INET, &(p_address[i].sin_addr), ip_addr, buffer_size));
        
        sockfd = socket(AF_INET, SOCK_STREAM, _z);

        if(sockfd < _z)
        {
            printf("\n Socket creation error \n");
            return -1;
        }

        if(connect(sockfd, (struct sockaddr *)&(p_address[i]), sizeof(p_address[i])) < _z)    // Connecting with peer nodes
        {
           printf("\n Couldn't connect to peer node\n");
           return -1;
        }

        printf("Connected to peer node\n");

        if(send(sockfd, filename, strlen(filename),0)!=strlen(filename)) //sending filename to peer node
        {
            perror("Couldn't send filename\n");
            return -1;
        }

        memset(buffer,'\0',buffer_size);

        if(recv(sockfd, buffer, buffer_size, 0) < 0)
        {
            perror("Receive error\n");
            exit(-1);
        }

        long int filesize;

        char *temp = strtok(buffer, "@");
        char *found_in_peer = temp;

        if(*found_in_peer=='0') //File not found
        {
            printf("File not found in peer node number %d\n\n",i+1);
            close(sockfd);
            continue;
        }

        printf("File found in peer node number %d\n\n",i+1);
        found = 1; // found in atleast one peer node

        if(temp != NULL)
        {
            temp = strtok(NULL,"$");
            filesize= atoi(temp);
        }

        long int remaining_bytes = filesize; // number of bytes left to be transferred

        printf("File size = %ld \n",filesize);

        FILE *rcvd_file = fopen(filename,"w");
        int len;

        while(remaining_bytes > _z && (len = recv(sockfd, buffer, buffer_size, _z)) > _z)
        {
            fwrite(buffer, sizeof(char), len, rcvd_file);
            remaining_bytes = remaining_bytes - len;
            printf("Buffer currently contains: %s \n",buffer); //denotes current buffer
            printf("Received = %d bytes, Remaining bytes  = %ld bytes \n",len, remaining_bytes );// show updates on bytes received and left
        }

        printf("File transfer completed\n\n"); // file completely transferred

        fclose(rcvd_file); //close file
        close(sockfd);  //close socket
    
    }

    if(found == _z) // not found in any of the peers
    {
        printf("File not found in all peer_nodes\n");
    }

    return 0;
}






int main(int argc, char *argv[])
{
    int i = 0, sockfd;
    char buffer[buffer_size] = {0};
    
    if(argc != 3)               //Invalid input by user
    {
        printf("\n Input format: %s <Server IP address> <Server port number> \n",argv[0]);
        return -1;
    } 

    sockfd = socket(AF_INET, SOCK_STREAM, 0);    //socket file descriptor for client
    
    if (sockfd < 0)                                  // failed to get valid socket file descriptor
    {
        printf("\n Socket creation error, terminating! \n");
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;                //initializing peer_node_address attributes
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) != 1)
    {
        printf("\n Invalid server IP, terminating!\n");
        return -1;
    }

    if(connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  //connecting with relay sever
    {
       printf("\n Unable to connect to the server, terminating! \n");
       return -1;
    }

    char *msg = "0#Hi there! This is the client." ;
    
    if(send(sockfd, msg, strlen(msg),0) != strlen(msg))   // requesting response from relay server
    {
        perror("Message not sent from client to server\n");
        return -1;
    }

    if(recv(sockfd,buffer,buffer_size,0) < 0)   //response sent from relay server
    {
        perror("Message sent by server, not received by client\n");
        return -1;
    }

    char *temp = strtok(buffer,"$");
    printf("Server says: %s\n\n",temp);
    temp = strtok(NULL,"$");
    
    int peer_count = 0;
    peer_count = atoi(strtok(temp,":"));
    printf("PeerNode Count : %d\n\n",peer_count);
    
    temp = strtok(NULL, ":");

    struct sockaddr_in peer_address[peer_count];  //array to store addresses of peer nodes
    char* peer_ports[peer_count];  //array to store ports of peer nodes
    
    while(temp != NULL)    //extracting peer node addresses and ports from message sent by relay server
    {   
        peer_address[i].sin_family = AF_INET;
        
        if(inet_pton(AF_INET, temp, &(peer_address[i].sin_addr.s_addr)) < 0) 
        {
            printf("Error: Invalid Address\n");
            return -1;
        }
        
        temp = strtok(NULL, ":");
        peer_address[i].sin_port = htons(atoi(temp));
        
        temp = strtok(NULL, ":");
        i++;
    }

    // Details of all peer nodes

    for(i = 0; i < peer_count; i++)
    {   
        char ip_addr[buffer_size];
        printf("Peer IP: %s\n",inet_ntop(AF_INET, &(peer_address[i].sin_addr), ip_addr, buffer_size));
        printf("Peer port: %d\n",ntohs(peer_address[i].sin_port));
    }

    close(sockfd); 
    
    process_file(peer_count, peer_address);
    
    return 1;
    
}


