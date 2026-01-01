# VHS Effect Processor 📼

Aplicador de efeitos VHS em imagens estáticas.

## 🎨 Efeitos Implementados

11 efeitos totais:
- Color shift/bleed/ghosting
- Chromatic aberration  
- Tracking glitch & tape stretching
- Scanlines & noise
- Vignette & dithering

## 🚀 Uso
```bash
# Compilar
gcc -o vhs main.c image.c vhs_effects.c utils.c -lm

# Executar
./vhs input.jpg output.png --preset-90s
```

## 📋 Presets

- `--preset-80s`: VHS degradado (fita antiga)
- `--preset-90s`: VHS qualidade média (padrão)
- `--preset-clean`: VHS artístico limpo

## 🛠️ Tecnologias

- C (gcc)
- stb_image/stb_image_write
- getopt para CLI

## 📚 Aprendizados

Projeto desenvolvido para aprender:
- Manipulação de arrays lineares
- Processamento de imagem em C
- Modularização de código
- Ordered dithering (Bayer matrix)

## 📝 Notas e Observações

Este projeto foi desenvolvido com foco educacional e experimental. É importante notar que:

* **Foco na Estética, não no Realismo:** O objetivo principal não é uma simulação física 100% fiel ao sinal analógico original, mas sim capturar e aplicar a "vibe" e os efeitos visuais característicos de uma filmagem VHS.
* **Origem das Fórmulas:** Algumas fórmulas matemáticas de processamento de cor e distorção foram adaptadas de discussões em **fóruns de matemática e processamento de sinais**.
* **Uso de IA:** Partes da implementação lógica e o refinamento de fórmulas complexas contaram com o auxílio de **Inteligência Artificial**.
* **Trabalho em Progresso:** Por ser um projeto de aprendizado, ainda há muito o que melhorar em termos de performance, precisão dos algoritmos e refatoração de código.

---

Desenvolvido como parte de aprendizado em C.