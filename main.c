#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "gravidade.h"  // NOVO: a bola fica no gravidade.c
#include "fundo.h"      // NOVO: fundo animado (GIF da torcida)
#include "menu.h"       // NOVO: menu inicial, configurações e tela de fim

//para ter movimento em cima da cabeça do outro jogador
bool em_cima_do_jogador2=false;
bool em_cima_do_jogador1=false;

// NOVO: true enquanto o jogador segura a tecla de chute.
// Nesse tempo W / seta-cima mudam o ângulo do chute, então não fazem o jogador pular.
bool carregando_chute1=false;
bool carregando_chute2=false;

//verifica se o jogador pode ocupar uma nova posição
bool pode_mover(
    int nova_posicao,
    int x_outro,
    int y_jogador,
    int y_outro,
    int largura_boneco,
    int altura_boneco
) {
    bool colisao_vertical =y_jogador < y_outro + altura_boneco &&y_jogador + altura_boneco > y_outro;
    bool colisao_horizontal =nova_posicao < x_outro + largura_boneco &&nova_posicao + largura_boneco > x_outro;
    if(em_cima_do_jogador1){
        return true;
    }
    if(em_cima_do_jogador2){
        return true;
    }
    return !(colisao_vertical && colisao_horizontal);
}

// Função de pulo
void pular(
    int altura,
    int altura_boneco,
    int largura_boneco,
    float gravidade,
    float forca_pulo,
    int *x_jogador1,
    int *x_jogador2,
    int *y_jogador1,
    int *y_jogador2,
    float *velocidade_pulo_jogador1,
    float *velocidade_pulo_jogador2,
    bool *no_ar
) {//  guardar as posições anteriores
    int y_anterior1 = *y_jogador1;
    int y_anterior2 = *y_jogador2;

    // Pulo jogador 1
    if (IsKeyPressed(KEY_W) && !carregando_chute1) {
        if (*y_jogador1 >= altura - altura_boneco || em_cima_do_jogador2==true) {
            *velocidade_pulo_jogador1 = forca_pulo;
        }
    }// Pulo jogador 2
    if (IsKeyPressed(KEY_UP) && !carregando_chute2) {
        if (*y_jogador2 >= altura - altura_boneco || em_cima_do_jogador1==true) {
            *velocidade_pulo_jogador2 = forca_pulo;
        }
    }

    // Gravidade
    *velocidade_pulo_jogador1 += gravidade;
    *velocidade_pulo_jogador2 += gravidade;
    *y_jogador1 += *velocidade_pulo_jogador1;
    *y_jogador2 += *velocidade_pulo_jogador2;

    //jogador1 na cabeças do jogador 2
    if (*velocidade_pulo_jogador1 > 0 &&y_anterior1 + altura_boneco <= *y_jogador2 &&*y_jogador1 + altura_boneco >= *y_jogador2 &&*x_jogador1 < *x_jogador2 + largura_boneco &&*x_jogador1 + largura_boneco > *x_jogador2) {
        *y_jogador1 = *y_jogador2 - altura_boneco;
        *velocidade_pulo_jogador1 = 0;
        em_cima_do_jogador2=true;
    }else{em_cima_do_jogador2=false;}

    // jogador 2 pousando na cabeça do jogador 1
    if (*velocidade_pulo_jogador2 > 0 &&y_anterior2 + altura_boneco <= *y_jogador1 &&*y_jogador2 + altura_boneco >= *y_jogador1 &&*x_jogador2 < *x_jogador1 + largura_boneco &&*x_jogador2 + largura_boneco > *x_jogador1) {
        *y_jogador2 = *y_jogador1 - altura_boneco;
        *velocidade_pulo_jogador2 = 0;
        em_cima_do_jogador1=true;
    }else{em_cima_do_jogador1=false;}

    // Limite do chão jogador 1
    if (*y_jogador1 >= altura - altura_boneco) {
        *y_jogador1 = altura - altura_boneco;
        *velocidade_pulo_jogador1 = 0;
    }// Limite do chão jogador 2
    if (*y_jogador2 >= altura - altura_boneco) {
        *y_jogador2 = altura - altura_boneco;
        *velocidade_pulo_jogador2 = 0;
    }
    // atualizar o estado geral do ar
    *no_ar = (*velocidade_pulo_jogador1 != 0 ||*velocidade_pulo_jogador2 != 0);
}

// Movimentação jogador 1
void movimentacao_jogador1(
    int *x_jogador1,
    int *x_jogador2,
    int *y_jogador1,
    int *y_jogador2,
    int largura,
    int largura_boneco,
    int altura_boneco,
    int x_inicial_jogador1,
    int velo_boneco,
    Texture2D *direita_jogador1,
    Texture2D *esquerda_jogador1,
    Texture2D *atual_jogador1
) {

    // Mover direita
    if (*x_jogador1 < largura - largura_boneco) {
        //  verificar a próxima posição antes de mover
        if (pode_mover(
            *x_jogador1 + velo_boneco,
            *x_jogador2,
            *y_jogador1,
            *y_jogador2,
            largura_boneco,
            altura_boneco
        )) {
            if (IsKeyDown(KEY_D)) {
                *atual_jogador1 = *direita_jogador1;
                *x_jogador1 += velo_boneco;
            }
        }
    }
    // Mover esquerda
    if (*x_jogador1 > x_inicial_jogador1) {
        //  verificar a próxima posição antes de mover
        if (pode_mover(
            *x_jogador1 - velo_boneco,
            *x_jogador2,
            *y_jogador1,
            *y_jogador2,
            largura_boneco,
            altura_boneco
        )) {
            if (IsKeyDown(KEY_A)) {
                *atual_jogador1 = *esquerda_jogador1;
                *x_jogador1 -= velo_boneco;
            }
        }
    }
}
// Movimentação jogador 2
void movimentacao_jogador2(
    int *x_jogador2,
    int *x_jogador1,
    int *y_jogador2,
    int *y_jogador1,
    int largura,
    int largura_boneco,
    int altura_boneco,
    int velo_boneco,
    Texture2D *direita_jogador2,
    Texture2D *esquerda_jogador2,
    Texture2D *atual_jogador2
) {
    // Mover direita
    if (*x_jogador2 < largura - largura_boneco) {
        //verificar a próxima posição antes de mover
        if (pode_mover(
            *x_jogador2 + velo_boneco,
            *x_jogador1,
            *y_jogador2,
            *y_jogador1,
            largura_boneco,
            altura_boneco
        )) {
            if (IsKeyDown(KEY_RIGHT)) {
                *atual_jogador2 = *direita_jogador2;
                *x_jogador2 += velo_boneco;
            }
        }
    }
    // Mover esquerda
    if (*x_jogador2 > 0) {
        // verificar a próxima posição antes de mover
        if (pode_mover(
            *x_jogador2 - velo_boneco,
            *x_jogador1,
            *y_jogador2,
            *y_jogador1,
            largura_boneco,
            altura_boneco
        )) {
            if (IsKeyDown(KEY_LEFT)) {
                *atual_jogador2 = *esquerda_jogador2;
                *x_jogador2 -= velo_boneco;
            }
        }
    }
}

int main() {

    // Tela menor: cabe num notebook 1366x768 com a barra de tarefas.
    // Tudo abaixo se ajusta sozinho pela "escala" (o jogo foi feito pra 900 de altura).
    const int largura = 1200, altura = 650;
    const float escala = altura / 900.0f;

    const int altura_boneco = 230 * escala, largura_boneco = 80 * escala;
    const float gravidade = 1.0f;
    const int x_inicial_jogador1 = 0;
    const int x_inicial_jogador2 = largura;
    const int velo_boneco = 7 * escala + 0.5f;

    // Movimento jogador 1
    int x_jogador1 = x_inicial_jogador1;
    // Movimento jogador 2
    int x_jogador2 = x_inicial_jogador2 - largura_boneco;

    // Pulo e gravidade
    int y_jogador1 = altura - altura_boneco;
    int y_jogador2 = altura - altura_boneco;
    float velocidade_pulo_jogador1 = 0.0f;
    float velocidade_pulo_jogador2 = 0.0f;
    float forca_pulo = -23.0f * sqrtf(escala);  // mesma proporção de altura do pulo
    bool no_ar = false;

    // Mostrar X jogador 1
    char texto_coordenada[50];

    // Abrir janela
    SetTraceLogLevel(LOG_WARNING);  // NOVO: esconde as mensagens INFO do terminal
    InitWindow(largura, altura, "futebol");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);  // NOVO: ESC não fecha mais a janela direto; o jogo controla (volta ao menu)

    // NOVO: acha a pasta "jogador" mesmo se o jogo rodar de dentro da pasta output
    if (!DirectoryExists("jogador")) {
        ChangeDirectory(GetApplicationDirectory());
        if (!DirectoryExists("jogador")) ChangeDirectory("..");
    }

    // Imagens jogador
    Texture2D direita_jogador1 =LoadTexture("jogador/amarelo/direita/jogador_amarelo_direita_parado.png");
    Texture2D esquerda_jogador1 =LoadTexture("jogador/amarelo/esquerda/jogador_amarelo_esquerda_parado.png");
    Texture2D direita_jogador2 =LoadTexture("jogador/azul/direita/jogador_azul_direita_parado.png");
    Texture2D esquerda_jogador2 =LoadTexture("jogador/azul/esquerda/jogador_azul_esquerda_parado.png");
    Texture2D atual_jogador1 = direita_jogador1;
    Texture2D atual_jogador2 = esquerda_jogador2;

    // NOVO: sprites da bola (normal e fogo, direita e esquerda)
    Texture2D sprite_bola = carregar_sprite_bola("bola/bola.png");

    // NOVO: chute. Segure a tecla para carregar a força; cima/baixo mudam o ângulo; solte para chutar.
    //   jogador 1: tecla E, ângulo com W/S
    //   jogador 2: SHIFT direito, ângulo com seta cima/baixo
    Chute chute1 = criar_chute(KEY_E, KEY_NULL, KEY_W, KEY_S);
    Chute chute2 = criar_chute(KEY_RIGHT_SHIFT, KEY_NULL, KEY_UP, KEY_DOWN);
    const float forca_toque = 6 * escala;   // só encostar na bola dá um toquinho (o chute forte é pela tecla)

    // NOVO: bola começa no meio, caindo do alto
    Bola bola = criar_bola(largura / 2.0f, altura / 10.0f, 25 * escala);

    // NOVO: os dois gols (cada um com a sua imagem) e o placar
    Gol gol_esquerdo = criar_gol("gol/gol_esquerdo.png", true,  largura, altura, 320 * escala);
    Gol gol_direito  = criar_gol("gol/gol_direito.png",  false, largura, altura, 320 * escala);
    int gols_jogador1 = 0, gols_jogador2 = 0;

    // NOVO: fundo animado; 0.1f = segundos por quadro (aumente para deixar mais lento)
    Fundo fundo = criar_fundo("fundo/torcida.gif", 0.1f);


    // NOVO: o jogo abre no menu
    Tela tela = TELA_MENU;
    Menu menu = criar_menu();
    float tempo_restante = 0;
    bool sair = false;

    // Loop principal
    while (!WindowShouldClose() && !sair) {
        bool resetar = false;   // true = jogadores e bola voltam para o começo
        atualizar_fundo(&fundo);

        // ===================== ATUALIZAR =====================
        if (tela == TELA_MENU) {
            AcaoMenu acao = atualizar_menu(&menu);
            if (acao == ACAO_JOGAR) {
                gols_jogador1 = 0;
                gols_jogador2 = 0;
                tempo_restante = minutos_da_partida(menu) * 60.0f;
                resetar = true;
                tela = TELA_JOGANDO;
            } else if (acao == ACAO_CONFIG) {
                tela = TELA_CONFIG;
            }
            if (IsKeyPressed(KEY_ESCAPE)) sair = true;
        }
        else if (tela == TELA_CONFIG) {
            if (atualizar_config(&menu) || IsKeyPressed(KEY_ESCAPE)) tela = TELA_MENU;
        }
        else if (tela == TELA_JOGANDO) {
            if (IsKeyPressed(KEY_ESCAPE)) tela = TELA_MENU;

            carregando_chute1 = chute1.carregando;   // NOVO: avisa o pulo para não pular enquanto mira
            carregando_chute2 = chute2.carregando;

            sprintf(texto_coordenada, "X do Jogador: %d", x_jogador1);
            movimentacao_jogador1(
                &x_jogador1,
                &x_jogador2,
                &y_jogador1,
                &y_jogador2,
                largura,
                largura_boneco,
                altura_boneco,
                x_inicial_jogador1,
                velo_boneco,
                &direita_jogador1,
                &esquerda_jogador1,
                &atual_jogador1
            );
            movimentacao_jogador2(
                &x_jogador2,
                &x_jogador1,
                &y_jogador2,
                &y_jogador1,
                largura,
                largura_boneco,
                altura_boneco,
                velo_boneco,
                &direita_jogador2,
                &esquerda_jogador2,
                &atual_jogador2
            );
            pular(
                altura,
                altura_boneco,
                largura_boneco,
                gravidade,
                forca_pulo,
                &x_jogador1,
                &x_jogador2,
                &y_jogador1,
                &y_jogador2,
                &velocidade_pulo_jogador1,
                &velocidade_pulo_jogador2,
                &no_ar
            );

            // NOVO: bola cai, quica e é chutada pelos jogadores
            atualizar_bola(&bola, gravidade, largura, altura);
            Rectangle corpo_jogador1 = { x_jogador1, y_jogador1, atual_jogador1.width * escala, atual_jogador1.height * escala };
            Rectangle corpo_jogador2 = { x_jogador2, y_jogador2, atual_jogador2.width * escala, atual_jogador2.height * escala };
            chutar_bola(&bola, corpo_jogador1, forca_toque);   // toque: só empurra a bola
            chutar_bola(&bola, corpo_jogador2, forca_toque);

            // NOVO: chute com tecla (vem depois do toque, então o chute forte vale por cima)
            atualizar_chute(&chute1, &bola, corpo_jogador1, atual_jogador1.id == direita_jogador1.id, escala);
            atualizar_chute(&chute2, &bola, corpo_jogador2, atual_jogador2.id == direita_jogador2.id, escala);

            // NOVO: travessão e gol
            colidir_travessao(&bola, gol_esquerdo);
            colidir_travessao(&bola, gol_direito);

            // Jogador 1 defende a esquerda, jogador 2 defende a direita
            int quem_marcou = 0;
            if (bola_entrou(bola, gol_esquerdo)) quem_marcou = 2;
            if (bola_entrou(bola, gol_direito))  quem_marcou = 1;

            if (quem_marcou != 0) {
                if (quem_marcou == 1) gols_jogador1++;
                else                  gols_jogador2++;
                resetar = true;
            }

            // NOVO: cronômetro da partida
            tempo_restante -= GetFrameTime();
            if (tempo_restante <= 0) {
                tempo_restante = 0;
                tela = TELA_FIM;
            }
        }
        else if (tela == TELA_FIM) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) tela = TELA_MENU;
        }

        // Reseta jogadores e bola para o começo (início da partida ou depois de um gol)
        if (resetar) {
            x_jogador1 = x_inicial_jogador1;
            x_jogador2 = x_inicial_jogador2 - largura_boneco;
            y_jogador1 = altura - altura_boneco;
            y_jogador2 = altura - altura_boneco;
            velocidade_pulo_jogador1 = 0;
            velocidade_pulo_jogador2 = 0;
            no_ar = false;
            em_cima_do_jogador1 = false;
            em_cima_do_jogador2 = false;
            atual_jogador1 = direita_jogador1;
            atual_jogador2 = esquerda_jogador2;
            bola = criar_bola(largura / 2.0f, altura / 10.0f, 25 * escala);
            chute1 = criar_chute(KEY_E, KEY_NULL, KEY_W, KEY_S);   // NOVO: zera carga e recarga
            chute2 = criar_chute(KEY_RIGHT_SHIFT, KEY_NULL, KEY_UP, KEY_DOWN);
        }

        // ===================== DESENHAR =====================
        BeginDrawing();
            ClearBackground(RAYWHITE);
            desenhar_fundo(fundo, largura, altura);  // primeiro de tudo = fica atrás

            // A partida (também aparece parada atrás da tela de fim)
            if (tela == TELA_JOGANDO || tela == TELA_FIM) {
                // imagens diminuídas pela escala pra caber na tela menor
                DrawTextureEx(atual_jogador1, (Vector2){ x_jogador1, y_jogador1 }, 0, escala, WHITE);
                DrawTextureEx(atual_jogador2, (Vector2){ x_jogador2, y_jogador2 }, 0, escala, WHITE);
                desenhar_bola(bola, sprite_bola);

                // NOVO: barra de força, seta do ângulo e recarga do chute
                if (tela == TELA_JOGANDO) {
                    Rectangle corpo1 = { x_jogador1, y_jogador1, atual_jogador1.width * escala, atual_jogador1.height * escala };
                    Rectangle corpo2 = { x_jogador2, y_jogador2, atual_jogador2.width * escala, atual_jogador2.height * escala };
                    desenhar_chute(chute1, corpo1, atual_jogador1.id == direita_jogador1.id, escala);
                    desenhar_chute(chute2, corpo2, atual_jogador2.id == direita_jogador2.id, escala);
                }

                // gols por último = primeiro plano (bola e jogadores passam por trás da rede)
                desenhar_gol(gol_esquerdo);
                desenhar_gol(gol_direito);

                // placar e relógio
                const char *placar = TextFormat("%d  x  %d", gols_jogador1, gols_jogador2);
                DrawText(placar, largura / 2 - MeasureText(placar, 40) / 2, 15, 40, BLACK);

                int segundos = (int)ceilf(tempo_restante);
                const char *relogio = TextFormat("%02d:%02d", segundos / 60, segundos % 60);
                Color cor_relogio = (tempo_restante <= 10 && tela == TELA_JOGANDO) ? RED : BLACK;
                DrawText(relogio, largura / 2 - MeasureText(relogio, 30) / 2, 60, 30, cor_relogio);
            }

            // Telas por cima
            if (tela == TELA_MENU)        desenhar_menu(menu, largura, altura);
            else if (tela == TELA_CONFIG) desenhar_config(menu, largura, altura);
            else if (tela == TELA_FIM)    desenhar_fim(gols_jogador1, gols_jogador2, largura, altura);
        EndDrawing();
    }
    // Liberar texturas
    UnloadTexture(direita_jogador1);
    UnloadTexture(esquerda_jogador1);
    UnloadTexture(direita_jogador2);
    UnloadTexture(esquerda_jogador2);
    descarregar_sprite_bola(sprite_bola);  // NOVO
    descarregar_gol(gol_esquerdo);  // NOVO
    descarregar_gol(gol_direito);   // NOVO
    descarregar_fundo(fundo);       // NOVO
    // Fechar janela
    CloseWindow();
    return 0;
}