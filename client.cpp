//client.cpp
//Charles Bao
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>

int main(int argc, char** argv){
    if(argc != 2){
        std::cerr << "Usage:" << argv[0] << "<port>\n";
        exit(0);
    }
    int port = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in client, server;
    char buffer[1024];
    socklen_t addr_size;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	    std::cerr << "cannot create socket\n";
	    exit(0);
    }

    memset(&client, '\0', sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");

    if ( bind(sockfd, (const struct sockaddr *) &client, sizeof(client)) < 0 ){ 
        std::cerr << "cannot create socket\n";
	    exit(0);
    }
    addr_size = sizeof(server);
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &server, &addr_size);
    std::cout << "Data recieved: " << buffer << std::endl;
    return 0;
}