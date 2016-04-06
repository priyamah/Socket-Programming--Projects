/*For sending files from client to server, the connection between client to server needs to be established. Then following algorithm is used:
  1. Firstly, it is checked whether input file is present or not. If present, it is opened.
  2. Secondly , the size of input file is calculated and transmitted to server.
  3. Next step is to send meta data to server like size of input file and name of input file.
  4. Then, next step is to read file and put 10 bytes in buffer at a time.For this,
  a]Read 1 byte at a time. If exactly 10 bytes are read from input file then it is placed into buffer and sent to server.
  b]If less than 10 bytes are read as the contents in the file are over and nothing is left to read,remaining bytes are read from file to buffer and sent to server.
  5. Select function is used to determine the timeout which is set to 1 seconds.
  6. Checksum is calculated and it is send to the server for verification.
  7. UDP connection is used between client and server.
 */
// User defined header file that contains all the header-file and structure used in this program
#include "header_file.h"
// Function to flip the acknowledgement.
void flip(int * ptr)
{
	if(*ptr == 0){
		*ptr = 1;
	} else {
		*ptr = 0;
	}
}
// Function to open the file
FILE * file_open(char *x)
{
	FILE *fp = fopen(x, "rb");
	if(!fp) {
		perror("File does not exists");
		// Close the file
		fclose(fp);
		exit(1);
	}
	return fp;
}
//calculate checksum
unsigned int checksum(void *buffer, size_t len)
{
	unsigned int seed = 0;
	unsigned char *buf = (unsigned char *)buffer;
	size_t i;

	for (i = 0; i < len; ++i)
		seed += (unsigned int)(*buf++);
	return seed;
}

//Calculate size of file  
int file_size(FILE *fp)
{
	int sz;
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
	printf("the size of file is %d\n",sz);
	return sz;
}
// Logic of sending file in 10 bytes
void send_file(FILE *fp,int sock, struct sockaddr_in si_other)
{
	fd_set readfds;    // descriptor set
	t_m_ timeout;
	timeout.tv_sec = 1;         // Set the time out value (5 sec)
	timeout.tv_usec = 0;
	fd_set select_fds;
	FD_ZERO( &readfds );        // Reset all bits
	FD_SET ( sock, &readfds );
	printf("socket number %d\n", sock);
	ack ack_msg;
	m_h msg_header;
	int slen=sizeof(si_other);
	int read_from_file = True;
	memset(msg_header.buff,0,sizeof(msg_header.buff));
	//Setting the file pointer to top of the file
	fseek(fp, 0L, SEEK_SET);
	// Compute the 16-bit checksum
	printf("Sending file\n");
	int id = 0;
	int i = 0;
	int rval;
	//Start sending file
	while(!feof(fp))
	{
		FD_ZERO( &readfds );        // Reset all bits
		FD_SET ( sock, &readfds );
		// Logic of sending 10 bytes
		if(read_from_file == True) {
			for(i = 0; i < 10 ; i++) {
				fread(&(msg_header.buff[i]),1,1,fp);
				if(feof(fp)) { msg_header.buff[i] = '\0';break; }
			}
		}

		// Logic of sending less then 10 bytes
		if (i != 0) {
			// Compute the 16-bit checksum
			// random function is used to introduce erroe in checksum
			int random_n = (random()%10000)/1000;
			if (random_n >8)
			{
				msg_header.checksum = checksum(msg_header.buff, 0);
			}
			else
			{
				msg_header.checksum = checksum(msg_header.buff, i);
			}
			msg_header.size=i;
			msg_header.id = id;
			printf("Msg Send: Id:%d size:%d checksum:%d\n", msg_header.id, msg_header.size, msg_header.checksum);
			if(sendto(sock, &msg_header, sizeof(msg_header), 0,&si_other, slen) <= 0){
				perror("send failed");
				// Close the file
				fclose(fp);
				// Close the socket
				close (sock);
				exit(1);
			}

			printf("Waiting for message\n");
			//select function is used for timeout packets
			if ( select ( 32, &readfds, NULL, NULL, &timeout ) == 0 )
			{	read_from_file=False;	
				timeout.tv_sec = 0.01;         // Set the time out value (1 sec)
				timeout.tv_usec = 0;
				printf("\t\t\t\t\t\tTIME OUT: RESENDING\n");          // Re-Send data
			} else {

				//Receive Acknowledgement
				if ((rval=recvfrom(sock,&ack_msg,sizeof(ack_msg),0, &si_other, &slen))<0){
					perror("Reading Stream Message Error\n");
				} else if(rval==0){
					printf("Ending connection\n");
				} else {
					printf("ACK %d RECIEVED.\n",ack_msg.id);
				}	
				flip(&id);
				read_from_file=True;
			}
		}

	}
}

//Main function begins
int main(int argc,char *argv[])
{
	int sock;
	struct sockaddr_in client;
	FILE *fp;
	struct hostent *hp;
	char buff[10]="/0";
	int rval;
	int count=0;
	int sz;	
	struct sockaddr_in si_me; 
	int slen = sizeof(client);

	printf("The number of arguments are %d %s %s\n",argc, argv[1], argv[2]);
	if(argc<=4)
	{	
		perror("the arguments are less\n");
		exit(1);
	}
	fp=file_open(argv[1]);
	sz=file_size(fp);

	input_data_t id;
	// copy the output file name to the structure 
	strcpy(id.file_name, argv[2]);
	id.size = sz;
	//Create socket
	sock=socket(AF_INET,SOCK_DGRAM,0);
	if(sock <0)
	{
		perror("Failed to access socket");
		// Close the file
		fclose(fp);
		// Close the socket
		close (sock);
		exit(1);
	}
	client.sin_family=AF_INET;
	// Connection establishment if client and server are running on the same machine
	if(strcmp(argv[3],"localhost")==0)
	{
		if (strcmp(argv[1],argv[2])==0){
			perror("Trying to overwrite the file on the same machine \n");
			// Close the file
			fclose(fp);
			// Close the socket
			close (sock);
			exit(1);
		}	
		hp=gethostbyname(argv[3]);
		if(hp==0)
		{
			perror("gethostbyname failed");
			// Close the file
			fclose(fp);
			// Close the socket
			close (sock);
			exit(1);
		}
		memcpy(&client.sin_addr,hp->h_addr,hp->h_length);
	} else
		// Connection establishment if client and server  both are running on different machine
	{
		client.sin_addr.s_addr = inet_addr(argv[3]);

	}
	//Binding the client to a port number
	client.sin_port=htons(atoi(argv[4]));
	//Checking Connection
	// Send meta data to the server
	// 1. Output file name
	// 2. Size of file 
	if(sendto(sock,&id,sizeof(id),0,&client, slen)<=0)
	{
		//close socket before exiting the program
		perror("send failed");
		// Close the file
		fclose(fp);
		// Close the socket
		close (sock);
		exit(1);
	}
	printf("Checking for Connection establishment:The data sending is %s %d \n",id.file_name, id.size); 
	send_file(fp,sock, client);
	printf("File successfully sent\n");
	// Close the file
	fclose(fp);
	// Close the socket
	close (sock);
	return 0;
}


