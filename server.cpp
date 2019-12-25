//server.cpp
//Charles Bao
//currently works as a client->server->client chat for 2 clients
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

struct sockaddr_in sendSock, server;
int sockfd;
struct users* userInfo;
void initServer(int argc, char** argv);
char* getData(std::ifstream &in);
void sendMessage(std::string message, struct sockaddr_in recipient);
std::string readMessage();
void messageRouter(struct users& userInfo);
struct users* initUsers();
void deleteUsers();
bool compareSock(struct sockaddr_in user1, struct sockaddr_in user2);
void sendOtherUser(std::string message, struct users* userInfo);

struct user{
    struct sockaddr_in clientSocket;
};//just stores user data

struct users{
    struct user* user1;
    struct user* user2;
};//temp for testing

void initServer(int argc, char** argv){
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

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	    std::cerr << "Cannot create socket\n";
	    exit(0);
    }//check to see if socket was properly created

    memset(&sendSock, '\0', sizeof(sendSock));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(addressData);
    delete [] addressData;

    if ( bind(sockfd, (const struct sockaddr *) &server, sizeof(server)) < 0 ){ 
        std::cerr << "Cannot bind socket\n";
	    exit(0);
    }//check to see if the socket was properly binded
}

char* getData(std::ifstream &in){
    std::string data;
    getline(in,data);
    data = data.substr(15);
    int n = data.length() + 1; //convert from a string to char array
    char* charArr = new char[n];
    strcpy(charArr, data.c_str());
    return charArr;
}//takes in data from the file

void sendMessage(std::string message, struct sockaddr_in recipient){
    char buffer[1024];
    strcpy(buffer, message.c_str());
    sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &recipient, sizeof(recipient));
}//sends a message

std::string readMessage(){
    char buffer[1024];
    socklen_t addr_size = sizeof(sendSock);
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &sendSock, &addr_size);
    std::string message(buffer);
    return message;
}//reads a message

void messageRouter(struct users* userInfo){
    bool running = true;
    while(running){
        std::string message = readMessage();
        char IP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sendSock.sin_addr), IP, INET_ADDRSTRLEN);
        std::cout << "data recieved: " << IP << "-" << sendSock.sin_port << ": " << message << std::endl;
        if(message == "end"){
                running = false;
                break;
            }
        if(message == "/login"){
            if(userInfo->user1 == NULL){
                userInfo->user1 = new user{sendSock};
                sendMessage("Connected to server", sendSock);
            }
            else if(userInfo->user2 == NULL){
                userInfo->user2 = new user{sendSock};
                sendMessage("Connected to server", sendSock);
            }
            else{
                sendMessage("There are already 2 people chatting",sendSock);
            }
        }
        else if(userInfo->user1 != NULL && userInfo->user2 != NULL){
            sendOtherUser(message, userInfo);
        }
    }
}//redirects all the messages to the other person 

struct users* initUsers(){
    struct users* userInfo= new users{NULL,NULL};
    return userInfo;
}//creates struct containing user info

void deleteUsers(struct users* userInfo){
    delete userInfo->user1;
    delete userInfo->user2;
    delete userInfo;
}//deletes all user info

bool compareSock(struct sockaddr_in user1, struct sockaddr_in user2){
    return (user1.sin_addr.s_addr == user2.sin_addr.s_addr && user1.sin_port == user2.sin_port);
}

void sendOtherUser(std::string message, struct users* userInfo){
    struct sockaddr_in user1 = userInfo->user1->clientSocket;
    struct sockaddr_in user2 = userInfo->user2->clientSocket;
    if(compareSock(sendSock,user1)){
        sendMessage(message,user2);
    }
    else if(compareSock(sendSock,user2)){
        sendMessage(message,user1);
    }
}//sends the message to the other user (temp for testing)

int main(int argc, char** argv){
    initServer(argc,argv);
    userInfo = initUsers();
    messageRouter(userInfo);
    deleteUsers(userInfo);
    return 0;
}