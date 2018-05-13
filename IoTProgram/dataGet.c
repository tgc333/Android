#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "/usr/include/mysql/mysql.h"

void htmlReturn(void)
{
  char content[MAXLINE];
  char *buf;
  char *ptr;

  /* Make the response body */
  sprintf(content, "%s<html>\r\n<head>\r\n", content);
  sprintf(content, "%s<title>CGI test result</title>\r\n", content);
  sprintf(content, "%s</head>\r\n", content);
  sprintf(content, "%s<body>\r\n", content);
  sprintf(content, "%s<h2>Welcome to the CGI program</h2>\r\n", content);
  buf = getenv("QUERY_STRING");
  sprintf(content,"%s<p>Env : %s</p>\r\n", content, buf);
  ptr = strsep(&buf, "&");
  while (ptr != NULL){
    sprintf(content, "%s%s\r\n", content, ptr);
    ptr = strsep(&buf, "&");
  }
  sprintf(content, "%s</body>\r\n</html>\r\n", content);
  
  /* Generate the HTTP response */
  printf("Content-Length: %d\r\n", (int)strlen(content));
  printf("Content-Type: text/html\r\n\r\n");
  printf("%s", content);
  fflush(stdout);
}

void textReturn(void)
{
  char content[MAXLINE];
  char *buf;
  char *ptr;

  buf = getenv("QUERY_STRING");
  sprintf(content,"%sEnv : %s\n", content, buf);
  ptr = strsep(&buf, "&");
  while (ptr != NULL){
    sprintf(content, "%s%s\n", content, ptr);
    ptr = strsep(&buf, "&");
  }
  
  /* Generate the HTTP response */
  printf("Content-Length: %d\n", (int)strlen(content));
  printf("Content-Type: text/plain\r\n\r\n");
  printf("%s", content);
  fflush(stdout);
}

void myfunc(void){

  char content[MAXLINE];
  char *buf = getenv("QUERY_STRING");
  char *cmd, *arg1, *arg2, *time;
  int count, id, index;
  float value, average=0, min=1000, max=0;

  strtok(buf,"=");
  cmd = strtok(NULL,"&");
  strtok(NULL,"=");
  arg1 = strtok(NULL,"&");
  strtok(NULL,"=");
  arg2 = strtok(NULL,"&");

  if(arg2!=NULL)
    index = atoi(arg2);

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
  if(mysql_query(conn, "SELECT * FROM sensorList")) {
      printf("SELECT ERROR\n");
      exit(1);
  }
  if(strcmp(cmd, "List")==0){
    MYSQL_RES *result = mysql_store_result(conn);
    while((row = mysql_fetch_row(result))!=NULL)
      sprintf(content, "%s%s\n", content, row[0]);
  }

  else if(strcmp(cmd, "INFO")==0){
    MYSQL_RES *result = mysql_store_result(conn);
    while((row = mysql_fetch_row(result))!=NULL) {
      if(!strcmp(arg1, row[0])){
        id = atoi(row[1]);
        count = atoi(row[2]);
      }
    }
    char name[MAXLINE];
    sprintf(name, "SELECT * FROM sensorData%02d;",id);
    mysql_query(conn, name);
    result = mysql_store_result(conn);
    while((row = mysql_fetch_row(result))!=NULL) {
      if(min>atof(row[1]))
        min=atof(row[1]);
      if(max<atof(row[1]))
        max=atof(row[1]);
      average += atof(row[1]);
      time = row[0];
      value = atof(row[1]);
    }
    average /= count;
    sprintf(content, "id : %d\n", id);
    sprintf(content, "%scount : %d\n", content, count);
    sprintf(content, "%smin : %f\n", content, min);
    sprintf(content, "%saverage : %f\n", content, average);
    sprintf(content, "%smax : %f\n", content, max);
  }

  else if(strcmp(cmd, "GET")==0){
    MYSQL_RES *result = mysql_store_result(conn);
    while((row = mysql_fetch_row(result))!=NULL) {
      if(!strcmp(arg1, row[0])){
        id = atoi(row[1]);
        count = atoi(row[2]);
      }
    }
    char name[MAXLINE];
    sprintf(name, "SELECT * FROM sensorData%02d;",id);
    mysql_query(conn, name);
    result = mysql_store_result(conn);
    int flag=0;
    while((row = mysql_fetch_row(result))!=NULL) {
      if(count-index < 0){
        printf("fail to read table\n\n");
        break;
      }
      else{
        flag++;
          if(flag>(count-index)){
          time = row[0];
          value = atof(row[1]);
          sprintf(content, "%s%s %f\n", content, time, value);
        }
      }
    }
  }
  printf("Content-Length: %d\n", (int)strlen(content));
  printf("Content-Type: text/plain\r\n\r\n");
  printf("%s", content);

  printf("HTTP/1.0 200 OK\r\n");
}

int main(void)
{
  //htmlReturn();
  //textReturn();
  myfunc();
  return(0);
}
