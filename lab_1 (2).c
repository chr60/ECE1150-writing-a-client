#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "lab_1.h"

#define BUFFER_SIZE 1024
#define TRACE_ON = 0; 

int main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, j;
    size_t len;
    ssize_t nread;

    if (argc !=3)
    {
        printf("Please enter arguements in the following format: CITY STATE");
        exit(1);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM;

    s = getaddrinfo("api.wunderground.com", "http", &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                      rp->ai_protocol);
         if (sfd == -1)
             continue;

         if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
         {
            //printf("\nconnected");
            break;                  /* Success */ 
         }
         else{
            close(sfd);
         }
     }

 if (rp == NULL) {               /* No address succeeded */
     fprintf(stderr, "Could not connect\n");
     exit(EXIT_FAILURE);
 }

//write query string to the socket
char QString[200] = "GET /api/b0a73c25c2f40b1b/conditions/q/";
char QStringEnd[] = ".xml HTTP/1.1\r\nHost: api.wunderground.com\r\n\r\n";
// "GET /api/b0a73c25c2f40b1b/conditions/q/PA/Pittsburgh.xml HTTP/1.1\r\nHost: api.wunderground.com\r\n\r\n"
strcat(QString, argv[2]);
strcat(QString, "/");
strcat(QString, argv[1]);
strcat(QString, QStringEnd);
int sendRes = write(sfd, QString, strlen(QString));

//create buffer and read from socket
char buff[512];
int leng = 0;
leng = read(sfd, buff, sizeof(buff));
if(leng <= 0)
{
    printf("\nERROR - Bad City State\n");
    printf("Please enter arguements in the following format: CITY STATE");
}

//2. parse content length
int content_length = parse_content_length(buff);
//printf("The content length is: %d", content_length);

//3. Malloc a buffer of the correct size
char * full_buffer = malloc(sizeof(char)*content_length);

//make a chunck buffer
char buffer[BUFFER_SIZE];
bzero(buffer, BUFFER_SIZE);

int i = 1;
while(read(sfd, buffer, BUFFER_SIZE - 1) > 0)
{
    //printf("\nNEW READ_____________________________________");
    //printf("%s", buffer);
    strcat(full_buffer,buffer); //copy chunck buffer into big buffer
    bzero(buffer, BUFFER_SIZE);

    //printf("\nBYTES WRITTEN: %d\nContent Length: %d", i*BUFFER_SIZE, content_length);
    if(i*BUFFER_SIZE >= content_length)
    {
        break;
    }
    i++;
}

//printf("\nRESULT___________________________\n%s\n\n", full_buffer);

print_weather(full_buffer, argv[1], argv[2]);

exit(EXIT_SUCCESS);
}