#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include "image.h"
#include "vhs_effects.h"

typedef struct {
    char *input_file;
    char *output_file;
    
    // Parâmetros de imagem e cor
    float color_fade;
    float color_tint;
    float bleed_strength;
    float ghosting_strength;
    float crawl_strength;
    
    // Scanlines
    int scanlines_intensity;
    float scanlines_variation;
    float scanlines_head_prob;
    
    // Aberração e Ruído
    int chromatic_offset;
    float chromatic_jitter;
    int noise_intensity;
    float noise_coherence;
    
    // Deformação de fita
    int tracking_intensity;
    float tracking_probability;
    float tracking_dropout;
    int tape_clusters;
    int tape_shift;
    
    // Finalização
    float vignette_strength;
    int dither_levels;
} VHSParams;

// --- PRESETS (Carregadores de dados) ---

void load_preset_80s(VHSParams *p) {
    p->color_fade = 0.25f; p->color_tint = 0.15f;
    p->bleed_strength = 0.15f;
    p->ghosting_strength = 0.15f;
    p->chromatic_offset = 3; p->chromatic_jitter = 0.6f;
    p->tape_clusters = 5; p->tape_shift = 40;
    p->tracking_intensity = 20; p->tracking_probability = 0.15f; p->tracking_dropout = 0.01f;
    p->crawl_strength = 0.15f;
    p->scanlines_intensity = 3; p->scanlines_variation = 0.3f; p->scanlines_head_prob = 0.05f;
    p->noise_intensity = 15; p->noise_coherence = 0.7f;
    p->vignette_strength = 0.4f;
    p->dither_levels = 4;
}

void load_preset_90s(VHSParams *p) {
    p->color_fade = 0.08f; p->color_tint = 0.06f;
    p->bleed_strength = 0.08f;
    p->ghosting_strength = 0.05f;
    p->chromatic_offset = 2; p->chromatic_jitter = 0.3f;
    p->tape_clusters = 3; p->tape_shift = 25;
    p->tracking_intensity = 10; p->tracking_probability = 0.08f; p->tracking_dropout = 0.005f;
    p->crawl_strength = 0.08f;
    p->scanlines_intensity = 4; p->scanlines_variation = 0.15f; p->scanlines_head_prob = 0.02f;
    p->noise_intensity = 8; p->noise_coherence = 0.8f;
    p->vignette_strength = 0.2f;
    p->dither_levels = 6;
}

void load_preset_clean(VHSParams *p) {
    p->color_fade = 0.05f; p->color_tint = 0.08f;
    p->bleed_strength = 0.05f;
    p->ghosting_strength = 0.03f;
    p->chromatic_offset = 1; p->chromatic_jitter = 0.2f;
    p->tape_clusters = 2; p->tape_shift = 15;
    p->tracking_intensity = 8; p->tracking_probability = 0.05f; p->tracking_dropout = 0.002f;
    p->crawl_strength = 0.05f;
    p->scanlines_intensity = 5; p->scanlines_variation = 0.1f; p->scanlines_head_prob = 0.01f;
    p->noise_intensity = 5; p->noise_coherence = 0.85f;
    p->vignette_strength = 0.15f;
    p->dither_levels = 8;
}

// --- CORE ---

void print_usage(const char *prog) {
    printf("VHS Effect - Simulador de Fita VHS\n\n");
    printf("Uso:\n");
    printf("  %s input.png output.png [opções]\n\n", prog);
    printf("Modos (carregam valores base):\n");
    printf("  --preset-80s         VHS degradado\n");
    printf("  --preset-90s         Qualidade média (padrão)\n");
    printf("  --preset-clean       Artístico/Sutil\n\n");
    printf("Ajustes Manuais (sobrescrevem o preset):\n");
    printf("  --scanlines N        Intensidade (1-10)\n");
    printf("  --no-scanlines       Desabilita scanlines\n");
    printf("  --chromatic N        Aberração (1-5)\n");
    printf("  --noise N            Ruído (1-30)\n");
    printf("  --vignette F         Vignette (0.0-1.0)\n");
    printf("  --dither N           Níveis dithering (3-16)\n");
    printf("  --no-dither          Desabilita dithering\n\n");
}

void apply_all_effects(Image *img, VHSParams *p) {
    // Aplica efeitos na ordem correta de processamento de sinal
    apply_color_shift(img, p->color_fade, p->color_tint);
    apply_color_bleed(img, p->bleed_strength);
    apply_ghosting(img, p->ghosting_strength);
    
    apply_chromatic_aberration(img, p->chromatic_offset, p->chromatic_jitter);
    apply_tape_stretching(img, p->tape_clusters, p->tape_shift);
    apply_tracking_glitch(img, p->tracking_intensity, p->tracking_probability, p->tracking_dropout);
    
    apply_dot_crawl(img, p->crawl_strength);
    
    // Verificação de segurança para as flags de desativação
    if (p->scanlines_intensity > 0) {
        apply_scanlines(img, p->scanlines_intensity, p->scanlines_variation, p->scanlines_head_prob);
    }
    
    apply_noise(img, p->noise_intensity, p->noise_coherence);
    apply_vignette(img, p->vignette_strength);
    
    if (p->dither_levels > 0) {
        apply_ordered_dithering(img, p->dither_levels);
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    // Inicializa com o padrão 90s
    VHSParams params;
    load_preset_90s(&params);
    
    static struct option long_options[] = {
        {"preset-80s", no_argument, 0, '8'},
        {"preset-90s", no_argument, 0, '9'},
        {"preset-clean", no_argument, 0, 'c'},
        {"scanlines", required_argument, 0, 's'},
        {"chromatic", required_argument, 0, 'a'},
        {"noise", required_argument, 0, 'n'},
        {"vignette", required_argument, 0, 'v'},
        {"dither", required_argument, 0, 'd'},
        {"no-dither", no_argument, 0, 'D'},
        {"no-scanlines", no_argument, 0, 'S'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int opt;
    // Note que removi o "D:S" do optstring curto para usar apenas flags sem argumento
    while ((opt = getopt_long(argc, argv, "h89cs:a:n:v:d:DS", long_options, NULL)) != -1) {
        switch (opt) {
            case '8': load_preset_80s(&params); break;
            case '9': load_preset_90s(&params); break;
            case 'c': load_preset_clean(&params); break;
            
            case 's':
                params.scanlines_intensity = atoi(optarg);
                if (params.scanlines_intensity < 0) params.scanlines_intensity = 0;
                break;
            case 'S':
                params.scanlines_intensity = 0;
                break;
                
            case 'a':
                params.chromatic_offset = atoi(optarg);
                break;
            case 'n':
                params.noise_intensity = atoi(optarg);
                break;
            case 'v':
                params.vignette_strength = atof(optarg);
                break;
                
            case 'd':
                params.dither_levels = atoi(optarg);
                break;
            case 'D':
                params.dither_levels = 0;
                break;
                
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    // Validar argumentos posicionais (input e output)
    if (optind + 2 != argc) {
        fprintf(stderr, "Erro: Informe o arquivo de entrada e de saída.\n");
        return 1;
    }
    
    params.input_file = argv[optind];
    params.output_file = argv[optind + 1];
    
    printf("VHS Effect Processor\n");
    printf("Lendo: %s\n", params.input_file);
    
    Image img = load_image(params.input_file);
    if (img.pixels == NULL) {
        fprintf(stderr, "Erro ao carregar imagem.\n");
        return 1;
    }
    
    apply_all_effects(&img, &params);
    
    printf("Salvando: %s\n", params.output_file);
    save_image(params.output_file, &img);
    free_image(&img);
    
    printf("Concluído!\n");
    return 0;
}