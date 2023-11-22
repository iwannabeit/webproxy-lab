/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 *
 * Updated 11/2019 droh
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 */
#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize, char *method);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs, char *method);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);

int main(int argc, char **argv) {
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr,
                    &clientlen);  // line:netp:tiny:accept
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    doit(connfd);   // line:netp:tiny:doit
    Close(connfd);  // line:netp:tiny:close
  }
}
//DOIT
void doit(int fd)
{
  int is_static; //정적 파일 구분 변수
  struct stat sbuf; // 파일 정보를 담기 위한 구조체
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  //printf("fd number:%d\n", fd);

  /*request 라인과 헤더 읽기*/
  //request 라인
  Rio_readinitb(&rio, fd);//rio구조체를 초기화한 후 fd 와 rio를 연결
  Rio_readlineb(&rio, buf, MAXLINE); //버퍼에 fd를 저장
  
  //request 헤더
  printf("Request headers:\n");
  printf("%s", buf); //GET/HTTP/1.1
  //sscanf : string scanf 로, 문자를 추출해서 데이터를 변수에 저장
  sscanf(buf, "%s %s %s", method, uri, version);//파싱하여 http메서드, uri, version 정보 추출
  
  if(strcasecmp(method, "GET") && strcasecmp(method, "HEAD")){//
    clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method");
    return;
  }
  //요청헤더를 읽어들이는 함수 호출
  read_requesthdrs(&rio);
  printf("requsthdhdhdhdhdhdhdhdhdhdhdhdhd%s\n", buf);

  /*Parse URI from GET request*/
  is_static = parse_uri(uri, filename, cgiargs);

  if(stat(filename, &sbuf) <0){
    clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
    return;
  }

  if(is_static){ //1이면 실행(정적이면)
    if(!(S_ISREG(sbuf.st_mode))||!(S_IRUSR & sbuf.st_mode)){
      clienterror(fd, filename, "403", "Forbidden", "tiny couldn't read the file");
      return;
    }
    serve_static(fd, filename, sbuf.st_size, method);
  }
  else{ //동적이면 0
    if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)){
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
      return;
    }
    serve_dynamic(fd, filename, cgiargs, method);
  }
}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
  char buf[MAXLINE], body[MAXBUF];

  sprintf(body, "<html><title>Tiny Error</title>");
  sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);
  
  /* Print the HTTP response */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-Type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-Length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}
//read_requesthdrs
void read_requesthdrs(rio_t *rp)
{
  char buf[MAXLINE];

  Rio_readlineb(rp, buf, MAXLINE);
  while(strcmp(buf, "\r\n")){ //공백두개인경우까지 반복문 수행
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }
  return;
}
//파싱파싱파싱
int parse_uri(char *uri, char*filename, char *cgiargs)
{
  char *ptr;
  //strstr(a,b) a에 b가 있으면 참
  if(!strstr(uri, "cgi-bin")){ //uri에 cgibin있는지 
    strcpy(cgiargs, "");
    strcpy(filename, ".");
    //strcat 연결
    strcat(filename, uri);
    if(uri[strlen(uri)-1] == '/'){
      strcat(filename, "home.html");
    }
    return 1;
  }
  else{ //cgibin이 있으면 
    ptr = index(uri, '?');
    if(ptr){
      strcpy(cgiargs, ptr+1); //null전까지 cgiargs에 들어감
      *ptr = '\0';
    }
    else{
      strcpy(cgiargs, "");
    }
    strcpy(filename, ".");
    strcat(filename, uri);
    return 0;
  }
}

void serve_static(int fd, char *filename, int filesize, char *method)
{
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];

  /*send response headers to client*/
  get_filetype(filename, filetype);
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
  sprintf(buf, "%sConnection: close\r\n", buf);
  sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
  sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
  Rio_writen(fd, buf, strlen(buf));
  printf("Response headers:\n");
  printf("%s", buf); //헤더 출력

  /* Send response body to client */
    if(strcmp(method, "GET") == 0 ){ //method가 get일 때만 body를 처리한다.
    srcfd = Open(filename, O_RDONLY, 0);
  
    //srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    Rio_readn(srcfd, srcp, filesize);
    Close(srcfd);
    Rio_writen(fd, srcp, strlen(srcp));
    Free(srcp);
  }
}

void get_filetype(char *filename,  char *filetype)
{
  if(strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if(strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".png"))
    strcpy(filetype, "image/png");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg");
  else if (strstr(filename, ".mp4"))
    strcpy(filetype, "video/mp4");
  else 
    strcpy(filetype, "text/plain");

}

void serve_dynamic(int fd, char *filename, char *cgiargs, char *method)
{
  char buf[MAXLINE], *emptylist[] = {NULL};

  /*return first part of HTTP response*/
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));

  //Child
  if(Fork() == 0){
    //query_string 을 adder.c에 보내줌
    setenv("QUERY_STRING", cgiargs, 1);
    //method를 cgi-bin/adder.c 로 보내주기 위해
    setenv("REQUEST_METHOD", method, 1);
    Dup2(fd, STDOUT_FILENO); //표준출력을 fd로 복제 -> 자식프로세스 표준출력이 원래 fd로 리다이렉트된다.
    Execve(filename, emptylist, environ); //filename을 실행 emptylist-> 명령라인인자가 없음, environ->현재 환경변수 전달
  }
  Wait(NULL);
}