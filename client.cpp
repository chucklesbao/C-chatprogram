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
#include <cstdlib>
#include <fstream>
#include <string>
#include <thread>

char* getData(std::ifstream &in){
    std::string data;
    getline(in,data);
    data = data.substr(15);
    int n = data.length() + 1; //convert from a strint to char array
    char* charArr = new char[n];
    strcpy(charArr, data.c_str());
    return charArr;
}

int main(int argc, char** argv){
    if(argc != 2){
        std::cerr << "Usage:" << argv[0] << " ServerInfo.txt\n";
        exit(0);
    }//check to make sure the server info file was passed to main

    std::ifstream in;

    in.open(argv[1]);
    if(!in){
        std::cerr << "Error opening the file\n";
        exit(0);
    }//check to see if the file can be opened
    char* portData = getData(in);
    char* addressData = getData(in);
    
    int port = atoi(portData);
    delete [] portData;
    int sockfd;
    struct sockaddr_in client, server;
    char buffer[1024];
    socklen_t addr_size;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	    std::cerr << "cannot create socket\n";
	    exit(0);
    }//check to see if socket was properly created

    memset(&client, '\0', sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = inet_addr(addresData);
    delete [] portData;

    if ( bind(sockfd, (const struct sockaddr *) &client, sizeof(client)) < 0 ){ 
        std::cerr << "cannot bind socket\n";
	    exit(0);
    }//check to see if the socket was properly binded

    addr_size = sizeof(server);
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &server, &addr_size);
    std::cout << "Data recieved: " << buffer << std::endl;

    return 0;
}