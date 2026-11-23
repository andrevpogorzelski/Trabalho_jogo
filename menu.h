#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include <stdbool.h>

// Telas do jogo. O jogo abre em TELA_MENU.
typedef enum { TELA_MENU, TELA_CONFIG, TELA_JOGANDO, TELA_FIM } Tela;

// O que o menu principal pede para o main fazer
typedef enum { ACAO_NENHUMA, ACAO_JOGAR, ACAO_CONFIG } AcaoMenu;

// Opções do menu principal. Para criar uma nova: coloque aqui (antes do TOTAL),
// escreva o texto em TEXTO_MENU no menu.c e trate a escolha em atualizar_menu.
typedef enum { ITEM_JOGAR, ITEM_CONFIG, TOTAL_ITENS_MENU } ItemMenu;

// Opções da tela de configurações (mesma ideia)
typedef enum { CONFIG_TEMPO, CONFIG_VOLTAR, TOTAL_ITENS_CONFIG } ItemConfig;

typedef struct {
    int sel_menu;        // opção marcada no menu principal
    int sel_config;      // opção marcada nas configurações
    int indice_tempo;    // 0 = 2 min, 1 = 3 min, 2 = 5 min
} Menu;

Menu criar_menu(void);
int minutos_da_partida(Menu menu);

AcaoMenu atualizar_menu(Menu *menu);
bool atualizar_config(Menu *menu);   // true = o jogador escolheu "Voltar"

void desenhar_menu(Menu menu, int largura, int altura);
void desenhar_config(Menu menu, int largura, int altura);
void desenhar_fim(int gols_jogador1, int gols_jogador2, int largura, int altura);

#endif
