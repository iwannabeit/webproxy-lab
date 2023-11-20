/*
 * adder.c - a minimal CGI program that adds two numbers together
 */
/* $begin adder */
#include "csapp.h"

int main(void) {
  //문자열을 저장할 버퍼 및 변수 선언
  char *buf, *p;
  char arg1 [MAXLINE], arg2[MAXLINE], content[MAXLINE];
  int n1=0, n2=0;
  
  if((buf = getenv("QUERY_STRING")) != NULL){
    p = strchr(buf, '&'); //buf에서 &가 처음으로 나타나는 위치를 가리키는 p
    *p = '\0'; //p를 null로 변경 -> buf문자열은 &문자를 기준으로 두 부분으로 나누어짐(&문자 이후부분은 새로운 문자열 시작)
    strcpy(arg1, buf);
    strcpy(arg2, p+1);
    n1 = atoi(arg1);
    n2 = atoi(arg2);

  }

  sprintf(content, "query_string=%s", buf);
  sprintf(content, "welcome to add.com: ");
  sprintf(content, "%sTHE Internet addition portal. \r\n<p>", content);
  sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>", content, n1, n2, n1+n2);
  sprintf(content, "%sThanks for visiting!\r\n", content);

  /*generate the http response*/
  printf("Connection: close\r\n");
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("content-type: text/html\r\n\r\n");
  printf("%s", content);
  fflush(stdout);

  exit(0);
}
/* $end adder */
