#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  char buf[MAXBUF];
  char *body;
  int size = 0;
  rio_t rio;
  char *name, *temp, *time;
  float value;

  size = atoi(getenv("CONTENT_LENGTH"));
  body = getenv("REQUEST_BODY");
  Rio_readinitb(&rio, STDIN_FILENO);

  if(strlen(body)-1<size)
    Rio_readnb(&rio, buf, size - strlen(body));

  	strcat(body,buf);
	
  strtok(body,"=");
  name = strtok(NULL,"&");
  strtok(NULL,"=");
  time = strtok(NULL,"&");
  strtok(NULL,"=");
  temp = strtok(NULL,"&");
  sscanf(temp, "%f", &value);

  printf("HTTP/1.0 200 OK\r\n");
  fprintf(stderr, "경고: %s로부터 %s 시각에 %f라는 값이 발생했습니다.\n", name, time, value);

  return 0;
}