#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<pthread.h>

pthread_t w2threads[2];
//#define BUFFER_SIZE 1024 

FILE *input;
int n; 
int count = 0; 
struct sockaddr_in serv_addr;  
struct sockaddr_in cli_addr;
int s;  
int noargs = 0;							
int sendq[20];
int *sendhead,*sendtail;
int sockfd;
int buffer;
int flag = 1;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

void *sender(void *arg)
{
  printf("Entering sender thread \n");
  socklen_t fromlen = sizeof(struct sockaddr_in);
  int buffer1 = sendq[*sendhead];
  int a;
  if(sendto(sockfd,&buffer1,sizeof(buffer1), 0, (struct sockaddr *)&serv_addr, fromlen) == -1)
    {
      fprintf(stderr, "error while sending data!\n");
      exit(-1);
    }
  printf("Sent data item = %d\n", buffer1);
  //pthread_mutex_lock(&m);
  *sendhead = (*sendhead+1)%20;
  buffer1 = sendq[*sendhead];
  while(flag)
    {
      
      while(*sendhead != *sendtail)
	{
	  buffer1 = sendq[*sendhead];
	  //printf("Sender says hello !\n");
	  if(sendto(sockfd,&buffer1,sizeof(buffer1), 0, (struct sockaddr *)&serv_addr, fromlen) == -1)
	    {
	      fprintf(stderr, "error while sending data!\n");
	      exit(-1);
	    }
	  printf("Sent data item = %d\n", buffer1);
	  //pthread_mutex_lock(&m);
	  *sendhead = (*sendhead+1)%20;
	  //buffer1 = sendq[*sendhead];
	  for(a=0;a<20;a++)
	    {
	      printf("%d ",sendq[a]);
	    }
	  printf("sendhead = %d, sendtail = %d\n",*sendhead, *sendtail);
	  printf("\n\n");	  

	  //pthread_mutex_unlock(&m);
	}
    }
  printf("Sender ending\n");
  return 0;
}


void *receiver(void *arg)
{
  printf("Entering receiver thread \n");
  int nbrecv = 0;
  int buffer2;
  struct sockaddr_in temp;
  socklen_t fromlen = sizeof(struct sockaddr_in);
  printf("started listening for first thing\n");
  if((nbrecv = recvfrom(sockfd, &buffer2,sizeof(buffer2), 0, (struct sockaddr *)&temp, &fromlen)) == -1)
    {
      fprintf(stderr, "fail while receiving data! \n");
      exit(-1);
    }
  printf("Received from server(receiver) = %d, sendhead = %d, sendtail = %d\n",buffer2,*sendhead, *sendtail );
  sendq[*sendtail] = buffer2;
  *sendtail = (*sendtail+1) % 20;
  //while(*sendhead != *sendtail)
  
  //printf("I am here and looping \n");
      while(*sendhead != (*sendtail +1 )%20 )
      {
  
      printf("Receiver says hello !\n");
      if((nbrecv = recvfrom(sockfd, &buffer2,sizeof(buffer2), 0, (struct sockaddr *)&temp, &fromlen)) == -1)
	{
	  fprintf(stderr, "fail while receiving data! \n");
	  exit(-1);
	}
	//if (*sendhead != *sendtail){
      printf("Received from server(receiver) = %d, sendhead = %d, sendtail = %d\n",buffer2,*sendhead, *sendtail );
      int a;
      printf("send q is : ");
  
      if(buffer2 == 20)
	{
	  flag = 0;
	  break;
	}
      //pthread_mutex_lock(&m);
      sendq[*sendtail] = buffer2;
      *sendtail = (*sendtail+1) % 20;
      for(a=0;a<20;a++)
	{
	  printf("%d ",sendq[a]);
	}
      printf("sendhead = %d, sendtail = %d\n",*sendhead, *sendtail);
      printf("\n\n");
	
      //pthread_mutex_unlock(&m);
  
    }
//}
   
  printf("Receiver ending\n");
  return 0;
}

int main(int argc , char *argv[])
{
  //int portno, n;
  //int buffer;
  struct hostent *server;
  //socklen_t fromlen;
  int portnoserv, portnocli;

  if (argc < 4) 
    {
      fprintf(stderr,"usage %s hostname port filename\n", argv[0]);
      exit(0);
    }

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");
  
  /* Sender setting struct sockaddr fields for receiver */
  
  server = gethostbyname(argv[1]);
  if (server == NULL)
    {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
    }
  portnoserv = atoi(argv[2]);
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
  serv_addr.sin_port = htons(portnoserv);

  /* Sender setting struct sockaddr fields for itself and to bind */

  bzero((char *) &cli_addr, sizeof(cli_addr));
  portnocli = atoi(argv[3]);
  cli_addr.sin_family = AF_INET;
  cli_addr.sin_addr.s_addr = INADDR_ANY;
  cli_addr.sin_port = htons(portnocli);
  if (bind(sockfd, (struct sockaddr *) &cli_addr,sizeof(cli_addr)) < 0) 
    error("ERROR on binding");
  
  
  //fromlen = sizeof(struct sockaddr_in);
  sendhead = malloc(sizeof(int));
  sendtail = malloc(sizeof(int));
  int e = 0,i;
  for(i=0;i<20;i++)
    sendq[i] = i;
  //sendq[4] = 4;
  //sendq[5] = 5;
  //sendq[6] = 6;
  //sendq[7] = 7;
  //sendq[8] = 8;
  //sendq[9] = 9;
  //sendq[4] = 1;
  //sendq[5] = 1;
  //sendq[6] = 1;
  //sendq[7] = 1;
  //sendq[8] = 1;
  *sendhead = 0;
  *sendtail = 0;
  printf("Send queue is : \n");
  for(i=0;i<20;i++)
    printf("%d ", sendq[i]);
  printf("\n");
  /*hostname = (char*)malloc(sizeof(256)); 
    portnum = (char*)malloc(sizeof(256));
    filename = (char*)malloc(sizeof(256));
    object = (char*)malloc(sizeof(256));
    portnumret = (char*)malloc(sizeof(256));*/
  
  e = pthread_create(&w2threads[0], NULL, sender,NULL);
  e = pthread_create(&w2threads[1], NULL, receiver, NULL);
  
  if (e)
    {
      printf("ERROR; return code from pthread_create() is %d\n", e);
      exit(-1);
    }
  pthread_join(w2threads[0],NULL);
  pthread_join(w2threads[1],NULL);
  printf("Send queue is : \n");
  for(i=0;i<20;i++)
    printf("%d ", sendq[i]);
  printf("\n");
  free(sendhead);
  free(sendtail); 
  printf("quit \n");
  return 0;	
}
