/*For sending files from client to server, the connection between client to server needs to be established. Then following algorithm is used:
  1. Firstly, it is checked whether input file is present or not. If present, it is opened.
  2. Secondly , the size of input file is calculated and transmitted to server.
  3. Next step is to send meta data to server like size of input file and name of input file.
  4. Then, next step is to read file and put 10 bytes in buffer at a time.For this,
  a]Read 1 byte at a time. If exactly 10 bytes are read from input file then it is placed into buffer and sent to server.
  b]If less than 10 bytes are read as the contents in the file are over and nothing is left to read,remaining bytes are read from file to buffer and sent to server.
*/
// User defined header file that contains all the header-file and structure used in this program

#include "header_file.h"                                                                                   
//Check if input file exists.
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
//Calculate size of file  
int file_size(FILE *fp)
{
	int sz;
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
	printf("the size of file is %d\n",sz);
	return sz;
}

void send_file(FILE *fp,int sock)
{
	char buff[10]="/0";
	memset(buff,0,sizeof(buff));
        //Setting the file pointer to top of the file
        fseek(fp, 0L, SEEK_SET);
        printf("Sending file\n");
        //Start sending file
        while(!feof(fp))
        {
                buff[0] = '\0';
                int i = 0;
                // Logic of sending 10 bytes
                for(i =0; i < 10 ; i++) {
                        fread(&buff[i],1,1,fp);
                        if(feof(fp)) { buff[i] = '\0';break; }
                }
                // Logic of sending less then 10 bytes
                if (i != 0) {
                        if(send(sock, buff, i, 0)<=0){
                                perror("send failed");
                                // Close the file
                                fclose(fp);
                                // Close the socket
                                close (sock);
                                exit(1);
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
	char buff[20]="/0";
	int rval;
	int count=0;
	int sz;	
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
	sock=socket(AF_INET,SOCK_STREAM,0);
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
	if(connect(sock, (struct sockaddr *) &client, sizeof(client))<0)
	{
		perror("connection failed");
		// Close the file
		fclose(fp);
		// Close the socket
		close (sock);
		exit(1);
	}

	// Send meta data to the server
	// 1. Output file name
	// 2. Size of file 
	if(send(sock,&id,sizeof(id),0)<=0)
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
	send_file(fp,sock);
	printf("File successfully sent\n");
	// Close the file
	fclose(fp);
	// Close the socket
	close (sock);
	return 0;
}


