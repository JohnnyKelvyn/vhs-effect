// vhs_effects.h
#ifndef VHS_EFFECTS_H
#define VHS_EFFECTS_H

#include <stdlib.h>
#include <math.h>

#include "image.h"

// matrix para dithering
static const int bayer_4x4[4][4] = {
    { 0,  8,  2, 10},
    {12,  4, 14,  6},
    { 3, 11,  1,  9},
    {15,  7, 13,  5}
};

// Efeito 1: Scanlines
void apply_scanlines(Image *img, int intensity, float variation, float head_switch_prob);

// Efeito 2: Chromatic Aberration
void apply_chromatic_aberration(Image *img, int offset, float jitter);

// Efeito 3: Noise/Grain
void apply_noise(Image *img, int intensity, float coherence);

// Efeito 4: Vignette
void apply_vignette(Image *img, float strength);

// Efeito 5: Color Bleed
void apply_color_bleed(Image *img, float strength);

// Efeito 6: tracking glitch
void apply_tracking_glitch(Image *img, int intensity, float probability, float dropout_prob);

// Efeito 7: tape stretching
void apply_tape_stretching(Image *img, int num_clusters, int max_shift_range);

// Efeito 8: ghosting 
void apply_ghosting(Image *img, float strength);

// Efeito 9: color shift
void apply_color_shift(Image *img, float fade_strength, float yellow_tint);

// Efeito 10: dot craw
void apply_dot_crawl(Image *img, float strength);

// Efeito 11: ordered dithering
void apply_ordered_dithering(Image *img, int levels);

#endif