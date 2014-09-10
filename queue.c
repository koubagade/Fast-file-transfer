#include <stdio.h>
#include <stdlib.h>

#define SIZE 8
int main ()
{
	int send_Q[SIZE];
	int recv_Q[SIZE];
	int send_head, send_tail;
	int recv_head, recv_tail;
	int recv_counter = 7;
	int sent_data_id, not_matched_data_id;
	int i;
	for (i = 0; i < 7; i++)
	{
		recv_Q[i] = i;
	}
	send_Q[0] = 1;
	send_Q[1] = 2;
	send_Q[2] = 3;
	send_Q[3] = 4;
	send_Q[4] = 5;
	send_head = 0;
	recv_head = 0;
	send_tail = 5;
	recv_tail = 7;
	printf("starting queue program\n");
	
	
	while(recv_head != recv_tail)
	{
		sent_data_id = send_Q[send_head];
		printf("sent data id = %d \n", sent_data_id);
		send_head = (send_head + 1) % SIZE ;
		while (recv_counter != 0 && sent_data_id != recv_Q[recv_head])
		{
			not_matched_data_id = recv_Q[recv_head];
			recv_head = (recv_head + 1) % SIZE;
			printf("not matched %d %d\n", sent_data_id, not_matched_data_id);
			send_Q[send_tail] = not_matched_data_id;
			send_tail = (send_tail + 1 ) % SIZE;
			recv_Q[recv_tail] = not_matched_data_id;
			recv_tail = ( recv_tail + 1 ) % SIZE;
		}
		if (recv_counter == 0)
		{
			printf("File transferred successfully!\n");
			exit(1);
		}
		if (sent_data_id == recv_Q[recv_head])
		{
			recv_counter --;
			printf("piece received and matched %d %d, remaining = %d\n", sent_data_id, recv_Q[recv_head], recv_counter);
			recv_head = ( recv_head + 1 ) % SIZE;
			// Add file chunk received to chunk list
		}
		else
		{
			printf("ERROR! Check Logic");
			exit(1);
		}
	}
}
			 

