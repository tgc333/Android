/*
 * clientPost.c: A very, very primitive HTTP client for sensor
 * 
 * To run, prepare config-cp.txt and try: 
 *      ./clientPost
 *
 * Sends one HTTP request to the specified HTTP server.
 * Get the HTTP response.
 */


#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/time.h>
#include "stems.h"
#include <string.h>

void currentTime(char *time) {
  struct timeval val;
  struct tm *ptm;

  gettimeofday(&val, NULL);
  ptm = localtime(&val.tv_sec);
  sprintf(time, "%04d-%02d-%02d(%02d:%02d:%02d)"
    , ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday
    , ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
}

void clientSend(int fd, char *filename, char *body)
{
  char buf[MAXLINE];
  char hostname[MAXLINE];

  Gethostname(hostname, MAXLINE);

  /* Form and send the HTTP request */
  sprintf(buf, "POST %s HTTP/1.1\n", filename);
  sprintf(buf, "%sHost: %s\n", buf, hostname);
  sprintf(buf, "%sContent-Type: text/plain; charset=utf-8\n", buf);
  sprintf(buf, "%sContent-Length: %d\n\r\n", buf, (int)strlen(body));
  sprintf(buf, "%s%s\n", buf, body);
  Rio_writen(fd, buf, strlen(buf));
}
  
/*
 * Read the HTTP response and print it out
 */
void clientPrint(int fd)
{
  rio_t rio;
  char buf[MAXBUF];
  int length = 0;
  int n;
  
  Rio_readinitb(&rio, fd);

  /* Read and display the HTTP Header */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (strcmp(buf, "\r\n") && (n > 0)) {
    /* If you want to look for certain HTTP tags... */
    if (sscanf(buf, "Content-Length: %d ", &length) == 1)
      printf("Length = %d\n", length);
    printf("Header: %s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }

  /* Read and display the HTTP Body */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (n > 0) {
    printf("%s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }
}

/* currently, there is no loop. I will add loop later */
void userTask(char *myname, char *hostname, int port, char *filename, char *time, float value)
{
  int clientfd;
  char msg[MAXLINE];

  sprintf(msg, "name=%s&time=%s&value=%f", myname, time, value);
  printf("..sending %s\n",msg);
  clientfd = Open_clientfd(hostname, port);
  clientSend(clientfd, filename, msg);
  clientPrint(clientfd);
  Close(clientfd);
}

void getargs_cp(char *myname, char *hostname, int *port, char *filename, char *time, float *value)
{
  FILE *fp;

  fp = fopen("config-cp.txt", "r");
  if (fp == NULL)
    unix_error("config-cp.txt file does not open.");

  fscanf(fp, "%s", myname);
  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", filename);
  fscanf(fp, "%s", time);
  fscanf(fp, "%f", value);
  fclose(fp);
}

void setargs_cp(char *myname, char *hostname, int *port, char *filename, char *time, float *value)
{
  FILE *fp;

  fp = fopen("config-cp.txt", "w");
  if (fp == NULL)
    unix_error("config-cp.txt file does not open.");

  fprintf(fp, "%s\r\n", myname);
  fprintf(fp, "%s\r\n", hostname);
  fprintf(fp, "%d\r\n", *port);
  fprintf(fp, "%s\r\n", filename);
  fprintf(fp, "%s\r\n", time);
  fprintf(fp, "%f\r\n", *value);
  fclose(fp);
}

int main(void)
{
  char myname[MAXLINE], hostname[MAXLINE], filename[MAXLINE], time[MAXLINE];
  int port;
  float value;

  getargs_cp(myname, hostname, &port, filename, time, &value);

  char input[MAXLINE];
  char *temp;
  char *cmd;

  printf("If you want to see commands, type 'help'\n");
  while(1){
    printf(">> ");
    fgets(input, MAXLINE, stdin);
    int n = strlen(input) - 1;
    input[n] = '\0';

    cmd = strtok(input, " ");
    temp = strtok(NULL, " ");
    if(cmd != NULL) {
      if(strcmp(cmd,"quit")==0)
        break;
      else if(strcmp(cmd,"help") == 0) {
        printf("help: list available commands.\n");
        printf("name: print current sensor name.\n");
        printf("name <sensor>: change sensor name to <sensor>.\n");
        printf("value: print current value of sensor.\n");
        printf("value <n>: set sensor value to <n>.\n");
        printf("send: send(current sensor name, time, value) to server.\n");
        printf("quit: quit the program.\n");
      }
      else if(strcmp(cmd,"name") == 0 && temp == NULL) 
          printf("Current sensor is '%s'\n", myname);
      else if(strcmp(cmd, "name")==0 && temp != NULL){
        strcpy(myname,temp);
        printf("Sensor name is change to '%s'\n", myname);
      }
      else if(strcmp(cmd,"value") == 0 && temp == NULL)
          printf("Current value of sensor is '%f'\n", value);
      else if(strcmp(cmd, "value")==0 && temp != NULL){
        value = atof(temp);
        printf("Sensor value is change to '%f'\n", value);
      }
      else if(strcmp(cmd,"send")==0){
        currentTime(time);
        userTask(myname, hostname, port, filename, time, value);
        setargs_cp(myname, hostname, &port, filename, time, &value);
      }
    }
  }
  printf("The program will be terminated.\n\n");
  return(0);
}
