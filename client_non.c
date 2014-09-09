#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>

#define PORT 9333
#define BUFFER_SIZE 1024

int n;
FILE *output;

struct sockaddr_in local, remote;
int len_remote = sizeof(remote);
int s;
int count = 0;

int main(){

char buffer[BUFFER_SIZE];
output = fopen("output.txt", "w");

if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
    fprintf(stderr, "Error getting a socket!\n");
    exit(-1);
}

memset((char *) &remote, 0, sizeof(remote));
memset((char *) &local, 0, sizeof(remote));

local.sin_family = AF_INET;
local.sin_port = htons(PORT);
local.sin_addr.s_addr = htonl(INADDR_ANY);

if(bind(s, (struct sockaddr *)&local, sizeof(local)) == -1){
    fprintf(stderr, "Cannot connect to service!\n");
    exit(-1);
}
int flag = 1;
int   nbrecv = 0;    
while(flag == 1){

    memset(buffer, '\0', BUFFER_SIZE);

    if((nbrecv = recvfrom(s, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&remote, &len_remote)) == -1){
        fprintf(stderr, "fail while receiving data! \n");
        exit(-1);
    }

    count += nbrecv;
    //printf("n = %d\n", nbrecv);

    fwrite(buffer, 1, nbrecv, output);

    if(nbrecv<1024){
        flag = 0;
    }

}
printf("Got the file!\n");

close(s);
fclose(output);
return 0;

}
