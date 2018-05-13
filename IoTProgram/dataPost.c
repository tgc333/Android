#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "/usr/include/mysql/mysql.h"
//
// This program is intended to help you test your web server.
// 

int main(int argc, char *argv[])
{
  int fd;
  char *body;
  char body_size[MAXBUF];
  int bodylength;
  char *method = getenv("REQUEST_METHOD");
  char *fifo_name = getenv("FIFO_NAME");

  if(strcasecmp(method, "GET") == 0){
    char *argument = getenv("QUERY_STRING");
  }
  else if(strcasecmp(method, "POST") == 0){
    char *tmplength = getenv("CONTENT_LENGTH");
    body = getenv("REQUEST_BODY");
    char restbody[MAXLINE];
    sscanf(tmplength, "%d", &bodylength);
    rio_t rio;
    Rio_readinitb(&rio, STDIN_FILENO);
    if(bodylength > (strlen(body)-1))
      Rio_readnb(&rio, restbody, bodylength - strlen(body));
  }

  if((fd = open(fifo_name, O_RDWR)) < 0){
    printf("fail to call open() dp\n");
    exit(1);
  }

  sprintf(body_size,"%d", bodylength);
  if(write(fd, body_size, MAXBUF) == -1){
    printf("fail to call write() dp\n");             
    exit(1);
  }

  if(write(fd, body, bodylength)==-1){
    printf("fail to call write() dp2\n");
    exit(1);
  }
  close(fd);



  char *name, *temp, *time;
  float value;
  strtok(body,"=");
  name = strtok(NULL,"&");
  strtok(NULL,"=");
  time = strtok(NULL,"&");
  strtok(NULL,"=");
  temp = strtok(NULL,"&");
  sscanf(temp, "%f", &value);

  MYSQL *conn;
  MYSQL_ROW row;

  char *server = "localhost";
  char *user = "root";
  char *password = "z156456";
  char *database = "ops";

  if(!(conn = mysql_init((MYSQL*)NULL))){
    printf("init fail\n");
    exit(1);
  }

  if(!mysql_real_connect(conn, server, user, 
    password, NULL, 3306, NULL, 0)){
    printf("connect error\n");
    exit(1);
  }

  if(mysql_select_db(conn, database)!=0){
    mysql_close(conn);
    printf("select_db fail\n");
    exit(1);
  }
  
  if(mysql_query(conn, "SELECT * FROM sensorList;")) {
    printf("SELECT ERROR\n");
    exit(1);
  }

  MYSQL_RES *result = mysql_store_result(conn);  
  int usercount=1;
  int id_num=0, id_max=1;
  char cmd[256];

  while((row = mysql_fetch_row(result))!=NULL){
    if(!strcmp(name, row[0])){
      usercount = atoi(row[2])+1;
      id_num = atoi(row[1]);
      sprintf(cmd, "UPDATE sensorList SET count=%d WHERE name='%s'", usercount, name);
      mysql_query(conn, cmd);
    }
    id_max = atoi(row[1])+1;
  }

  if(usercount==1){
    id_num = id_max;
    sprintf(cmd, "INSERT INTO sensorList VALUES('%s', NULL, %d)", name, usercount);
    mysql_query(conn, cmd);
    sprintf(cmd, "create table sensorData%02d( time varchar(30), data double, num int auto_increment primary key);", id_num);
    mysql_query(conn, cmd);
  }

  sprintf(cmd, "INSERT INTO sensorData%02d VALUES('%s', %f, NULL);",id_num, time, value);
  mysql_query(conn, cmd);


  fflush(stdout);
  return(0);
}
