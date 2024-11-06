#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define PORT 2201
#define PORT2 2202 //Need to define a port for client 2
#define BUFFER_SIZE 1024

long file_size = 0;

int main(){
    //Setting up the server
    int listen_fd1, conn_fd1, listen_fd2, conn_fd2; //Creating the listening and connecting file descriptors
    struct sockaddr_in address1, address2; 
    
    int addrlen1 = sizeof(address1); //Length of address 1
    int addrlen2 = sizeof(address2); //Length of address 2
    char buffer[BUFFER_SIZE] = {0};

    //Create the listen socket
    if((listen_fd1 = socket(AF_INET, SOCK_STREAM, 0)) == 0){ //so if socket is not created, print fail
        perror("socket 1 failed");
        exit(EXIT_FAILURE);
    }
    if((listen_fd2 = socket(AF_INET, SOCK_STREAM, 0)) == 0){ //so if socket is not created, print fail
        perror("socket 2 failed");
        exit(EXIT_FAILURE);
    }

    


    // Bind socket to port
    address1.sin_family = AF_INET; //The address family -> indicates that it will follow IPv4 Protocols
    address1.sin_addr.s_addr = INADDR_ANY; //Bind to any available 
    address1.sin_port = htons(PORT); //Port Number
    if (bind(listen_fd1, (struct sockaddr *)&address1, addrlen1) < 0)
    {
        perror("[Server] bind() failed.");
        exit(EXIT_FAILURE);
    }
    
    address2.sin_family = AF_INET;
    address2.sin_addr.s_addr = INADDR_ANY;
    address2.sin_port = htons(PORT2);
    if (bind(listen_fd2, (struct sockaddr *)&address2, addrlen2) < 0)
    {
        perror("[Server] bind() failed for client 2.");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(listen_fd1, 3) < 0)
    {
        perror("[Server] listen() failed.");
        exit(EXIT_FAILURE);
    }

    printf("[Server] Running on port %d\n", PORT);


    if (listen(listen_fd2, 3) < 0)
    {
        perror("[Server] listen() failed for client 2.");
        exit(EXIT_FAILURE);
    }

    printf("[Server] Running on port %d\n", PORT2);

    // Accept incoming connection
    if ((conn_fd1 = accept(listen_fd1, (struct sockaddr *)&address1, (socklen_t *)&addrlen1)) < 0)
    {
        perror("[Server] accept() failed.");
        exit(EXIT_FAILURE);
    }

    printf("[Server] accept client 1 success");

    if ((conn_fd2 = accept(listen_fd2, (struct sockaddr *)&address2, (socklen_t *)&addrlen2)) < 0)
    {
        perror("[Server] accept() failed for client 2.");
        exit(EXIT_FAILURE);
    }

    printf("[Server] accept client 2 success");

    // End Server Setup

    //NEED TO IMPLEMENT GAME LOGIC HERE

    close(conn_fd1);
    close(conn_fd2);
    close(listen_fd1);
    close(listen_fd2);

    printf("[Server] connection ends");
}