/* After recieving meta data i.e size of input file and name of input file, the copying of buff into output file begins. This happens using following algorithm:
   1.Checksum is calculated on the 10 bytes recieved from client.
   2.This checksum is compared with the checksum recieved , if its same then the corrosponding ack will be sent, otherwise nothing will be sent.
   3.Flip function is used to flip the acknowledgement.
 */
#include "header_file.h"
#include <assert.h>
// Function to flip the ack
void flip(int * ptr)
{
	if(*ptr == 0){
		*ptr = 1;
	} else {
		*ptr = 0;
	}
}
// Function to calculate checksum
unsigned int checksum(void *buffer, size_t len)
{
	unsigned int seed = 0;
	unsigned char *buf = (unsigned char *)buffer;
	size_t i;

	for (i = 0; i < len; ++i)
		seed += (unsigned int)(*buf++);
	return seed;
}
// mMain function begins
int main(int argc,char *argv[])
{
	input_data_t id;
	int sock;
	struct sockaddr_in server;
	int mysock;
	char buff[5];
	int rval;
	int count=0;
	FILE *fp1;
	m_h msg_hdr;
	ack ack_msg;
	int slen = sizeof(server);
	//word16  x;
	if(argc!=2)
	{
		printf("Enter the port number\n");
		exit(1);
	}
	//Create socket
	sock=socket(AF_INET,SOCK_DGRAM,0);
	if(sock <0){
		perror("Failed to access socket\n");
		close(sock);
		exit(1);
	}
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(atoi(argv[1]));
	//Binding the socket
	if(bind(sock, (struct sockaddr *)&server,sizeof(server))){
		perror("Bind Failed\n");
		close(sock);
		exit(1);
	}

	// Recieving output file name and size of input file name	
	mysock=sock;
	if ((rval=recvfrom(mysock,&id,sizeof(id),0, &server, &slen))<0){
		perror("Reading Stream Message Error\n");
	} else if(rval==0){
		printf("Ending connection\n");
	} else {
		printf("MSG RECIEVED: %s %d\n",id.file_name, id.size);
	}
	memset(buff,0,sizeof(buff));
	printf("File copying start\n");
	// Open the Output File
	fp1=fopen(id.file_name, "wb");
	int read_bytes = 0;
	while(read_bytes < id.size) {
		if ((rval=recvfrom(mysock,&msg_hdr, sizeof(msg_hdr), 0,&server, &slen))<0)
			perror("Reading Stream Message Error\n");
		else if (rval==0)
			printf("Ending connection\n");
		else {
			unsigned int x = checksum(msg_hdr.buff, msg_hdr.size);
			// If the calculated checksum is same as that of recieved checksum then send corrosponding ack 
			if(x == msg_hdr.checksum){
				printf("Checksum calculated : %d\n",x);
				printf("Valid MSG %d Received: size:%d checksum:%d",msg_hdr.id, msg_hdr.size, msg_hdr.checksum);
				ack_msg.id = msg_hdr.id;
				int random_n = (random()%10000)/1000;
				if (random_n>5)	
				printf("   Packet lost\n");
				else{
				printf("Sending ACK %d\n", ack_msg.id);
				if ((rval=sendto(mysock, &ack_msg, sizeof(ack_msg), 0, &server, slen))<0)
					perror("Reading Stream Message Error\n");
				else if(rval==0)
					printf("Ending connection\n");
				else
					fwrite(&msg_hdr.buff[0],msg_hdr.size,1,fp1);
				
				printf("ACK SENT\n");
				read_bytes += msg_hdr.size;
				}
			}
			else {
				printf("\nInvalid MSG Received.\n");
			}	
		}
	}	
	

	printf("File copying complete\n");
	fclose(fp1);
	close(mysock);
	return 0;
}


