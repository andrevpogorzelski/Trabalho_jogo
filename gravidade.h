#ifndef GRAVIDADE_H
#define GRAVIDADE_H

#include "raylib.h"
#include <stdbool.h>

// Mesma física do gravidade.c: velocidade += gravidade; y += velocidade;
// no chão ela volta com "resistencia" (perde um pouco de força a cada quique)
typedef struct {
    float x, y;           // centro da bola
    float velocidade_x;   // anda pros lados
    float velocidade_y;   // a "velocidade" do gravidade.c
    float raio;
    float resistencia;    // 0.9 = volta com 90% da força
    float fase;           // NOVO: posição na animação de rolar (0 a FRAMES_BOLA)
    bool olhando_esquerda;// NOVO: true = última vez que andou foi para a esquerda
} Bola;

#define FRAMES_BOLA 8     // NOVO: quadros de cada linha da folha bola/bola.png

Bola criar_bola(float x, float y, float raio);
void atualizar_bola(Bola *bola, float gravidade, int largura, int altura);
void chutar_bola(Bola *bola, Rectangle jogador, float forca);
void desenhar_bola(Bola bola, Texture2D sprite);   // NOVO: recebe a folha de sprites
Texture2D carregar_sprite_bola(const char *arquivo);  // NOVO
void descarregar_sprite_bola(Texture2D sprite);       // NOVO

// ===================== GOL =====================
typedef struct {
    Texture2D sprite;        // imagem do gol (já sem as bordas transparentes)
    Rectangle area;          // onde o gol fica na tela
    float altura_travessao;  // espessura do travessão (parte de cima, sólida)
    bool lado_esquerdo;      // true = gol da esquerda
} Gol;

Gol criar_gol(const char *arquivo, bool lado_esquerdo, int largura_tela, int altura_tela, float altura_gol);
void colidir_travessao(Bola *bola, Gol gol);
bool bola_entrou(Bola bola, Gol gol);
void desenhar_gol(Gol gol);
void descarregar_gol(Gol gol);

// ===================== CHUTE =====================
// Segure a tecla de chute para carregar a força. Enquanto segura, as teclas de cima/baixo
// mudam o ângulo. Ao soltar, se a bola estiver ao alcance (na frente do jogador), ela é chutada.
// Soltar a tecla (acertando ou não) inicia a recarga: só dá para chutar de novo depois dela.
// Ajuste o chute mexendo só nestes números:
#define TEMPO_CARGA_MAX   1.0f   // segundos segurando até a força máxima
#define RECARGA_CHUTE     1.5f   // segundos até poder chutar de novo (2.0f = 2 segundos)
#define FORCA_CHUTE_MIN  12.0f   // força com um toque rápido na tecla
#define FORCA_CHUTE_MAX  30.0f   // força com a carga cheia
#define ALCANCE_CHUTE    28.0f   // quanto o pé alcança na frente do jogador (pixels)
#define ANGULO_INICIAL   30.0f   // graus: 0 = reto, 90 = pra cima
#define ANGULO_MIN        5.0f
#define ANGULO_MAX       75.0f
#define VELOCIDADE_MIRA  80.0f   // graus por segundo ao mexer no ângulo

typedef struct {
    int tecla_chute, tecla_chute_alt;   // duas teclas possíveis para chutar
    int tecla_cima, tecla_baixo;        // mudam o ângulo enquanto carrega
    bool carregando;                    // true = segurando a tecla
    float carga;                        // segundos segurando (até TEMPO_CARGA_MAX)
    float angulo;                       // graus
    float recarga;                      // segundos que faltam para poder chutar de novo
} Chute;

Chute criar_chute(int tecla_chute, int tecla_chute_alt, int tecla_cima, int tecla_baixo);
void atualizar_chute(Chute *chute, Bola *bola, Rectangle corpo, bool olhando_direita, float escala);
void desenhar_chute(Chute chute, Rectangle corpo, bool olhando_direita, float escala);

#endif