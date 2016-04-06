#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#define True 1
#define False 0
// structure to send meta data to server
typedef struct input_data_ {
      char file_name[20];
      int size;
}input_data_t;
typedef struct msg_header{
        int size;
        int id;
        unsigned char buff[10];
        unsigned int checksum;
}m_h;
typedef struct acknowledgement
{
	int id;
}ack;
typedef struct timeval_time {
    int tv_sec;     // seconds
    int tv_usec;    // microseconds
}t_m_; 

