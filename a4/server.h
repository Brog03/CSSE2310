#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <csse2310a4.h>
#include <semaphore.h>
#include <signal.h>
#include <FreeImage.h>
#include <stdbool.h>

#define RES_200 (int)200
#define RES_200_MSG (const char*)("OK")

#define RES_405 (int)405
#define RES_405_SIZE (unsigned long)(15)
#define RES_405_MSG (const char*)("Method Not Allowed")
#define RES_405_BODY (const unsigned char*)("Invalid method\n")

#define RES_404 (int)(404)
#define RES_404_SIZE (unsigned long)(16)
#define RES_404_MSG (const char*)("Not Found")
#define RES_404_BODY (const unsigned char*)("Invalid address\n")

#define RES_400 (int)(400)
#define RES_400_SIZE (unsigned long)(28)
#define RES_400_MSG (const char*)("Bad Request")
#define RES_400_BODY (const unsigned char*)("Invalid operation requested\n")

#define RES_422 (int)(422)
#define RES_422_SIZE (unsigned long)(24)
#define RES_422_MSG (const char*)("Unprocessable Content")
#define RES_422_BODY (const unsigned char*)("Unable to process image\n")

#define RES_501 (int)(501)
#define RES_501_SIZE (unsigned long)(19)
#define RES_501_MSG (const char*)("Not Implemented")
#define RES_501_BODY (const unsigned char*)("Operation failed: %s\n")

#define RES_413 (int)(413)
#define RES_413_SIZE (unsigned long)(32)
#define RES_413_MSG (const char*)("Payload Too Large")
#define RES_413_BODY                                                           \
    (const unsigned char*)("Image received is too large: %lu bytes\n")

#define NUM_STATS 5
#define STATS_CLIENTS_CONNECTED 0
#define STATS_CLIENTS_COMPLETED 1
#define STATS_HTTP_SUCCESSFUL 2
#define STATS_HTTP_FAILED 3
#define STATS_IMAGE_SUCCESSFUL 4

#define MAX_IMAGE_SIZE 8388608

#define MIN_ANGLE (-359)
#define MAX_ANGLE 359
#define MIN_SCALE 1
#define MAX_SCALE 10000

#define FAILED_OPERATION_LENGTH 7

#define BLOCK_SIZE 100
#define BYTE_SIZE sizeof(unsigned char)
#define TEMP_BUFFER_SIZE 50
#define LISTEN_SOCKET_MAX 100

typedef struct {
    int socket;
    struct addrinfo* ai;
} ListenSocket;

typedef struct {
    unsigned char* data;
    unsigned long size;
    long numOps;
    int status;
} HttpResponse;

typedef struct {
    long** stats;
    sem_t* semaphore;
    int fd;

} ThreadData;

typedef struct {
    long** stats;
    sem_t* semaphore;
    sigset_t* signalHandle;
} SignalThread;

void* client_thread(void*);
ListenSocket init_server(const char* port);
void process_connections(int fdServer, int maxConnections);
bool isdigit_str(char* input);

void construct_invalid_response(int status, char* data, HttpResponse* response);

int perform_image_operation(FIBITMAP** image, char** subOps);
long* init_stats(void);

void construct_valid_response(
        unsigned char* body, unsigned long size, HttpResponse* response);
void proccess_image_file(
        char* method, FIBITMAP** image, HttpResponse* response);
void send_response(FILE* writeEnd, HttpResponse* response);
HttpResponse* handle_http_request(char* method, char* address,
        unsigned char* body, unsigned long bodySize);
void read_home_page_into_buffer(HttpResponse* response);
void create_signal_handle(long** stats, sem_t* semaphore);
void* signal_thread(void* arg);
void print_stats(long** stats);

#endif
