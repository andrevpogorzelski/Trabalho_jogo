#ifndef FUNDO_H
#define FUNDO_H

#include "raylib.h"

// Fundo animado (GIF): carrega todos os quadros e troca de quadro com o tempo
typedef struct {
    Image animacao;        // todos os quadros do GIF, um depois do outro na memória
    Texture2D textura;     // quadro que está na tela
    int quadros;
    int quadro_atual;
    float tempo;
    float duracao_quadro;  // segundos por quadro
} Fundo;

Fundo criar_fundo(const char *arquivo, float duracao_quadro);
void atualizar_fundo(Fundo *fundo);
void desenhar_fundo(Fundo fundo, int largura, int altura);
void descarregar_fundo(Fundo fundo);

#endif
