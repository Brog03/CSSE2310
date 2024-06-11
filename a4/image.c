#include "image.h"
#include "server.h"

// perform_image_operation()
//
// tries to perform an image operation, image data is NULL if failed
int perform_image_operation(FIBITMAP** image, char** subOps)
{
    FIBITMAP* newBitMap = NULL;

    if (strcmp(subOps[0], "rotate") == 0) {
        // Operation is a rotate
        newBitMap = FreeImage_Rotate(*image, atoi(subOps[1]), NULL);

    } else if (strcmp(subOps[0], "scale") == 0) {
        // Operation is a scale
        newBitMap = FreeImage_Rescale(
                *image, atoi(subOps[1]), atoi(subOps[2]), FILTER_BILINEAR);

    } else {
        // Operation is a flip
        if (subOps[1][0] == 'h') {
            if (FreeImage_FlipHorizontal(*image) != 0) {
                // If the flip was a success, assign the data to newBitMap
                newBitMap = *image;
            }
        } else {
            if (FreeImage_FlipVertical(*image) != 0) {
                // If the flip was a success, assign the data to newBitMap
                newBitMap = *image;
            }
        }
    }
    *image = newBitMap;
    return (*image == NULL) ? RES_501 : RES_200;
}

// proccess_image_file()
//
// goes through address and checks it is formatted correctly, and tries
// to complete each image operation if so
void proccess_image_file(char* method, FIBITMAP** image, HttpResponse* response)
{
    int status = (*image == NULL) ? RES_422 : RES_200;
    // Split operations into an array delimted by '/'
    char** operationsArray = split_by_char(method, '/', 0);
    char** operation;
    char failedOperation[FAILED_OPERATION_LENGTH];

    int i = 1;
    while (operationsArray[i] && *image != NULL) {
        // There is text followed by the previous '/', copy it into temp
        char temp[strlen(operationsArray[i]) + 1];
        strcpy(temp, operationsArray[i]);
        // Spilt this into subOperations
        operation = split_by_char(temp, ',', 0);

        if (strcmp(operation[0], "rotate") == 0 && operation[1]
                && isdigit_str(operation[1])
                && (atoi(operation[1]) <= MAX_ANGLE
                        && atoi(operation[1]) >= MIN_ANGLE)) {
            // A legal rotate operation has been provided
            status = perform_image_operation(image, operation);
        } else if (strcmp(operation[0], "flip") == 0
                && strlen(operation[1]) == 1
                && (operation[1][0] == 'h' || operation[1][0] == 'v')) {
            // A legal scale operation has been provided
            status = perform_image_operation(image, operation);
        } else if (strcmp(operation[0], "scale") == 0 && operation[1]
                && operation[2] && isdigit_str(operation[1])
                && isdigit_str(operation[2]) && atoi(operation[1]) > 0
                && atoi(operation[2]) > 0 && atoi(operation[1]) <= MAX_SCALE
                && atoi(operation[2]) <= MAX_SCALE) {
            // A legal flip operation has been provided
            status = perform_image_operation(image, operation);
        } else {
            // The operation is not legal, send a 400
            status = RES_400;
            *image = NULL;
        }
        if (status == RES_200) {
            i++;
        } else if (status == RES_501) {
            strcpy(failedOperation, operation[0]);
        }
    }
    if (status != 0) {
        construct_invalid_response(status, failedOperation, response);
    }
    free(method);
    response->numOps = i - 1;
    response->status = status;
}
