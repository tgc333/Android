/*
 * clientGet.c: A very, very primitive HTTP client for console.
 * 
 * To run, prepare config-cg.txt and try: 
 *      ./clientGet
 *
 * Sends one HTTP request to the specified HTTP server.
 * Prints out the HTTP response.
 *
 * For testing your server, you will want to modify this client.  
 *
 * When we test your server, we will be using modifications to this client.
 *
 */


#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include "stems.h"
#include <string.h>

/*
 * Send an HTTP request for the specified file 
 */
void clientSend(int fd, char *filename)
{
  char buf[MAXLINE];
  char hostname[MAXLINE];

  Gethostname(hostname, MAXLINE);

  /* Form and send the HTTP request */
  sprintf(buf, "GET %s HTTP/1.1\n", filename);
  sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
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
    printf("Header: %s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);

    /* If you want to look for certain HTTP tags... */
    if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
      printf("Length = %d\n", length);
    }
  }

  /* Read and display the HTTP Body */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (n > 0) {
    printf("%s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }
}

/* currently, there is no loop. I will add loop later */
void userTask(char hostname[], int port, char webaddr[])
{
  int clientfd;

  clientfd = Open_clientfd(hostname, port);
  clientSend(clientfd, webaddr);
  clientPrint(clientfd);
  Close(clientfd);
}

void getargs_cg(char hostname[], int *port, char webaddr[])
{
  FILE *fp;

  fp = fopen("config-cg.txt", "r");
  if (fp == NULL)
    unix_error("config-cg.txt file does not open.");

  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", webaddr);
  fclose(fp);
}

int main(void)
{
  char hostname[MAXLINE], webaddr[MAXLINE];
  int port;

  pid_t pid = Fork();
  
  if(pid == 0){
    Execve("./pushServer", (NULL), (NULL));
  }

  getargs_cg(hostname, &port, webaddr);

  char input[MAXLINE];
  char sendcmd[MAXLINE];
  char *cmd, *arg1, *arg2;
  printf("If you want to see commands, type 'help'\n");
  
  while(1) {
    strcpy(sendcmd, webaddr);
    printf(">> ");
    fgets(input, MAXLINE, stdin);
    int n = strlen(input) - 1;
    input[n] = '\0';
    cmd = strtok(input, " ");
    arg1 = strtok(NULL, " ");
    arg2 = strtok(NULL, " ");
    if(cmd != NULL) {
      if (strcasecmp(cmd, "quit") == 0 || strcasecmp(cmd, "exit") == 0)
        break;
      else if (strcasecmp(cmd, "help") == 0) {
        printf("help: list available commands.\n");
        printf("List: print sensorList.\n");
        printf("INFO <sname>: print sensor <sname> entry\n");
        printf("GET <sname>: print the most recent time, value\n");
        printf("GET <sname> <n>: print the most recent n time, value\n");
        printf("quit: quit the program.\n");
        printf("exit: exit the program.\n");
      }
      else if (strcasecmp(cmd, "List") == 0) {
        sprintf(sendcmd, "%scommand=List", sendcmd);
        userTask(hostname, port, sendcmd);
      }
      else if (strcasecmp(cmd, "INFO") == 0 && arg1 != NULL) {
        sprintf(sendcmd, "%scommand=INFO&value=%s", sendcmd, arg1);
        userTask(hostname, port, sendcmd);
      }
      else if (strcasecmp(cmd, "GET") == 0 && arg1 != NULL) {
        if (arg2 != NULL) {
          sprintf(sendcmd, "%scommand=GET&NAME=%s&N=%s", sendcmd, arg1, arg2);
          userTask(hostname, port, sendcmd);
        }
        else {
          sprintf(sendcmd, "%scommand=GET&NAME=%s&N=1", sendcmd, arg1);
          userTask(hostname, port, sendcmd);  
        }
      }
    }
  }
  kill(pid, SIGKILL);
  
  return(0);
}
