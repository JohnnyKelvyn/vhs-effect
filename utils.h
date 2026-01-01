#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline int clamp(int v, int min, int max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

static inline float clampf(float v, float min, float max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

int rand_range(int min, int max);

/*--------------------------------------------------------------
 *  rotate_row – rotaciona a linha i k posições à direita
 *
 *  a      : vetor contendo a matriz (row‑major)
 *  width  : número de colunas da matriz
 *  i      : índice da linha a ser rotacionada (0‑based)
 *  k      : deslocamento à direita (k pode ser > width)
 *--------------------------------------------------------------*/
void rotate_row_rgb(uint8_t* pixels, int width, int row, int shift);

#endif