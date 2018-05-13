#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "stems.h"
#include <sys/time.h>
#include <time.h>
#define MAXTIMINGS 83
#define DHTPIN 7
int dht11_dat[5] = {0, 0, 0, 0, 0};
void currentTime(char *time) {
  struct timeval val;
  struct tm *ptm;
  
  gettimeofday(&val, NULL);
  ptm = localtime(&val.tv_sec);
 
  //memset(dt , 0x00 , sizeof(dt));
 
  // format : YYMMDDhhmmssuuuuuu
  sprintf(time, "%04d-%02d-%02d(%02d:%02d:%02d)"
      , ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday
      , ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
}
void getargs_cp(char *hostname, int *port, char *filename, int *period)
{
  FILE *fp;
  fp = fopen("config-pi.txt", "r");
  if (fp == NULL)
    unix_error("config-pi.txt file does not open.");
  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", filename);
  fscanf(fp, "%d", period);
  fclose(fp);
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
    sscanf(buf, "Content-Length: %d ", &length);
    if (sscanf(buf, "Content-Length: %d ", &length) == 1);
     //printf("Length = %d\n", length);
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
 
void userTask(char *myname, char *hostname, int port, char *filename, char *value)
{
   char time[MAXLINE];
   int clientfd;
   char msg[MAXLINE];
   currentTime(time);
   sprintf(msg, "name=%s&time=%s&value=%f", myname, time, atof(value));
   clientfd = Open_clientfd(hostname, port);
   clientSend(clientfd, filename, msg);
   clientPrint(clientfd);
   close(clientfd);
}
 
int read_dht11_dat(char *temperature, char *humidity) {  
   uint8_t laststate = HIGH;  
   uint8_t counter = 0;  
   uint8_t j = 0, i;  
   uint8_t flag = HIGH;  
   uint8_t state = 0;  
   float f;
   memset(dht11_dat, 0, sizeof(int) * 5);
   pinMode(DHTPIN, OUTPUT);
   digitalWrite(DHTPIN, LOW);  
   delay(18);  
   digitalWrite(DHTPIN, HIGH);  
   delayMicroseconds(30);  
   pinMode(DHTPIN, INPUT);  
   for (i = 0; i < MAXTIMINGS; i++) {    
      counter = 0;   
      while (digitalRead(DHTPIN) == laststate) {      
         counter++;      
         delayMicroseconds(1);      
         if (counter == 200) break;    
      }    
      laststate = digitalRead(DHTPIN);    
      if (counter == 200) break; // if while breaked by timer, break for   
      if ((i >= 4) && (i % 2 == 0)) {      
         dht11_dat[j / 8] <<= 1;      
         if (counter > 20) 
            dht11_dat[j / 8] |= 1;      j++;    
      }  
   }  
   if ((j >= 40) && (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + 
        dht11_dat[3]) & 0xff))) {    
      printf("humidity = %d.%d %% Temperature = %d.%d *C \n", dht11_dat[0],   
         dht11_dat[1], dht11_dat[2], dht11_dat[3]);
         sprintf(humidity,"%d.%02d", dht11_dat[0], dht11_dat[1]);
         sprintf(temperature,"%d.%02d", dht11_dat[2], dht11_dat[3]);
      return 1;
   }  
   else{
      printf("Data get failed\n");
      return 0;
   } 
   
}
int main(void) 
{  
   char hostname[MAXLINE], filename[MAXLINE];
   int port, period;
   char temperature[MAXLINE], humidity[MAXLINE];
   getargs_cp(hostname, &port, filename, &period);
   printf("dht11 Raspberry pi\n");  
   if (wiringPiSetup() == -1) exit(1);  
   period *= 1000;
   while (1) {    
      delay(period/2);
      if(read_dht11_dat(temperature, humidity)){
         userTask("temperaturePI", hostname, port, filename, temperature);
         delay(period/2);
	 userTask("humidityPI", hostname, port, filename, humidity);    
      }
   }  
   return 0; 
} 