#include "stems.h"
#include "request.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct timeval startTime;

void requestReadhdrs(rio_t *rp, int *length)
{
  char buf[MAXLINE];

  *length = -1;
  Rio_readlineb(rp, buf, MAXLINE);
  while (strcmp(buf, "\r\n")) {
    sscanf(buf, "Content-Length: %d", length);
    Rio_readlineb(rp, buf, MAXLINE);
  }
  return;
}

int parseURI(char *uri, char *filename, char *cgiargs) 
{
  if (!strstr(uri, "cgi")) {
    // static
    strcpy(cgiargs, "");
    sprintf(filename, ".%s", uri);
    if (uri[strlen(uri)-1] == '/') {
      strcat(filename, "index.html");
    }
    return STATIC;
  } else {
    char *method = getenv("REQUEST_METHOD");
    sprintf(filename, ".%s", uri);
    filename = strtok(filename, "?");
    cgiargs = strtok(NULL, "");
    if(strcasecmp(method, "GET")==0)
    		Setenv("QUERY_STRING", cgiargs, 1);
    return DYNAMIC;
  }
}

// handle a request
void requestServeDynamic(rio_t *rio, int fd, char *filename, char *cgiargs, int bodyLength, double arrivalTime, int id)
{
  //
  // Followings are dummy. After they should be replaced with dynamic
  // request implementation.
  //
  char buf[MAXLINE];
  char fifo_name[MAXBUF];
  if (bodyLength > 0) {
    Rio_readrestb(rio, cgiargs);
    Setenv("REQUEST_BODY", cgiargs, 1);
    //printf("%s\n",cgiargs);
  }
  
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf, "%sServer: My Web Server\r\n", buf);
  sprintf(buf, "%sContent-Length: %d\r\n", buf, (int)strlen(cgiargs));
  sprintf(buf, "%sContent-Type: text/plain\r\n", buf);
  sprintf(buf, "%sStat-req-arrival: %lf\r\n\r\n", buf, arrivalTime);
  sprintf(buf, "%s\r\n", buf);
  Rio_writen(fd, buf, strlen(buf));

  sprintf(fifo_name,"./fifo%d", id);
  pid_t pid;
  pid = Fork();

  if(pid == 0) {
     Setenv("FIFO_NAME", fifo_name, 1);
     Dup2(fd, STDOUT_FILENO);
     Dup2(fd, STDIN_FILENO);
     Execve(filename, (NULL) , environ);
   }
}

void initWatch(void)
{
  gettimeofday(&startTime, NULL);
}

double getWatch(void)
{
  struct timeval curTime;
  double tmp;

  gettimeofday(&curTime, NULL);
  tmp = (curTime.tv_sec - startTime.tv_sec) * 1000.0;
  return (tmp - (curTime.tv_usec - startTime.tv_usec) / 1000.0);
}

void requestError(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) 
{
  char buf[MAXLINE], body[MAXBUF];

  printf("Request ERROR\n");

  // Create the body of the error message
  sprintf(body, "<html><title>CS537 Error</title>");
  sprintf(body, "%s<body bgcolor=""fffff"">\r\n", body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr>CS537 Web Server\r\n", body);

  // Write out the header information for this response
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  printf("%s", buf);

  sprintf(buf, "Content-Type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  printf("%s", buf);

  sprintf(buf, "Content-Length: %lu\r\n\r\n", (long unsigned int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  printf("%s", buf);

  // Write out the content
  Rio_writen(fd, body, strlen(body));
  printf("%s", body);

}

void requestGetFiletype(char *filename, char *filetype)
{
  if (strstr(filename, ".html")) 
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif")) 
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".jpg")) 
    strcpy(filetype, "image/jpeg");
  else 
    strcpy(filetype, "test/plain");
}

void requestServeStatic(int fd, char *filename, int filesize, double arrivalTime)
{
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];

  requestGetFiletype(filename, filetype);
  srcfd = Open(filename, O_RDONLY, 0);
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
  Close(srcfd);

  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf, "%sServer: My Web Server\r\n", buf);
  sprintf(buf, "%sStat-req-arrival: %lf\r\n", buf, arrivalTime);  
  sprintf(buf, "%sContent-Length: %d\r\n", buf, filesize);
  sprintf(buf, "%sContent-Type: %s\r\n\r\n", buf, filetype);

  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, srcp, filesize);
  Munmap(srcp, filesize);
}

void requestHandle(int connfd, double arrivalTime, int id)
{
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  rio_t rio;
  int reqType;
  char filename[MAXLINE], cgiargs[MAXLINE];
  struct stat sbuf;
  int bodyLength;

  Rio_readinitb(&rio, connfd);
  Rio_readlineb(&rio, buf, MAXLINE);
  sscanf(buf, "%s %s %s", method, uri, version);
  printf("%s %s %s\n", method, uri, version);
  Setenv("REQUEST_METHOD", method, 1);
  requestReadhdrs(&rio, &bodyLength);
  reqType = parseURI(uri, filename, cgiargs);
  if ((strcasecmp(method, "GET")!=0)&&(strcasecmp(method,"POST")!=0)) {
    requestError(connfd, method, "501", "Not Implemented",
		 "My Server does not implement this method");
    return;
  }
  else if (strcasecmp(method, "POST") == 0) {
	char size[10];
	sprintf(size, "%d", bodyLength);
	Setenv("CONTENT_LENGTH", size, 1);
  }

  if (stat(filename, &sbuf) < 0) {
    requestError(connfd, filename, "404", "Not found", "My Server could not find this file");
    return;
  }

  if (reqType == STATIC) {
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
      requestError(connfd, filename, "403", "Forbidden", "My Server could not read this file");
      return;
    }
    requestServeStatic(connfd, filename, sbuf.st_size, arrivalTime);
  } else {
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
      requestError(connfd, filename, "403", "Forbidden", "My Server could not run this CGI program");
      return;
    }
    requestServeDynamic(&rio, connfd, filename, cgiargs, bodyLength, arrivalTime, id);
  }

}
                
