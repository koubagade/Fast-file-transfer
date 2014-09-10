#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>

#define BUFFER_SIZE 65507

int n;
FILE *output;
struct sockaddr_in serv_addr, cli_addr;
int count = 0;

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[])
{
  int sockfd, portno;
  socklen_t fromlen;
  char buffer[BUFFER_SIZE];
  int flag = 1;
  int nbrecv = 0;    

  if (argc < 2)
    {
      fprintf(stderr,"ERROR, no port provided\n");
      exit(1);
    }
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    error("ERROR on binding");
  
  fromlen = sizeof(cli_addr);
  output = fopen("output.txt", "w");
  
  while(flag == 1)
    {
      memset(buffer, '\0', BUFFER_SIZE);
      if((nbrecv = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cli_addr, &fromlen)) == -1)
	{
	  fprintf(stderr, "fail while receiving data! \n");
	  exit(-1);
	}
      count += nbrecv;
      fflush(output);
      fwrite(buffer, 1, nbrecv, output);
      if(nbrecv<65507)
	{
	  flag = 0;
	}
    }
  printf("%d bytes received. \n", count);
  printf("Got the file!\n");
  close(sockfd);
  fclose(output);
  return 0;
}
