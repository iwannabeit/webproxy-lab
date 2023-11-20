#include "csapp.h"

int main(int argc, char **argv)
{
  int clientfd;
  char *host, *port, buf[MAXLINE];
  rio_t rio;

  if (argc != 3){ //실행파일 포함 3개 입력
    fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
    exit(0);
  }
  host = argv[1];
  port = argv[2];
  //서버에 연결된 클라이언트 소켓의 파일 디스크립터를 저장
  clientfd = Open_clientfd(host, port);
  Rio_readinitb(&rio, clientfd);//초기화


  while(Fgets(buf, MAXLINE, stdin) != NULL){//문자열 입력 무한루프, MAXLINE 바이트까지 문자열 읽음. 문자열은 buf에 저장
    // 입력된 문자열(buf)를 소켓을 통해 서버로 전송 Client -> Server
    Rio_writen(clientfd, buf, strlen(buf)); 
    
    //서버로부터 데이터를 읽어옴 Server -> Client
    rio_readlineb(&rio, buf, MAXLINE);
    Fputs(buf, stdout); //읽어온 데이터 출력(문자열을 출력하는 함수)
  }
  Close(clientfd);
  exit(0);
}