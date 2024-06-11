#ifndef IMAGE_H
#define IMAGE_H

#include <FreeImage.h>
#include <csse2310_freeimage.h>
#include <stdbool.h>
#include <string.h>

#include "server.h"

int perform_image_operation(FIBITMAP** image, char** subOps);
void proccess_image_file(
        char* method, FIBITMAP** image, HttpResponse* response);

#endif
