/* After recieving meta data i.e size of input file and name of input file, the copying of buff into output file begins. This happens using following algorithm:
1. Firstly size of input file is divided by 5, so that it can be determined how many time 5 bytes of data is required to read from buff. After reading data should be written to output file.
2. If size of input file % 5 is equal to zero then buff is copied fully and wriiten into output file.
3. But if the size of input file % 5 is not equal to zero then buffer is not empty and we need to read the remaining bytes from buffer to output file */
#include "header_file.h"
//Starting main function
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
	if(argc!=2)
	{
		printf("Enter the port number\n");
		exit(1);
	}
	//Create socket
	sock=socket(AF_INET,SOCK_STREAM,0);
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

	printf("Waiting for connection\n");
	listen(sock,5);
	mysock=accept(sock, (struct sockaddr *)0,0);
	if(mysock ==-1) {
		perror("accept failed\n");
		// Close the socket
		close(sock);
	} else {
		// Recieving output file name and size of input file name	
		if ((rval=recv(mysock,&id,sizeof(id),0))<0){
			perror("Reading Stream Message Error\n");
		}
		else if(rval==0){
			printf("Ending connection\n");
		}	
		else
			printf("MSG RECIEVED: %s %d\n",id.file_name, id.size);
		memset(buff,0,sizeof(buff));
		printf("File copying start\n");
		// Open the Output File
		fp1=fopen(id.file_name, "wb");
		// Reading the 5 bytes from buffer and  writing to Output File
		for(int i=0;i<id.size/5;i++){	
			if ((rval=recv(mysock,buff, 5, 0))<0)
				perror("Reading Stream Message Error\n");
			else if(rval==0)
				printf("Ending connection\n");
			else
				fwrite(&buff[0],5,1,fp1);
		}

		if(id.size % 5 > 0) {	
			// Writing less than 5 bytes 
			if ((rval=recv(mysock, buff, id.size%5, 0))<0){
				perror("Reading Stream Message Error\n");
			}
			else if(rval==0)
				printf("Ending connection\n");
			else 
				fwrite(&buff[0],id.size%5, 1, fp1);
		}
		// Close the output File
                printf("File copying complete\n");
		fclose(fp1);
                close(mysock);
	}
	// Close the socket
	close(sock);
	return 0;
}


