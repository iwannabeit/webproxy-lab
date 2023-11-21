/*
 * adder.c - a minimal CGI program that adds two numbers together
 */
/* $begin adder */
#include "csapp.h"

int main(void) {
  //문자열을 저장할 버퍼 및 변수 선언
  char *buf, *p, *a, *b, *num1, *num2;
  char arg1 [MAXLINE], arg2[MAXLINE], content[MAXLINE];
  int n1=0, n2=0;
  
  if(((buf = getenv("QUERY_STRING")) != NULL) && strcmp(buf,"")){ //query_string --> ?이후에 나오는 문자열 즉, 100&100을 buf에 넣음
    p = strchr(buf, '&'); //buf에서 &가 처음으로 나타나는 위치를 가리키는 p
    *p = '\0'; //p를 null로 변경 -> buf문자열은 &문자를 기준으로 두 부분으로 나누어짐(&문자 이후부분은 새로운 문자열 시작)
    a = buf;
    b = p+1;
    num1 = strchr(a, '=');
    *num1 = '\0';
    num2 = strchr(b, '=');
    *num2 = '\0';

    strcpy(arg1, num1+1);
    strcpy(arg2, num2+1);
    n1 = atoi(arg1);
    n2 = atoi(arg2);
  }
  
  sprintf(content, "<html><head><title>Adder Form</title></head><body>\n");
  sprintf(content + strlen(content), "<form action=\"/cgi-bin/adder\" method=\"get\">\n");
  sprintf(content + strlen(content), "  <label for=\"num1\">Number 1:</label>\n");
  sprintf(content + strlen(content), "  <input type=\"text\" id=\"num1\" name=\"num1\"><br>\n");
  sprintf(content + strlen(content), "  <label for=\"num2\">Number 2:</label>\n");
  sprintf(content + strlen(content), "  <input type=\"text\" id=\"num2\" name=\"num2\"><br>\n");
  sprintf(content + strlen(content), "  <input type=\"submit\" value=\"Add\">\n");
  sprintf(content + strlen(content), "</form>\n");
  sprintf(content + strlen(content), "<p>The answer is: %d + %d = %d</p>\n", n1, n2, n1 + n2);
  sprintf(content + strlen(content), "</body></html>\n");
  /*generate the http response*/
  printf("Connection: close\r\n");
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("content-type: text/html\r\n\r\n");
  printf("%s", content);
  fflush(stdout);

  exit(0);
}
/* $end adder */
