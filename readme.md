# VHS Effect Processor 📼

Aplicador de efeitos VHS em imagens estáticas.


## 🔧 Como Funciona

O processador aplica 11 efeitos em sequência otimizada:
1. **Color shift** → Desaturação e tint amarelado
2. **Chromatic aberration** → Separação RGB simulando falha de sincronismo
3. **Tracking glitch** → Linhas horizontais deslocadas aleatoriamente
4. **Scanlines** → Linhas de varredura CRT
5. **Ordered dithering** → Redução de paleta via matriz Bayer

> Usa `stb_image` para I/O e implementa manipulação direta de pixels em arrays lineares RGB.

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

## 🎯 Exemplos (Antes & Depois)

Abaixo, uma demonstração da comparação entre a imagem original e o resultado após a aplicação dos filtros com o preset padrão:

| Imagem Original | Processada (`--preset-80s`) |
| :---: | :---: |
| <img src="./imagens/original.jpg" width="300"> | <img src="./imagens/preset-80s.png" width="300"> |

**Créditos da imagem:** [Imagem por Freepik](https://www.freepik.com)

> **Dica:** Para um visual mais autêntico, utilize imagens com proporção **4:3** ou resoluções nativas de época (como **640x480**). O efeito de *scanlines* e ruído se torna muito mais visível em resoluções menores.


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


