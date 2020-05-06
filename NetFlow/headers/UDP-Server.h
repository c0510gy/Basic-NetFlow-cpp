#ifndef UDP_SERVER
#define UDP_SERVER

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

class UDPserver{
private:
    const int PORT, BUFSIZE;

    int sockfd;
    struct sockaddr_in servAddr;
    struct sockaddr_in clntAddr;
public:
    UDPserver(int port, int bufsize);
    std::string recv();
};
UDPserver::UDPserver(int port, int bufsize): PORT(port), BUFSIZE(bufsize){
    /* 인터넷으로 연결된 프로세스들 간에 통신을 하고 UDP 방법을 이용하는 소켓을 생성 */
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror("sock failed");
        exit(1);
    }
    
    /* servAddr를 0으로 초기화 */
    memset(&servAddr, 0, sizeof(servAddr));
    /* servAddr에 IP 주소와 포트 번호를 저장 */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(PORT);

    /* sockfd 소켓에 주소 정보 연결 */
    if(bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1){
        perror("bind failed");
        exit(1);
    }
}
std::string UDPserver::recv(){
    char recvBuffer[BUFSIZE];
    int clntLen;
    int recvLen;

    clntLen = sizeof(clntAddr);
    /* sockfd 소켓으로 들어오는 데이터를 받아 recvBuffer에 저장하고
        클라이언트 주소 정보를 clntAddr에 저장 */
    if((recvLen = recvfrom(sockfd, recvBuffer, BUFSIZE-1, 0, (struct sockaddr*)&clntAddr, (socklen_t*)&clntLen)) == -1){
        perror("recvfrom failed");
        exit(1);
    }
    recvBuffer[recvLen] = '\0';
    return recvBuffer;
}

#endif