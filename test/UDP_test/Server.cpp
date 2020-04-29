/* original - https://molrayo.tistory.com/entry/Network-UDP-%EC%98%88%EC%A0%9C-sendtorecvfrom */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 7777   /* 포트 번호 */
#define BUFSIZE 1024

int main(){
    int sockfd;
    struct sockaddr_in servAddr;
    struct sockaddr_in clntAddr;
    char recvBuffer[BUFSIZE];
    int clntLen;
    int recvLen;

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

    /* 무한 반복 */
    while(1){
        clntLen = sizeof(clntAddr);
        /* sockfd 소켓으로 들어오는 데이터를 받아 recvBuffer에 저장하고
            클라이언트 주소 정보를 clntAddr에 저장 */
        if((recvLen = recvfrom(sockfd, recvBuffer, BUFSIZE-1, 0, (struct sockaddr*)&clntAddr, (socklen_t*)&clntLen)) == -1){
            perror("recvfrom failed");
            exit(1);
        }
        recvBuffer[recvLen] = '\0';
        /* 받은 데이터를 출력 */
        printf("Recevied: %s\n", recvBuffer);
        
        /* 받은 데이터를 클라이언트에게 보냄 */
        if(sendto(sockfd, recvBuffer, recvLen, 0, (struct sockaddr*)&clntAddr, sizeof(clntAddr)) != recvLen){
            perror("sendto failed");
            exit(1);
        }
    }
}