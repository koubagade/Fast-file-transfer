#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <time.h>
//#define BUFFER_SIZE 1024
#define TIMEOUT 250
//int n;
//FILE *output;
struct sockaddr_in serv_addr, cli_addr,cldummy;
int count = 0;
int recvq[20], recvhead = 0, recvtail=0, recvcounter=20, expected = 0;

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[])
{	
  expected = recvq[recvhead];
  //stores pid of expected packet.
  int i;
  int sockfd, portserv ,portcli;
  //socklen_t fromlen,fromlen1;
  struct hostent *client;
  int buffer;
  int buffer1;
  //int flag = 1;
  int nbrecv = 0;   
  int a;
  int last_sent;
  for(a=0;a<20;a++)
    recvq[a]=a;
  //recvq[10]=10;
  time_t start, end;
  fd_set socks;
  struct timeval t;
  FD_ZERO(&socks);
  
  t.tv_usec = 250000;
  t.tv_sec = 0;
  if (argc < 4)
    {
      fprintf(stderr,"ERROR, no port provided\n");
      exit(1);
    }
  //To receive from client
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");
  
  portserv = atoi(argv[3]);
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portserv);
  //fromlen = sizeof(cli_addr);
  
  //To send to client
  client = gethostbyname(argv[1]);
  portcli = atoi(argv[2]);
  if (client == NULL)
    {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
    }
  bzero((char *) &cli_addr, sizeof(cli_addr));
  cli_addr.sin_family = AF_INET;
  bcopy((char *)client->h_addr,(char *)&cli_addr.sin_addr.s_addr,client->h_length);
  cli_addr.sin_port = htons(portcli);
  
  //fromlen1 = sizeof(struct sockaddr_in);
  
  socklen_t fromlendummy = sizeof(struct sockaddr_in);
  socklen_t clilen = sizeof(struct sockaddr_in);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    error("ERROR on binding");
  int p = 1;
  //printf("Hello \n");
  while(recvhead != recvtail || p)
    { 
		
      for(i=0;i<20;i++)
	printf("%d ", recvq[i]);
      printf("\n\n\n");
      printf ("Recvhead and Recv tail are %d and %d\n",recvhead,recvtail);
		
      while(1)
	{
	  FD_ZERO(&socks);
	  FD_SET(sockfd, &socks);
	  t.tv_usec = 250000;
	  t.tv_sec = 0;
	  i = select(sockfd + 1, &socks, NULL, NULL, &t);
			
	  if ( i > 0 || p) 
	    {
	      if(FD_ISSET(sockfd, &socks))
		{
		  if (recvfrom(sockfd, &buffer, sizeof(int), 0, (struct sockaddr *)&cldummy, &fromlendummy) == -1)  
		    {
		      fprintf(stderr, "fail while receiving data!, Resending last NAK \n");
		      exit(-1);
		    }
				
		  p = 0;
		  break;
		}
	    }
	  else if( i == 0)
	    {
	      if(!p)
		{
		  buffer1 = recvq[recvhead];
		  printf("sending %d recvhead %d recvtail %d\n", buffer1, recvhead, recvtail);
		  if(sendto(sockfd,&buffer1,sizeof(buffer1), 0, (struct sockaddr *)&cli_addr, clilen) == -1)
		    {
		      fprintf(stderr, "error while sending data!\n");
		      exit(-1);
		    }
		  recvq[recvtail] = buffer1;
		  recvtail = (recvtail+1)%20;
		  recvhead = (recvhead+1)%20;
					
		}
	    }
	  else if (i < 0)
	    {
	      printf("Error in Select\n");
	      exit(0);
	    }
	}
      printf("Received from sender(client) = %d\n",buffer);
      while(recvq[recvhead] != buffer)
	{
	  //printf("Sending from server\n");
	  buffer1 = recvq[recvhead];
	  recvq[recvtail] = buffer1;
	  recvtail = (recvtail+1)%20;
	  //recvq[recvtail] = buffer1;
	  recvhead = (recvhead+1)%20;
	  //printf("Sending %d from buffer \n",buffer1);
	  if(sendto(sockfd,&buffer1,sizeof(buffer1), 0, (struct sockaddr *)&cli_addr, clilen) == -1)
	    {
	      fprintf(stderr, "error while sending data!\n");
	      exit(-1);
	    }
	}
      if(recvq[recvhead] == buffer)
	{
	  recvcounter --;
	  recvhead = (recvhead+1)%20;
	  printf("Received pid %d receive counter = %d  \n",buffer, recvcounter); 
	}
      if (recvcounter == 0)
	{
	  break;
	}
		
    }
   
  //int i;
  for(i=0;i<20;i++)
    printf("%d ", recvq[i]);
  printf("\n");
  printf ("Recvhead and Recv tail on quitting are %d and %d\n",recvhead,recvtail);
    
  int temp = 20;
  for(i=0;i<5;i++)
  {
  if(sendto(sockfd,&temp,sizeof(temp), 0, (struct sockaddr *)&cli_addr, clilen) == -1)
    {
      fprintf(stderr, "error while sending data!\n");
      exit(-1);
    }
  }
  close(sockfd);
  return 0;
}

