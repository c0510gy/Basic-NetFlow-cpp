#ifndef UDP_CLIENT
#define UDP_CLIENT

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
using namespace std;

class UDPclient{
private:
    const int PORT, BUFSIZE;

    int sockfd;
    struct sockaddr_in servAddr;
    int recvLen, servLen;
public:
    UDPclient(string addrss, int port, int bufsize);
    ~UDPclient();
    void send(string message);
    string recv();
};

UDPclient::UDPclient(string addrss, int port, int bufsize): PORT(port), BUFSIZE(bufsize){
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror("sock failed");
        exit(1);
    }

    memset(&servAddr, 0, sizeof(servAddr));
    /* servAddr에 IP 주소와 포트 번호를 저장 */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(addrss.c_str());
    servAddr.sin_port = htons(PORT);
}
UDPclient::~UDPclient(){
    close(sockfd);
}
void UDPclient::send(string message){
    char sendBuffer[BUFSIZE];

    for(int j = 0; j < message.size(); ++j)
        sendBuffer[j] = message[j];
    sendBuffer[message.size()] = '\0';

    if(sendto(sockfd, sendBuffer, strlen(sendBuffer), 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) != strlen(sendBuffer)){
        perror("sendto failed");
        exit(1);
    }
}
string UDPclient::recv(){
    char recvBuffer[BUFSIZE];

    servLen = sizeof(servLen);
    /* sockfd 소켓으로 들어오는 데이터를 받아 recvBuffer에 저장 */
    if((recvLen=recvfrom(sockfd, recvBuffer, BUFSIZE-1, 0, (struct sockaddr*)&servAddr, (socklen_t*)&servLen)) != strlen(recvBuffer)){
        perror("recvfrom failed");
        exit(1);
    }
    
    recvBuffer[recvLen] = '\0';
    return recvBuffer;
}

#endif