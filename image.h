#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <stdio.h>

#include "./includes/stb/stb_image.h"
#include "./includes/stb/stb_image_write.h"


typedef struct {
    int width;
    int height;
    uint8_t *pixels;
} Image;


Image load_image(const char *filename);

void save_image(const char *out_filename, Image* img);

void free_image(Image *img);

#endif