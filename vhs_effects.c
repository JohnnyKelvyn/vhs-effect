#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <stdlib.h>
#include <string.h>


#include "vhs_effects.h"
#include "utils.h"


// função auxiliar para dithering
static uint8_t quantize(uint8_t value, int levels) {
    // Normaliza valor pra 0.0-1.0
    float normalized = value / 255.0f;  
    
    // Multiplica por (levels - 1) e arredonda
    int index = (int)roundf(normalized * (levels - 1));  
    
    // Calcula step entre níveis
    int step = 255 / (levels - 1);  
    
    // Retorna valor quantizado
    return index * step;
}


// Função auxiliar para ruído gaussiano aproximado (média 0, desvio padrão 1)
float gaussian_noise() {
    float u1 = ((float)rand() / (float)RAND_MAX) + 1e-7f; // Pequeno offset para nunca ser zero
    float u2 = (float)rand() / (float)RAND_MAX;
    // Se u1 for 0, o log(u1) quebra o programa. O 1e-7f impede isso. IA GEROU ISSO
    return (float)(sqrt(-2.0f * log(u1)) * cos(2.0f * M_PI * u2));
}


void apply_scanlines(Image *img, int intensity, float variation, float head_switch_prob) {
    if (!img || !img->pixels) return;
    if (intensity <= 0) return;

    int height = img->height;
    int width = img->width;
    uint8_t *pixels = img->pixels;

    for (int i = 0; i < height; i++) {
        float noise = (float)(rand() % 100) / 100.0f; // 0.0 até 1.0

        // Variação de espessura sutil: 1 ou 2 linhas
        int thickness = 1 + (rand() % 2); // 1 ou 2 para menos poluição

        if (noise < variation) {  // Aplicar apenas com baixa probabilidade para sutileza
            // Darkness base sutil + variação gaussiana para organicidade
            float base_darkness = 0.85f + gaussian_noise() * 0.05f; // Varia levemente em torno de 0.85-0.95
            base_darkness = clampf(base_darkness, 0.8f, 0.95f); // clamp float

            for (int t = 0; t < thickness && (i + t) < height; t++) {
                for (int j = 0; j < width; j++) {
                    int index = ((i + t) * width + j) * 3;

                    // Variação sinusoidal ao longo da linha para organicidade
                    float sin_variation = 0.05f * sinf(2 * M_PI * j / (float)width + (rand() % 10)); // Onda suave + offset rand
                    float line_darkness = base_darkness + sin_variation;
                    line_darkness = clampf(line_darkness, 0.8f, 1.0f);

                    // Aplicar sempre, mas sutil (sem rand() < 95)
                    pixels[index + 0] = (uint8_t)(pixels[index + 0] * line_darkness);
                    pixels[index + 1] = (uint8_t)(pixels[index + 1] * line_darkness);
                    pixels[index + 2] = (uint8_t)(pixels[index + 2] * line_darkness);
                }
            }
            i += thickness - 1; // Pular linhas processadas
        }

        // Head-switching noise: manter sutil
        if (i > height * 0.9 && (float)(rand() % 100) / 100.0f < head_switch_prob) {
            int is_white = rand() % 2;
            float factor = is_white ? 1.2f : 0.8f; // Ajustado para menos contraste
            for (int j = 0; j < width; j++) {
                int index = (i * width + j) * 3;
                pixels[index + 0] = clamp((int)(pixels[index + 0] * factor), 0, 255);
                pixels[index + 1] = clamp((int)(pixels[index + 1] * factor), 0, 255);
                pixels[index + 2] = clamp((int)(pixels[index + 2] * factor), 0, 255);
            }
        }
    }
}

void apply_chromatic_aberration(Image *img, int offset, float jitter) {
    if (!img || !img->pixels) return;
    if (offset <= 0) return;


    int height = img->height;
    int width = img->width;
    size_t totalsize = width * height * 3;
    uint8_t *temp = malloc(totalsize);
    if (!temp) {
        printf("\nFALHA: Erro ao aplicar efeito de CHROMATIC ABERRATION.\n");
        return;
    }
    memcpy(temp, img->pixels, totalsize);

    for (int y = 0; y < height; y++) {
        int line_jitter = (int)((rand() % 100 - 50) / 100.0f * offset * jitter);
        int offset_red = offset + line_jitter;
        int offset_blue = -offset + line_jitter;

        for (int x = 0; x < width; x++) {
            // Detecção simples de borda: diferença de brilho com vizinho esquerdo
            int idx_current = (y * width + x) * 3;
            int brightness_current = (temp[idx_current + 0] + temp[idx_current + 1] + temp[idx_current + 2]) / 3;
            int brightness_left = (x > 0) ? (temp[idx_current - 3] + temp[idx_current - 2] + temp[idx_current - 1]) / 3 : brightness_current;
            int edge_strength = abs(brightness_current - brightness_left);

            // Amplifica offset em bordas (fringing)
            int extra_offset = (edge_strength > 50) ? (rand() % 3) : 0; // Mais forte se diferença > 50
            int xr = clamp(x + offset_red + extra_offset, 0, width - 1);
            int xb = clamp(x + offset_blue - extra_offset, 0, width - 1);

            img->pixels[idx_current + 0] = temp[(y * width + xr) * 3 + 0];
            img->pixels[idx_current + 1] = temp[idx_current + 1]; // Verde fixo
            img->pixels[idx_current + 2] = temp[(y * width + xb) * 3 + 2];
        }
    }
    free(temp);
}




void apply_noise(Image *img, int intensity, float coherence) {
    if (!img || !img->pixels) return;
    if (intensity <= 0) return;

    uint8_t *pixels = img->pixels;



    printf("DEBUG apply_noise: width=%d, height=%d\n", img->width, img->height);
    printf("DEBUG: Primeiro pixel index = (0*%d+0)*3 = %zu\n", img->width, (size_t)(0 * img->width + 0) * 3);
    printf("DEBUG: Último pixel index = (%d*%d+%d)*3 = %zu\n", 
           img->height-1, img->width, img->width-1, 
           (size_t)((img->height-1) * img->width + (img->width-1)) * 3);


    // Parâmetros de escala ajustados para não saturar rapidamente
    float noise_scale = intensity * 1.5f; 
    float chroma_boost = 1.4f;
    float luma_reduction = 0.6f;

    for (int y = 0; y < img->height; y++) {
        // Reiniciamos o ruído anterior a cada linha para evitar que erros 
        // se propaguem pela imagem inteira e causem a tela preta
        float prev_r = 0, prev_g = 0, prev_b = 0;

        for (int x = 0; x < img->width; x++) {
            int index = (y * img->width + x) * 3;

            // 1. Gerar ruído base (Proteção: gaussian_noise deve ser estável)
            float nr = gaussian_noise() * noise_scale * chroma_boost;
            float ng = gaussian_noise() * noise_scale * luma_reduction;
            float nb = gaussian_noise() * noise_scale * chroma_boost;

            // 2. Aplicar Coerência (Filtro passa-baixa horizontal)
            // Isso cria o aspecto de "arrasto" ou "nuvem" de ruído típica do VHS
            nr = (coherence * prev_r) + ((1.0f - coherence) * nr);
            ng = (coherence * prev_g) + ((1.0f - coherence) * ng);
            nb = (coherence * prev_b) + ((1.0f - coherence) * nb);

            // Guardar para o próximo pixel da mesma linha
            prev_r = nr; prev_g = ng; prev_b = nb;

            // 3. Spikes de ruído aleatórios (Interferência de rádio)
            if (rand() % 1000 < 5) {
                nr *= 3.0f;
                nb *= 3.0f;
            }

            // 4. Aplicação final com Clamp rigoroso
            // Convertemos para int ANTES de somar para evitar overflow de uint8_t
            int r = (int)pixels[index + 0] + (int)nr;
            int g = (int)pixels[index + 1] + (int)ng;
            int b = (int)pixels[index + 2] + (int)nb;

            pixels[index + 0] = (uint8_t)clamp(r, 0, 255);
            pixels[index + 1] = (uint8_t)clamp(g, 0, 255);
            pixels[index + 2] = (uint8_t)clamp(b, 0, 255);

            // 5. Dropout (Perda de sincronia de cor)
            if (rand() % 5000 < 1) { 
                pixels[index + 0] = 160; // Cinza levemente azulado
                pixels[index + 1] = 160;
                pixels[index + 2] = 180;
            }
        }
    }
}


void apply_vignette(Image *img, float strength) {
    if (!img || !img->pixels) return;

    int height = img->height;
    int width = img->width;
    
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;
    // Usamos o quadrado da distância máxima para evitar sqrt() no loop
    float maxDistSq = centerX * centerX + centerY * centerY;

    for (int y = 0; y < height; y++) {
        int row_offset = y * width;
        float dy = y - centerY;
        float dySq = dy * dy;

        for (int x = 0; x < width; x++) {
            float dx = x - centerX;
            // Distância ao quadrado
            float distSq = dx * dx + dySq;

            // Fator de escurecimento (1.0 no centro, vai diminuindo para as bordas)
            // Quanto maior a força, mais rápido escurece
            float fator = 1.0f - (distSq / maxDistSq) * strength;
            
            if(fator < 0.0f) fator = 0.0f;
            if(fator > 1.0f) fator = 1.0f;

            size_t pixel_index = (row_offset + x) * 3;

            img->pixels[pixel_index + 0] *= fator;
            img->pixels[pixel_index + 1] *= fator;
            img->pixels[pixel_index + 2] *= fator;
        }
    }
}

void apply_color_bleed(Image *img, float strength) {
    // strength = 0.0 (sem bleed) a 0.3 (muito bleed)
    
    if (!img || !img->pixels) return;

    int width = img->width;
    int height = img->height;
    uint8_t *pixels = img->pixels;
    
    // Processa de baixo pra cima (linha Y usa linha Y-1)
    for (int y = 1; y < height; y++) { // começa em 1!
        for (int x = 0; x < width; x++) {
            
            int idx_current = (y * width + x) * 3;
            int idx_above = ((y - 1) * width + x) * 3;
            
            // Mistura cor da linha de cima
            for (int c = 0; c < 3; c++) {
                int current = pixels[idx_current + c];
                int above = pixels[idx_above + c];
                
                pixels[idx_current + c] = (uint8_t)(
                    (1.0f - strength) * current + strength * above
                );
            }
        }
    }
}

void apply_tracking_glitch(Image *img, int intensity, float probability, float dropout_prob) {
    for (int y = 0; y < img->height; y++) {
        if ((rand() % 100) < (probability * 100)) {
            int shift = (rand() % (intensity * 2 + 1)) - intensity;
            rotate_row_rgb(img->pixels, img->width, y, shift);

            // Adiciona dropouts: pixels pretos randômicos na linha
            for (int x = 0; x < img->width; x++) {
                if ((float)(rand() % 100) / 100.0f < dropout_prob) {
                    int index = (y * img->width + x) * 3;
                    img->pixels[index + 0] = 0;
                    img->pixels[index + 1] = 0;
                    img->pixels[index + 2] = 0;
                }
            }
        }
    }
}

void apply_tape_stretching(Image *img, int num_clusters, int max_shift_range) {
    
    for (int i = 0; i < num_clusters; i++) {
        
        // ============ PARÂMETROS DO CLUSTER ============
        
        // Onde começa? (linha aleatória)
        int start = rand_range(0, img->height);
        
        // Quantas linhas? (altura aleatória)
        int height = rand_range(10, 50);
        
        // Quanto desloca? (shift máximo aleatório)
        int max_shift = rand_range(5, max_shift_range);  
        
        // Direção? (esquerda ou direita)
        int direction = (rand() % 2 == 0) ? 1 : -1 ; 
        
        // ============ VALIDAÇÕES ============
        
        // Cluster sai da imagem?
        if (start + height > img->height) {
            height = img->height - start;
        }
        
        // ============ APLICAR STRETCH ============
        
        for (int y = start; y < start + height; y++) {
            // Calcula shift gradual para esta linha
            int shift = (y - start) * ((float)max_shift / height);  // Você já sabe essa fórmula!
            
            // Aplica direção
            shift = shift * direction;
            
            // Rotaciona linha
            rotate_row_rgb(img->pixels, img->width, y, shift);
        }
    }
}

void apply_ghosting(Image *img, float strength) {

    if (!img || !img->pixels) return;

    int width = img->width;
    int height = img->height;
    size_t totalsize = width * height * 3;
    uint8_t *temp = malloc(totalsize);
    if (!temp) return;
    memcpy(temp, img->pixels, totalsize);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;
            int idx_left = (x > 0) ? idx - 3 : idx;
            int idx_right = (x < width - 1) ? idx + 3 : idx;

            for (int c = 0; c < 3; c++) {
                int smeared = (int)((1.0f - strength) * temp[idx + c] +
                                    (strength / 2.0f) * temp[idx_left + c] +
                                    (strength / 2.0f) * temp[idx_right + c]);
                img->pixels[idx + c] = clamp(smeared, 0, 255);
            }
        }
    }
    free(temp);
}

void apply_color_shift(Image *img, float fade_strength, float yellow_tint) {
    if (!img || !img->pixels) return;

    int total_pixels = img->width * img->height;
    uint8_t *pixels = img->pixels;

    for (int i = 0; i < total_pixels; i++) {
        int idx = i * 3;
        int r = pixels[idx + 0];
        int g = pixels[idx + 1];
        int b = pixels[idx + 2];

        // Reduz saturação (média para cinza, misturada)
        int gray = (r + g + b) / 3;
        r = (int)((1.0f - fade_strength) * r + fade_strength * gray);
        g = (int)((1.0f - fade_strength) * g + fade_strength * gray);
        b = (int)((1.0f - fade_strength) * b + fade_strength * gray);

        // Adiciona tint amarelo (aumenta R e G levemente)
        r = clamp(r + (int)(yellow_tint * 30), 0, 255);
        g = clamp(g + (int)(yellow_tint * 20), 0, 255);

        pixels[idx + 0] = r;
        pixels[idx + 1] = g;
        pixels[idx + 2] = b;
    }
}


void apply_dot_crawl(Image *img, float strength) {
    if (!img || !img->pixels) return;

    int width = img->width;
    int height = img->height;
    uint8_t *pixels = img->pixels;

    for (int y = 0; y < height; y++) {
        for (int x = 1; x < width; x++) { // Começa em 1 para comparar com esquerdo
            int idx = (y * width + x) * 3;
            int idx_left = idx - 3;

            // Detecção de borda vertical (diferença de brilho)
            int bright = (pixels[idx + 0] + pixels[idx + 1] + pixels[idx + 2]) / 3;
            int bright_left = (pixels[idx_left + 0] + pixels[idx_left + 1] + pixels[idx_left + 2]) / 3;
            if (abs(bright - bright_left) > 40) { // Limiar para borda
                // Adiciona ruído colorido alternado (+R -B em pares)
                int offset = (x % 2 == 0) ? 1 : -1;
                pixels[idx + 0] = clamp(pixels[idx + 0] + (int)(strength * 20 * offset), 0, 255);
                pixels[idx + 2] = clamp(pixels[idx + 2] - (int)(strength * 20 * offset), 0, 255);
            }
        }
    }
}

void apply_ordered_dithering(Image *img, int levels) {
    if (!img || !img->pixels) return;
    if (levels < 2) return;

    const int BAYER_SIZE = 4;
    const float BAYER_MAX = 15.0f;
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            
            // ============ PARTE A: POSIÇÃO NA MATRIZ ============
            int bx = x % BAYER_SIZE;  // x módulo quanto?
            int by = y % BAYER_SIZE;  // y módulo quanto?
            
            // ============ PARTE B: THRESHOLD ============
            float threshold = bayer_4x4[by][bx] / BAYER_MAX;  // Divide por BAYER_MAX
            
            // ============ PARTE C: CALCULAR ERRO ============
            float step = 255.0f / (levels - 1);  // Divide por (levels - 1)
            float erro = (threshold - 0.5) * step;  // threshold - 0.5
            
            // ============ PARTE D: PROCESSAR PIXEL ============
            int idx = (y * img->width + x) * 3;
            
            // Pegar valores originais
            int r = img->pixels[idx + 0];
            int g = img->pixels[idx + 1];
            int b = img->pixels[idx + 2];
            
            // Adicionar erro (clampar!)
            r = clamp((int)(r + erro), 0, 255);
            g = clamp((int)(g + erro), 0, 255);
            b = clamp((int)(b + erro), 0, 255);
            
            // Quantizar e salvar
            img->pixels[idx + 0] = quantize(r, levels);
            img->pixels[idx + 1] = quantize(g, levels);
            img->pixels[idx + 2] = quantize(b, levels);
        }
    }
}