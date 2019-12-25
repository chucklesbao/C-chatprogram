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
int sockfd;
char* getData(std::ifstream &in);
void reader();
std::string readMessage();
void sender();
void sendMessage(std::string message, struct sockaddr_in recipient);
void initClient(int argc, char** argv);

char* getData(std::ifstream &in){
    std::string data;
    getline(in,data);
    data = data.substr(15);
    int n = data.length() + 1; //convert from a string to char array
    char* charArr = new char[n];
    strcpy(charArr, data.c_str());
    return charArr;
}//takes in data from the file

void receiver(){
    bool running = true;
    std::string message;
    while(running){
        message = readMessage();
        std::cout << "data recieved: " << message << std::endl;
        if(message == "end"){
            running = false;
            break;
        }
    }
}//constantly checks for new data being sent to client

std::string readMessage(){
    char buffer[1024];
    socklen_t addr_size = sizeof(server);
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &server, &addr_size);
    std::string message(buffer);
    return message;
}//reads a message

void sender(){
    bool running = true; 
    std::string message;
    while(running){
        std::getline(std::cin,message);
        sendMessage(message, server);
        if(message == "end"){
            running = false;
            break;
        }
        std::cout << "Data sent: "<< message <<std::endl;
    }
}//sends out new data

void sendMessage(std::string message, struct sockaddr_in recipient){
    char buffer[1024];
    strcpy(buffer, message.c_str());
    sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &recipient, sizeof(recipient));
}

void initClient(int argc, char** argv){
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
    char* portDataFF = getData(in);
    char* addressDataFF = getData(in);

    int port = atoi(portDataFF);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	    std::cerr << "Cannot create socket\n";
	    exit(0);
    }//check to see if socket was properly created

    memset(&server, '\0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(addressDataFF);
    delete [] portDataFF;
    delete [] addressDataFF;
}//initializes client socket and data

int main(int argc, char** argv){
    initClient(argc,argv);

    sendMessage("/login", server);
    
    std::thread rfs(receiver);
    rfs.detach();
    std::thread sts(sender);
    sts.join();
    
    return 0;
}