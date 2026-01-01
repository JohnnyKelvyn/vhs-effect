#include "utils.h"


void rotate_row_rgb(uint8_t* pixels, int width, int row, int shift) {
    if (width <= 1) return;
    
    shift = shift % width;
    if (shift == 0) return;
    
    // Buffer temporário para UMA linha
    uint8_t *temp = malloc(width * 3);
    if (!temp) return;
    
    // Copia linha atual
    int row_start = row * width * 3;
    memcpy(temp, &pixels[row_start], width * 3);
    
    // Rotaciona (deslocamento pra DIREITA)
    for (int i = 0; i < width; i++) {
        int src = ((i - shift + width) % width) * 3;
        int dst = i * 3;
        
        pixels[row_start + dst + 0] = temp[src + 0];
        pixels[row_start + dst + 1] = temp[src + 1];
        pixels[row_start + dst + 2] = temp[src + 2];
    }
    
    free(temp);
}

int rand_range(int min, int max)
{
    return min + (int)((double)rand() / RAND_MAX * (max - min));
}