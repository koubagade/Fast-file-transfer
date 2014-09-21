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
#define BUFFER_SIZE 2944
#define TIMEOUT 250
#define QUEUE_SIZE 357023
#define HEADER_SIZE 7
//int n;

pthread_t w2threads[2];
FILE *output,*fp;
struct sockaddr_in serv_addr, cli_addr, cldummy, serv_addr1, cli_addr1;
int count = 0;
int sendq[QUEUE_SIZE];
int *sendhead,*sendtail;
int flag = 1;
int n; 
int recvq[QUEUE_SIZE], recvhead = 0, recvtail=0, recvcounter=QUEUE_SIZE;
int sockfd,sockfd1;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void *sender(void *arg)
{
	//FILE *fp;
	fp = fopen("output.bin","rb");
	if(NULL == fp)
    	{
        printf("\n fopen() Error!!!\n");
        exit (1);
    	}
	printf("Entering sender thread \n");
	socklen_t fromlen = sizeof(struct sockaddr_in);
	int whatisinqueue = 0 ;
	long int bytestoberead = 0;
	char str[HEADER_SIZE];
	char buffer1[BUFFER_SIZE];
	char *total;
	whatisinqueue = sendq[*sendhead];
	bytestoberead = whatisinqueue * (BUFFER_SIZE - HEADER_SIZE);
	printf("what is in queue and bytes to be read are %d and %d\n",whatisinqueue, (int)bytestoberead);
	if (fseek(fp,bytestoberead, SEEK_SET) != 0)
	{
		fprintf(stderr, "error while seeking file\n");
	}
	printf("meh\n");
	printf("size of buffer 1 %lu \n", sizeof(buffer1));
	if ((n = fread(&buffer1, 1, BUFFER_SIZE - HEADER_SIZE , fp)) < 0 )
	{
		printf("error in fread\n");
	}
	// itoa(whatisinqueue, str, 10);
	//printf("buffer = %s \n",buffer1);
	printf("READ data bytes = %d \n", n);
	snprintf(str,HEADER_SIZE,"%d",whatisinqueue);
	printf("str = %s\n",str);
	total = malloc(BUFFER_SIZE + 1);
	memcpy(total, str, HEADER_SIZE);
	memcpy(total+HEADER_SIZE, buffer1, sizeof(buffer1));
	//total[1472] = '\0';
	printf("total = %s\n",total);
	//int buffer1 = sendq[*sendhead];
	int a;
	if(sendto(sockfd1, total, BUFFER_SIZE, 0, (struct sockaddr *)&cli_addr1, fromlen) == -1)
	{
		fprintf(stderr, "error while sending data!\n");
		exit(-1);
	}
	printf("Sent data item = %s\n", total);
	//pthread_mutex_lock(&m);
	*sendhead = (*sendhead+1)% QUEUE_SIZE;
	free(total);
	//buffer1 = sendq[*sendhead];
	while(flag)
	{
		while(*sendhead != *sendtail)
		{
			whatisinqueue = sendq[*sendhead];
			bytestoberead = whatisinqueue * (BUFFER_SIZE - HEADER_SIZE);
			if (fseek(fp,bytestoberead, SEEK_SET) != 0)
			{
				fprintf(stderr, "error while seeking file\n");
			}
			if ((n = fread(&buffer1, 1, BUFFER_SIZE - HEADER_SIZE , fp)) < 0 )
			{
				printf("error in fread\n");
			}
			// itoa(whatisinqueue, str, 10);
			snprintf(str,HEADER_SIZE,"%d",whatisinqueue);
			total = malloc(BUFFER_SIZE +1);
			memcpy(total, str, HEADER_SIZE);
			memcpy(total+HEADER_SIZE, buffer1, (n+HEADER_SIZE));
			//buffer1 = sendq[*sendhead];
			//printf("Sender says hello !\n");
			printf("sizeof total = %lu, The buffer being sent is %s\n",sizeof(total), total);
			if(sendto(sockfd1, total, n+HEADER_SIZE, 0, (struct sockaddr *)&cli_addr1, fromlen) == -1)
			{
				fprintf(stderr, "error while sending data!\n");
				exit(-1);
			}
			printf("Sent data item = %s\n", total);
			//for (a = 0; a < n+HEADER_SIZE -1; a++)
			//{
			//	printf("%c", total[a]);
			//}
			printf("\n\n DONE\n\n");
			//pthread_mutex_lock(&m);
			*sendhead = (*sendhead+1)%QUEUE_SIZE;
			free(total);
			//buffer1 = sendq[*sendhead];
			//for(a=0;a<QUEUE_SIZE;a++)
			//{
			//	printf("%d ",sendq[a]);
			//}
			printf("\nsendhead = %d, sendtail = %d\n",*sendhead, *sendtail);
			printf("\n\n");	  

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
	printf("see\n");
	if((nbrecv = recvfrom(sockfd1, &buffer2,sizeof(buffer2), 0, (struct sockaddr *)&temp, &fromlen)) == -1)
	{
	  fprintf(stderr, "fail while receiving data! \n");
	  exit(-1);
	}
	printf("Received from client(receiver) = %d, sendhead = %d, sendtail = %d\n",buffer2,*sendhead, *sendtail );
	sendq[*sendtail] = buffer2;
	*sendtail = (*sendtail+1) % QUEUE_SIZE;
	while(*sendhead != *sendtail || buffer2 != QUEUE_SIZE)
	{

	printf("Receiver says hello !\n");
	if((nbrecv = recvfrom(sockfd1, &buffer2,sizeof(buffer2), 0, (struct sockaddr *)&temp, &fromlen)) == -1)
	{
	  fprintf(stderr, "fail while receiving data! \n");
	  exit(-1);
	}
	printf("Received from client(receiver) = %d, sendhead = %d, sendtail = %d\n",buffer2,*sendhead, *sendtail );
	int a;
	//printf("send q is : ");

	if(buffer2 == QUEUE_SIZE)
	{
		flag = 0;
		break;
	}
	//pthread_mutex_lock(&m);
	sendq[*sendtail] = buffer2;
	*sendtail = (*sendtail+1) % QUEUE_SIZE;
	
	//for(a=0;a<QUEUE_SIZE;a++)
	//{
	//	printf("%d ",sendq[a]);
	//}
	printf("sendhead = %d, sendtail = %d\n",*sendhead, *sendtail);
	printf("\n\n");

	//pthread_mutex_unlock(&m);

	}

	printf("Receiver ending\n");
	return 0;
}


int main(int argc, char *argv[])
{	
	int i;
	int portserv ,portcli,portserv1 ,portcli1;
	struct hostent *client;
	char buffer[BUFFER_SIZE];
	int buffer1;
	int nbrecv = 0;   
	int a;
	char *file_start;
	char total[BUFFER_SIZE];
	char recv_pid[HEADER_SIZE];
	int last_sent;
	for(a=0;a<QUEUE_SIZE;a++)
		recvq[a]=a; 
	//recvq[10]=10;
	
	output = fopen("output.bin", "wb"); 
	
		
	time_t start, end;
	fd_set socks;
	struct timeval t;
	FD_ZERO(&socks);
	int received_pid;
	t.tv_usec = 500000;
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
	portserv1 = atoi(argv[5]);
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portserv);
	
	bzero((char *) &serv_addr1, sizeof(serv_addr1));
	serv_addr1.sin_family = AF_INET;
	serv_addr1.sin_addr.s_addr = INADDR_ANY;
	serv_addr1.sin_port = htons(portserv1);
	
	
	client = gethostbyname(argv[1]);
	
	portcli = atoi(argv[2]);
	portcli1 = atoi(argv[4]);
	
	if(client == NULL)
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	bzero((char *) &cli_addr, sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	bcopy((char *)client->h_addr,(char *)&cli_addr.sin_addr.s_addr,client->h_length);
	cli_addr.sin_port = htons(portcli);
	
	
	bzero((char *) &cli_addr1, sizeof(cli_addr1));
	cli_addr1.sin_family = AF_INET;
	bcopy((char *)client->h_addr,(char *)&cli_addr1.sin_addr.s_addr,client->h_length);
	cli_addr1.sin_port = htons(portcli1);
	
	socklen_t fromlendummy = sizeof(struct sockaddr_in);
	socklen_t clilen = sizeof(struct sockaddr_in);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	error("ERROR on binding");
	int p = 1;
	/* Main Code Starts listening on socket */
	while(recvhead != recvtail || p)
    { 
		/*for(i=0;i<QUEUE_SIZE;i++)
		printf("%d ", recvq[i]);
		printf("\n\n\n");*/
		printf ("Recvhead and Recv tail are %d and %d\n",recvhead,recvtail);
		
		while(1)
		{
			FD_ZERO(&socks);
			FD_SET(sockfd, &socks);
			t.tv_usec = 500000;
			t.tv_sec = 0;
			i = select(sockfd + 1, &socks, NULL, NULL, &t);
		
			if ( i > 0 || p) 
			{
				if(FD_ISSET(sockfd, &socks))
				{
					if ((nbrecv = recvfrom(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cldummy, &fromlendummy)) == -1)  
					{
						fprintf(stderr, "fail while receiving data!, Resending last NAK \n");
						exit(-1);
					}
					p = 0;
					//printf("\n%c, %c , %c\n", buffer[0],buffer[1], buffer[2]);
					for (a = 0; a < HEADER_SIZE ; a++)
					{
						recv_pid[a] = buffer[a];
					}
					//recv_pid[1] = buffer[1];
					//recv_pid[2] = buffer[2];
					//file_start = &buffer[3];
					received_pid = atoi(recv_pid);
					printf("printing buffer\n");
					/*for (a = HEADER_SIZE; a < nbrecv; a++)
					{
						printf("%c",buffer[a]);
						
					}*/
					
					printf("buffer received is : \n %s \n buffer size =%lu\n", buffer, sizeof(buffer));
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
					recvtail = (recvtail+1)%QUEUE_SIZE;
					recvhead = (recvhead+1)%QUEUE_SIZE;
							
				}
			}
			else if (i < 0)
			{
				printf("Error in Select\n");
				exit(0);
			}
		}
		printf("Received from sender(client) = %d\n",received_pid);
		while(recvq[recvhead] != received_pid)
		{
			//printf("Sending from server\n");
			buffer1 = recvq[recvhead];
			recvq[recvtail] = buffer1;
			recvtail = (recvtail+1)%QUEUE_SIZE;
			//recvq[recvtail] = buffer1;
			recvhead = (recvhead+1)%QUEUE_SIZE;
			printf("Sending %d from buffer \n",buffer1);
			if(sendto(sockfd,&buffer1,sizeof(buffer1), 0, (struct sockaddr *)&cli_addr, clilen) == -1)
			{
				fprintf(stderr, "error while sending data!\n");
				exit(-1);
			}
		}
		if(recvq[recvhead] == received_pid)
		{
			recvcounter --;
			recvhead = (recvhead+1)%QUEUE_SIZE;
			printf("Received pid %d receive counter = %d  \n",received_pid, recvcounter);
			if(fseek(output, (long int)(received_pid)*(BUFFER_SIZE - HEADER_SIZE), SEEK_SET) !=0 )
			{
				printf("error while seeking file\n");
			}
			if(fwrite(buffer+HEADER_SIZE, 1, nbrecv -HEADER_SIZE, output) < 0 )
			{
				printf("error while writing file\n");
			}
			 
		}
		if (recvcounter == 0)
		{
			break;
		}
		
	}
   
	//int i;
	/*for(i=0;i<QUEUE_SIZE;i++)
		printf("%d ", recvq[i]);
	printf("\n");*/
	printf ("Recvhead and Recv tail on quitting are %d and %d\n",recvhead,recvtail);

	int temp = QUEUE_SIZE;
	for(i=0;i<5;i++)
	{
		if(sendto(sockfd,&temp,sizeof(temp), 0, (struct sockaddr *)&cli_addr, clilen) == -1)
		{
			fprintf(stderr, "error while sending data!\n");
			exit(-1);
		}
	}
	fclose(output);
	close(sockfd);
	
	sockfd1 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd1 < 0) 
    error("ERROR opening socket");

    if (bind(sockfd1, (struct sockaddr *) &serv_addr1,sizeof(serv_addr1)) < 0) 
    error("ERROR on binding");
	
	
	
	
	
	
	


    //RE TRANSMISSION STARTS HERE//
  
	printf("End of transmission \n\n\n");
	usleep(5000000);
  
  
  /*Receiver re transmitting after recovering the file */
  
    sendhead = malloc(sizeof(int));
	sendtail = malloc(sizeof(int));
	int e = 0;
	for(i=0;i<QUEUE_SIZE;i++)
	sendq[i] = i;
	*sendhead = 0;
	*sendtail = 0;
	printf("Send queue is : \n");
	for(i=0;i<QUEUE_SIZE;i++)
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
	for(i=0;i<QUEUE_SIZE;i++)
	printf("%d ", sendq[i]);
	printf("\n");
	free(sendhead);
	free(sendtail); 

	
	 
	close(sockfd1);
	printf("quit \n");
	
	return 0;
}


