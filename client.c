#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>

#define BUFFER_SIZE 1024 

FILE *input;
int n; 
int count = 0; 
struct sockaddr_in serv_addr;  
int s;              

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[])
{
  int sockfd, portno, n;
  unsigned char buffer[BUFFER_SIZE];
  struct hostent *server;
  socklen_t fromlen;

  if (argc < 4) 
    {
      fprintf(stderr,"usage %s hostname port filename\n", argv[0]);
      exit(0);
    }

  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");
  server = gethostbyname(argv[1]);
  if (server == NULL)
    {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
    }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
  serv_addr.sin_port = htons(portno);
  
  fromlen = sizeof(struct sockaddr_in);
  input = fopen(argv[3], "r");
  
  while(!feof(input))
    {
      n = fread(buffer, 1, BUFFER_SIZE, input);
      count += n;
      //printf("n = %d\n", n);
      if(sendto(sockfd, buffer,n, 0, (struct sockaddr *)&serv_addr, fromlen) == -1)
	{
	  fprintf(stderr, "error while sending data!\n");
	  exit(-1);
	}
    }
  printf("%d bytes sent. \n", count);
  printf("File sent!!\n");
  close(sockfd);
  fclose(input);
  return 0;
}
