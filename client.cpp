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

struct sockaddr_in server;

char* getData(std::ifstream &in){
    std::string data;
    getline(in,data);
    data = data.substr(15);
    int n = data.length() + 1; //convert from a strint to char array
    char* charArr = new char[n];
    strcpy(charArr, data.c_str());
    return charArr;
}

void receiver(int sockfd){
    bool running = true;
    char buffer[1024];
    socklen_t addr_size;
    while(true){
        addr_size = sizeof(server);
        recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)& server, &addr_size);
        std::cout << "Data recieved: " << buffer << std::endl;
        if(std::strcmp(buffer,"end")==0){
            running = false;
            break;
        }
    }
}

void sender(int sockfd){
    bool running = true;
    char buffer[1024];
    std::string line;
    while(true){
        std::getline(std::cin,line);
        strcpy(buffer, line.c_str());
        sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server, sizeof(server));
        if(line == "end"){
            running = false;
            break;
        }
        std::cout << "Data sent: "<< buffer <<std::endl;
    }
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
    

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	    std::cerr << "Cannot create socket\n";
	    exit(0);
    }//check to see if socket was properly created

    memset(&server, '\0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(addressData);
    delete [] addressData;
    
    std::thread rfs(receiver, sockfd);
    rfs.detach();
    std::thread sts(sender, sockfd);
    sts.join();
    
    return 0;
}