#include "error_client.h"
#include "connect.h"

// connect_to_server()
//
// Establishes a connection to the server via TCP using an IPv4 Socket
// Returns an ImageHTTP which will cotain a read End FILE*, write end FILE*
// and an empty buffer pointer to store image data
ImageHTTP connect_to_server(const char* portNum)
{
    // Intiallise the image request to be empty
    ImageHTTP request = {NULL, 0, NULL, NULL, NULL};

    // Initialise TCP socket
    struct addrinfo* ai = 0;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    int err;

    if (!(err = getaddrinfo("localhost", portNum, &hints, &ai))) {
        // create socket. 0 == use default stream protocol (TCP)
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (!connect(fd, ai->ai_addr, sizeof(struct sockaddr))) {
            // copy fd, one for reading, one for writing
            FILE* readEnd = fdopen(fd, "r");
            FILE* writeEnd = fdopen(dup(fd), "w");

            request.readEnd = readEnd;
            request.writeEnd = writeEnd;
            // Used for free memory later on
            request.ai = ai;
        }
    }

    return request;
}

// send_image_to_server()
//
// Sends the necessary data to the serever via HTTP 1.1
void send_image_to_server(ImageHTTP request, int opData, int dataType)
{
    // Write the header to the port
    if (dataType == DATA_ROTATE) {
        // Operation is a rotate: data is one (int)
        fprintf(request.writeEnd, HTTP_REQUEST_ROTATE, opData, request.size);
    } else if (dataType == DATA_FLIP) {
        // Operation is a flip: data is one (char)
        fprintf(request.writeEnd, HTTP_REQUEST_FLIP, (char)opData,
                request.size);
    } else if (dataType == DATA_SCALE) {
        // Operation is a scale: data is two (int)s
        fprintf(request.writeEnd, HTTP_REQUEST_SCALE, RET_SCALE_X(opData),
                RET_SCALE_Y(opData), request.size);
    }

    // Flushes stdout
    fflush(stdout);
    // Write all data stored in the buffer to the port
    fwrite(request.data, BYTE_SIZE, request.size, request.writeEnd);
    // Close the writing end
    fclose(request.writeEnd);
}

// read_image_into_buffer()
//
// as STDIN has been redirected to the file specified by --in
// this reads data from STDIN and puts it into a buffer
int read_image_into_buffer(ImageHTTP* request)
{
    // Assumes file size is 10240 bytes
    size_t fileSize = 0;
    size_t bufferSize = BLOCK_SIZE; // 10Kb
    // Initialises the buffer
    unsigned char* buffer = malloc(BYTE_SIZE * bufferSize);

    int exitStatus = 0;

    size_t bytesRead = 0;
    while ((bytesRead = fread(buffer + fileSize, BYTE_SIZE, BLOCK_SIZE, stdin))
            > 0) {
        fileSize += bytesRead;
        if (bytesRead == BLOCK_SIZE) {
            // If 10240 bytes have been read, there should be more to come,
            // Allocate some more space for it
            bufferSize += BLOCK_SIZE;
            buffer = realloc(buffer, BYTE_SIZE * bufferSize);
        }
        // If bytesRead is less than 10240, most likely is the end of the file
        // and the next read should result in 0 bytes read, hence no extra
        // space is needed
    }

    if (fileSize == 0) {
        // Image file is empty
        exitStatus = ERR16_FILE_EMPTY;
    }

    // Save save data into request
    request->size = fileSize;
    request->data = buffer;

    return exitStatus;
}

// handle_http_response()
//
// Handles the response from the server
int handle_http_response(ImageHTTP request)
{
    // Initialise where the response data will be stored
    int status;
    char* statusExplain;
    HttpHeader** headers;
    unsigned char* body;
    unsigned long bodySize;

    int exitStatus = 0;

    // Wait for reponse
    if (get_HTTP_response(request.readEnd, &status, &statusExplain, &headers,
                &body, &bodySize)) {
        // Recieved a valid HTTP response
        if (status == RESPONSE_OK) {
            // Response is "200 OK"
            if (!fwrite(body, BYTE_SIZE, bodySize, stdout)) {
                // Could not write data to STDOUT
                exitStatus = ERR9_CANNOT_WRITE;
            }
        } else {
            // Recieved another reponse other than "200 OK"
            fprintf(stderr, ERR14_BAD_REQUEST_MSG, (int)bodySize, body);
            exitStatus = ERR14_BAD_REQUEST;
        }

        // Free necerssay memory used above
        free(body);
        free(statusExplain);
        free_array_of_headers(headers);

    } else {
        // get_HTTP_response read EOF (connection was closed)
        exitStatus = ERR20_CONNECTION_CLOSED;
    }

    return exitStatus;
}
