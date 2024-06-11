#include "server.h"
#include "image.h"

// init_server()
//
// Listens on given port. Returns listening socket (or exits on failure)
ListenSocket init_server(const char* port)
{
    struct addrinfo* ai = 0;
    struct addrinfo hints;
    struct sockaddr_in ad;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // listen on all IP addresses
    ListenSocket serverSocket = {-1, ai};

    if (!getaddrinfo(NULL, port, &hints, &ai)) {

        // Create a socket and bind it to a port
        // 0=default protocol (TCP)
        int listenfd = socket(AF_INET, SOCK_STREAM, 0);

        // Allow address (port number) to be reused immediately
        int optVal = 1;
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(int))
                >= 0) {

            if (bind(listenfd, ai->ai_addr, sizeof(struct sockaddr)) >= 0) {
                if (listen(listenfd, LISTEN_SOCKET_MAX) >= 0) {
                    serverSocket.socket = listenfd;
                    memset(&ad, 0, sizeof(struct sockaddr_in));
                    socklen_t len = sizeof(struct sockaddr_in);
                    getsockname(listenfd, (struct sockaddr*)&ad, &len);
                    fprintf(stderr, "%u\n", ntohs(ad.sin_port));
                }
            }
        }
    }

    // Have listening socket - return it
    return serverSocket;
}

// proccess_connection ()
//
// Listens and accepts new connections, creating a thread to handle
// each connection
void process_connections(int fdServer, int maxConnections)
{
    int fd;
    bool running = true;
    struct sockaddr_in fromAddr;

    long* stats = init_stats();
    sem_t semaphore;
    sem_init(&semaphore, 0, 1);

    socklen_t fromAddrSize;
    fromAddrSize = sizeof(struct sockaddr_in);
    // Create Signal Handling thread
    create_signal_handle(&stats, &semaphore);

    while (running) {
        if (stats[STATS_CLIENTS_CONNECTED] < maxConnections
                || maxConnections == 0) {
            // Maximum number of connections haven't been reached
            // Accetp the connection
            fd = accept(fdServer, (struct sockaddr*)&fromAddr, &fromAddrSize);

            char hostname[NI_MAXHOST];
            getnameinfo((struct sockaddr*)&fromAddr, fromAddrSize, hostname,
                    NI_MAXHOST, NULL, 0, 0);

            pthread_t threadID;
            ThreadData* data = malloc(sizeof(ThreadData));
            // Wait to update the number of clients connected
            sem_wait(&semaphore);
            stats[STATS_CLIENTS_CONNECTED]++;
            sem_post(&semaphore);

            // Create new Thread
            data->fd = fd;
            data->semaphore = &semaphore;
            data->stats = &stats;
            pthread_create(&threadID, NULL, client_thread, data);
            pthread_detach(threadID);
        }
    }
}

// create_signal_handle()
//
// Inititialises the signal handling thread
void create_signal_handle(long** stats, sem_t* semaphore)
{
    // Initilaise signal thread data
    SignalThread* signalData = malloc(sizeof(SignalThread));

    signalData->stats = stats;
    signalData->semaphore = semaphore;
    pthread_t threadID;

    // Initialise signal mask for both SIGPIPE and SIGHUP
    sigset_t* mask = malloc(sizeof(sigset_t));
    sigemptyset(mask);
    sigaddset(mask, SIGHUP);
    sigaddset(mask, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, mask, NULL);

    signalData->signalHandle = mask;

    // Create and detach signal handling thread
    pthread_create(&threadID, NULL, signal_thread, signalData);
    pthread_detach(threadID);
}

// signal_thread()
//
// target for the signal thread, ignores SIGPIPE and prints stats on SIGHUP
void* signal_thread(void* arg)
{
    SignalThread data = *((SignalThread*)arg);
    long** stats = data.stats;
    sem_t* semaphore = data.semaphore;
    sigset_t* signalHandle = data.signalHandle;

    int signal;
    while (1) {
        sigwait(signalHandle, &signal);

        if (signal == SIGHUP) {
            // SIGHUP recieved, lock semaphore and print stats
            sem_wait(semaphore);
            print_stats(stats);
            sem_post(semaphore);
        }
    }

    return NULL;
}

// print_stats()
//
// prints out the current stats of the system
void print_stats(long** stats)
{

    fprintf(stderr, "Connected clients: %ld\n",
            (*stats)[STATS_CLIENTS_CONNECTED]);
    fprintf(stderr, "Num completed clients: %ld\n",
            (*stats)[STATS_CLIENTS_COMPLETED]);
    fprintf(stderr, "Successfully processed HTTP requests: %ld\n",
            (*stats)[STATS_HTTP_SUCCESSFUL]);
    fprintf(stderr, "HTTP requests unsuccessful: %ld\n",
            (*stats)[STATS_HTTP_FAILED]);
    fprintf(stderr, "Image operations completed: %ld\n",
            (*stats)[STATS_IMAGE_SUCCESSFUL]);
}

// client_thread()
//
// target for regular client thread
void* client_thread(void* arg)
{
    // Extract all necessary data from arg
    ThreadData data = *((ThreadData*)arg);
    int fd = data.fd;
    sem_t* semaphore = data.semaphore;
    long** stats = data.stats;
    free(arg);

    FILE* writeEnd = fdopen(dup(fd), "w");
    FILE* readEnd = fdopen(fd, "r");

    char* method;
    char* address;
    HttpHeader** headers;
    unsigned char* body;
    unsigned long bodySize;
    HttpResponse* response = NULL;

    while (get_HTTP_request(
            readEnd, &method, &address, &headers, &body, &bodySize)) {
        // Recieved http request
        // Create a response
        response = handle_http_request(method, address, body, bodySize);
        // Send response
        send_response(writeEnd, response);

        // Update the statistics
        if (response != NULL && response->status == RES_200) {
            // Response was 200 OK
            (*stats)[STATS_HTTP_SUCCESSFUL]++;
            (*stats)[STATS_IMAGE_SUCCESSFUL] += (response->numOps);
        } else if (response != NULL) {
            // HTTP request was invalid or image operatiosn failed
            (*stats)[STATS_HTTP_FAILED]++;
            (*stats)[STATS_IMAGE_SUCCESSFUL] += (response->numOps);
        }
    }

    // Closes fds
    fclose(readEnd);
    fclose(writeEnd);

    // Update stats
    sem_wait(semaphore);
    (*stats)[STATS_CLIENTS_CONNECTED]--;
    (*stats)[STATS_CLIENTS_COMPLETED]++;
    sem_post(semaphore);

    return NULL; // could have called pthread_exit(NULL);
}

// handle_http_request()
//
// Proccess the http request and constructs either a 200OK response or
// necessary invalid response
HttpResponse* handle_http_request(char* method, char* address,
        unsigned char* body, unsigned long bodySize)
{

    FIBITMAP** image = malloc(sizeof(FIBITMAP*));
    HttpResponse* response = malloc(sizeof(HttpResponse));
    response->data = NULL;
    response->numOps = 0;
    response->status = 0;

    if (address[0] == '/') {
        if (strcmp(method, "GET") == 0) {
            if (!(strcmp(address, "/") == 0)) {
                construct_invalid_response(RES_404, NULL, response);
            } else {
                response->status = RES_200;
                read_home_page_into_buffer(response);
            }
        } else if (strcmp(method, "POST") == 0) {
            if (bodySize <= MAX_IMAGE_SIZE) {
                *image = fi_load_image_from_buffer(body, bodySize);
                proccess_image_file(address, image, response);

                if (response->data == NULL) {
                    unsigned long newBodySize;
                    unsigned char* newBody
                            = fi_save_png_image_to_buffer(*image, &newBodySize);
                    construct_valid_response(newBody, newBodySize, response);
                }
            } else {
                construct_invalid_response(RES_413, (char*)bodySize, response);
            }
        } else {
            construct_invalid_response(RES_405, NULL, response);
        }
    } else {
        construct_invalid_response(RES_400, NULL, response);
    }

    return response;
}

// read_home_page_into_buffer()
//
// reads the home page into the response buffer
void read_home_page_into_buffer(HttpResponse* response)
{
    HttpHeader** headers = malloc(sizeof(HttpHeader*) * 2);
    HttpHeader contentType = {"Content-Type", "text/html"};
    headers[0] = &contentType;
    headers[1] = NULL;

    // Assumes file size is 10240 bytes
    size_t fileSize = 0;
    size_t bufferSize = BLOCK_SIZE; // Assume File size is 100 bytes
    // Since the HTML file wont have DRASTIC chnages, changes should be
    // less than rougly 100 bytes

    // Initialises the buffer
    unsigned char* buffer = malloc(sizeof(unsigned char) * bufferSize);
    FILE* homePageFd
            = fopen("/local/courses/csse2310/resources/a4/home.html", "r");

    size_t bytesRead = 0;
    while ((bytesRead = fread(
                    buffer + fileSize, BYTE_SIZE, BLOCK_SIZE, homePageFd))
            > 0) {
        fileSize += bytesRead;
        if (bytesRead == BLOCK_SIZE) {
            // If 100 bytes have been read, there should be more to come,
            // Allocate some more space for it
            bufferSize += BLOCK_SIZE;
            buffer = realloc(buffer, BLOCK_SIZE * bufferSize);
        }
        // If bytesRead is less than 100, most likely is the end of the file
        // and the next read should result in 0 bytes read, hence no extra
        // space is needed
    }

    fclose(homePageFd);
    // Save save data into request
    //
    response->data = construct_HTTP_response(
            RES_200, RES_200_MSG, headers, buffer, fileSize, &response->size);
    response->status = RES_200;
}

// send_response()
//
// Sends the http response
void send_response(FILE* writeEnd, HttpResponse* response)
{
    fwrite(response->data, sizeof(unsigned char), response->size, writeEnd);
    free(response->data);
    free(response);
    fflush(writeEnd);
}

// isdigit_str()
//
// exactly the same as isdigit() except only returns true if the whole string
// is only numbers 0-9
//
bool isdigit_str(char* input)
{
    bool isNumber = true;

    for (int i = 0; i < (int)strlen(input); i++) {
        if (!isdigit(input[i]) && (input[i] != '+' && input[i] != '-')) {
            isNumber = false;
            break;
        }
    }

    return isNumber;
}

// construct_invalid_response()
//
// If the http request was invalid, send the necessary response
void construct_invalid_response(int status, char* data, HttpResponse* response)
{
    // Create headers, Content-Length is autmoatically created
    HttpHeader** headers = malloc(sizeof(HttpHeader*) * 2);
    HttpHeader contentType = {"Content-Type", "text/plain"};
    headers[0] = &contentType;
    headers[1] = NULL;

    if (status == RES_404) {
        // Construct a 404 response
        response->data = construct_HTTP_response(RES_404, RES_404_MSG, headers,
                RES_404_BODY, RES_404_SIZE, &response->size);
    } else if (status == RES_405) {
        // Construct a 405 response
        response->data = construct_HTTP_response(RES_405, RES_405_MSG, headers,
                RES_405_BODY, RES_405_SIZE, &response->size);
    } else if (status == RES_400) {
        // Construct a 400 response
        response->data = construct_HTTP_response(RES_400, RES_400_MSG, headers,
                RES_400_BODY, RES_400_SIZE, &response->size);
    } else if (status == RES_422) {
        // Construct a 422 response
        response->data = construct_HTTP_response(RES_422, RES_422_MSG, headers,
                RES_422_BODY, RES_422_SIZE, &response->size);
    } else if (status == RES_413) {
        // Construct a 413 response
        char body[TEMP_BUFFER_SIZE];
        sprintf(body, (char*)RES_413_BODY, (unsigned long)data);
        response->data = construct_HTTP_response(RES_413, RES_413_MSG, headers,
                (unsigned char*)body, strlen(body), &response->size);
    } else if (status == RES_501) {
        // Construct a 501 response
        char body[TEMP_BUFFER_SIZE];
        sprintf(body, (char*)RES_501_BODY, (char*)data);
        response->data = construct_HTTP_response(RES_501, RES_501_MSG, headers,
                (unsigned char*)body, strlen(body), &response->size);
    }
    response->status = status;
    free(headers);
}

// construct_valid_response()
//
// if the http request was valid, construct a 200 OK response with the given
// buffered data
void construct_valid_response(
        unsigned char* body, unsigned long size, HttpResponse* response)
{
    HttpHeader** headers = malloc(sizeof(HttpHeader*) * 2);
    HttpHeader contentType = {"Content-Type", "image/png"};
    headers[0] = &contentType;
    headers[1] = NULL;

    response->data = construct_HTTP_response(
            RES_200, RES_200_MSG, headers, body, size, &response->size);

    free(headers);
    free(body);
}

// init_stats()
//
// initialises the stats array
// 0th index - Clients connected
// 1st index - Clients completed
// 2nd index - Successful HTTP requests
// 3rd index - Failed HTTP requests
// 4th index - Successful image operations
long* init_stats(void)
{
    long* stats = malloc(sizeof(long) * NUM_STATS);
    stats[STATS_CLIENTS_CONNECTED] = 0;
    stats[STATS_CLIENTS_COMPLETED] = 0;
    stats[STATS_HTTP_SUCCESSFUL] = 0;
    stats[STATS_HTTP_FAILED] = 0;
    stats[STATS_IMAGE_SUCCESSFUL] = 0;

    return stats;
}
