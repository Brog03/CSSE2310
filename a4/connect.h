#ifndef CONNECT_H
#define CONNECT_H

#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <csse2310a4.h>

#define DATA_ROTATE 1
#define DATA_SCALE 2
#define DATA_FLIP 3
#define DATA_NULL 0

#define RET_SCALE_X(num) ((num) & (int)0xFFFF0000) >> 16
#define RET_SCALE_Y(num) ((num) & (int)0x0000FFFF)
#define STO_SCALE_X(num) ((num) << 16)
#define STO_SCALE_Y(num) (num)

#define BYTE_SIZE (size_t)(sizeof(unsigned char))
#define BLOCK_SIZE (size_t)(10240)

#define RESPONSE_OK 200

#define HTTP_REQUEST_ROTATE                                                    \
    "POST /rotate,%d HTTP/1.1\r\n"                                             \
    "Content-Length: %d\r\n\r\n"

#define HTTP_REQUEST_FLIP                                                      \
    "POST /flip,%c HTTP/1.1\r\n"                                               \
    "Content-Length: %d\r\n\r\n"

#define HTTP_REQUEST_SCALE                                                     \
    "POST /scale,%d,%d HTTP/1.1\r\n"                                           \
    "Content-Length: %d\r\n\r\n"

typedef struct {
    unsigned char* data;
    int size;
    FILE* readEnd;
    FILE* writeEnd;

    struct addrinfo* ai;

} ImageHTTP;

ImageHTTP connect_to_server(const char* portNum);
void send_image_to_server(ImageHTTP request, int opData, int dataType);
int read_image_into_buffer(ImageHTTP* request);
int handle_http_response(ImageHTTP request);

#endif
