#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION


#include "image.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




Image load_image(const char *filename){
    Image img;
    
    int channels;

    img.pixels = stbi_load(filename, &img.width, &img.height,  &channels, 3);
    if(!img.pixels){
        printf("Error: %s\n", stbi_failure_reason());
        exit(1);
    }

    printf("Imagem carregada: %dx%d (%d canais originais)\n", img.width, img.height, channels);
    
    return img;
}


void save_image(const char *out_filename, Image* img){

    const char *ext = strrchr(out_filename, '.');
    int result = 0;

    if(ext && strcmp(ext, ".png") == 0){
        result = stbi_write_png(out_filename, img->width, img->height, 3, img->pixels, img->width * 3);
    } else if(ext && strcmp(ext, ".jpg") == 0){
        result = stbi_write_jpg(out_filename, img->width, img->height, 3, img->pixels, 90);
    } else if(ext && strcmp(ext, ".bmp") == 0){
        result = stbi_write_bmp(out_filename, img->width, img->height, 3, img->pixels);
    } else {
        printf("ERRO: Formato não suportado. Use .png, .jpg ou .bmp\n");
        return;
    }

    if(result){
        printf("Imagem salva: %s\n", out_filename);
    } else{
        printf("Falha ao salvar '%s'\n", out_filename);
    }
}


void free_image(Image *ptr){
    if(ptr->pixels) stbi_image_free(ptr->pixels);
    ptr->pixels = NULL;
}

