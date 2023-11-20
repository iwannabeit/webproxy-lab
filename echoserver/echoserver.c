#include "csapp.h"

void echo(int connfd);

int main(int argc, char **argv)
{
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  char client_hostname[MAXLINE], client_port[MAXLINE];

  if(argc != 2){
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }
  //port번호가 인자, bind, listen 준 서버소켓 파일디스크립터
  listenfd = Open_listenfd(argv[1]);

  while(1){
    clientlen = sizeof(struct sockaddr_storage); //클라이언트 주소 정보 저장 변수
    //클라이언트 연결 수락 clientaddr에 클라이언트 주소 정보 저장 len은 주소 정보 크기
    connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
    //클라이언트 주소 정보를 사람이 읽기 쉬운 형태로 변환. 
    //clientaddr에 저장된 주소정보를 hostname, port에 읽기 쉬운 호스트 이름과 포트 번호로 변환 후, client_port, client_hostname에 저장
    Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0); 
    //클라이언트 주소 정보 출력
    printf("Connected to (%s, %s)\n", client_hostname, client_port);
    //클라이언트가 보낸 데이터를 클라이언트에게 다시 되돌려줌
    echo(connfd);
    //소켓 닫음
    Close(connfd);
  }
  exit(0);
}