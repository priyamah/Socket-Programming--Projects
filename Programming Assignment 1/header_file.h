#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
// structure to send meta data to server
typedef struct input_data_ {
      char file_name[20];
      int size;
}input_data_t;
