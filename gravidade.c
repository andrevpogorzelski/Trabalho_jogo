// gravidade.c: a bola do jogo (sem main; quem tem main é o main.c)
#include "gravidade.h"
#include <math.h>

Bola criar_bola(float x, float y, float raio) {
    Bola bola = { x, y, 0, 0, raio, 0.9f };
    return bola;
}

void atualizar_bola(Bola *bola, float gravidade, int largura, int altura) {
    bola->velocidade_y += gravidade;  // adiciona gravidade por frame
    bola->x += bola->velocidade_x;
    bola->y += bola->velocidade_y;

    // Chão: quica e vai perdendo força
    if (bola->y > altura - bola->raio) {
        bola->y = altura - bola->raio;  // para não afundar
        bola->velocidade_y = -bola->velocidade_y * bola->resistencia;
        if (fabsf(bola->velocidade_y) < 1.5f) bola->velocidade_y = 0;  // para de quicar

        bola->velocidade_x *= 0.98f;  // rolando no chão, vai freando
        if (fabsf(bola->velocidade_x) < 0.05f) bola->velocidade_x = 0;
    }

    // Teto
    if (bola->y < bola->raio) {
        bola->y = bola->raio;
        bola->velocidade_y = -bola->velocidade_y * bola->resistencia;
    }

    // Paredes
    if (bola->x < bola->raio) {
        bola->x = bola->raio;
        bola->velocidade_x = -bola->velocidade_x * bola->resistencia;
    }
    if (bola->x > largura - bola->raio) {
        bola->x = largura - bola->raio;
        bola->velocidade_x = -bola->velocidade_x * bola->resistencia;
    }

    // NOVO: animação de rolar. Quanto mais rápido anda, mais rápido gira
    // (0.07 quadro por pixel = uma volta completa a cada ~113 px, o perímetro da bola)
    if (bola->velocidade_x > 0) bola->olhando_esquerda = false;
    if (bola->velocidade_x < 0) bola->olhando_esquerda = true;
    bola->fase = fmodf(bola->fase + fabsf(bola->velocidade_x) * 0.07f, FRAMES_BOLA);
}

// Jogador encostou na bola: ela sai pro lado oposto e sobe um pouco
void chutar_bola(Bola *bola, Rectangle jogador, float forca) {
    if (!CheckCollisionCircleRec((Vector2){ bola->x, bola->y }, bola->raio, jogador)) return;

    float centro_jogador = jogador.x + jogador.width / 2;
    if (bola->x >= centro_jogador) {
        bola->velocidade_x = forca;
        bola->x = jogador.x + jogador.width + bola->raio;
    } else {
        bola->velocidade_x = -forca;
        bola->x = jogador.x - bola->raio;
    }
    bola->velocidade_y = -forca * 0.6f;
}

// ---------- sprite da bola ----------
// A folha bola/bola.png tem 4 linhas de 8 quadros de 36x36:
//   linha 0 = normal indo p/ direita   linha 1 = normal indo p/ esquerda
//   linha 2 = fogo   indo p/ direita   linha 3 = fogo   indo p/ esquerda
#define TAM_QUADRO 36

Texture2D carregar_sprite_bola(const char *arquivo) {
    Texture2D sprite = { 0 };
    if (!FileExists(arquivo)) return sprite;               // sem imagem: bola laranja provisória
    sprite = LoadTexture(arquivo);
    SetTextureFilter(sprite, TEXTURE_FILTER_POINT);        // pixel art nítida
    return sprite;
}

void descarregar_sprite_bola(Texture2D sprite) {
    if (sprite.id != 0) UnloadTexture(sprite);
}

void desenhar_bola(Bola bola, Texture2D sprite) {
    if (sprite.id == 0) {
        DrawCircle(bola.x, bola.y, bola.raio, ORANGE);     // imagem não encontrada
        return;
    }

    // Bola rápida (chute forte) = bola de fogo. Ajuste o 0.5f para o fogo aparecer mais/menos
    bool fogo = fabsf(bola.velocidade_x) > bola.raio * 0.5f;
    int linha  = (fogo ? 2 : 0) + (bola.olhando_esquerda ? 1 : 0);
    int quadro = (int)bola.fase % FRAMES_BOLA;

    // Onde fica o "corpo" da bola dentro do quadro de 36x36 (a chama fica de fora dele).
    // Assim a bola tem o mesmo tamanho (e a mesma colisão) com ou sem fogo.
    float corpo, centro_x, centro_y = 18;
    if (!fogo)                    { corpo = 32; centro_x = 18; }
    else if (bola.olhando_esquerda) { corpo = 26; centro_x = 12.5f; centro_y = 17.5f; }
    else                          { corpo = 26; centro_x = 22.5f; centro_y = 17.5f; }

    float k = (2 * bola.raio) / corpo;                     // escala: corpo = diâmetro da colisão
    Rectangle origem  = { (float)(quadro * TAM_QUADRO), (float)(linha * TAM_QUADRO), TAM_QUADRO, TAM_QUADRO };
    Rectangle destino = { bola.x - centro_x * k, bola.y - centro_y * k, TAM_QUADRO * k, TAM_QUADRO * k };
    DrawTexturePro(sprite, origem, destino, (Vector2){ 0, 0 }, 0, WHITE);
}

// ===================== GOL =====================

Gol criar_gol(const char *arquivo, bool lado_esquerdo, int largura_tela, int altura_tela, float altura_gol) {
    Gol gol = { 0 };
    gol.lado_esquerdo = lado_esquerdo;

    // Carrega a imagem e corta as bordas transparentes,
    // assim o gol na tela tem exatamente o tamanho do desenho
    if (FileExists(arquivo)) {
        Image imagem = LoadImage(arquivo);
        ImageAlphaCrop(&imagem, 0.1f);
        gol.sprite = LoadTextureFromImage(imagem);
        UnloadImage(imagem);
    }

    // Altura fixa; largura proporcional à imagem (sem imagem: gol provisório)
    float largura_gol = gol.sprite.id ? altura_gol * gol.sprite.width / gol.sprite.height : altura_gol * 1.25f;

    gol.area.width  = largura_gol;
    gol.area.height = altura_gol;
    gol.area.y = altura_tela - altura_gol;                       // apoiado no chão
    gol.area.x = lado_esquerdo ? 0 : largura_tela - largura_gol; // encostado na parede
    gol.altura_travessao = altura_gol * 0.08f;                   // ajuste se o travessão do desenho for mais grosso
    return gol;
}

static Rectangle travessao_do_gol(Gol gol) {
    return (Rectangle){ gol.area.x, gol.area.y, gol.area.width, gol.altura_travessao };
}

// A bola quica no travessão em vez de entrar por cima
void colidir_travessao(Bola *bola, Gol gol) {
    Rectangle t = travessao_do_gol(gol);
    if (!CheckCollisionCircleRec((Vector2){ bola->x, bola->y }, bola->raio, t)) return;

    if (bola->y < t.y) {
        // Caiu em cima do travessão
        bola->y = t.y - bola->raio;
        if (bola->velocidade_y > 0) bola->velocidade_y = -bola->velocidade_y * bola->resistencia;
        if (fabsf(bola->velocidade_y) < 1.5f) bola->velocidade_y = 0;
        bola->velocidade_x *= 0.98f;
    } else if (bola->y > t.y + t.height) {
        // Bateu por baixo
        bola->y = t.y + t.height + bola->raio;
        if (bola->velocidade_y < 0) bola->velocidade_y = -bola->velocidade_y * bola->resistencia;
    } else {
        // Bateu na ponta do travessão: volta pro campo
        if (gol.lado_esquerdo) bola->x = t.x + t.width + bola->raio;
        else                   bola->x = t.x - bola->raio;
        bola->velocidade_x = -bola->velocidade_x * bola->resistencia;
    }
}

// Gol = a bola inteira passou do X limite (a linha do gol) e está abaixo do travessão
bool bola_entrou(Bola bola, Gol gol) {
    if (bola.y - bola.raio < gol.area.y + gol.altura_travessao) return false;

    if (gol.lado_esquerdo) {
        float x_limite = gol.area.x + gol.area.width;  // frente do gol esquerdo
        return bola.x + bola.raio < x_limite;
    } else {
        float x_limite = gol.area.x;                    // frente do gol direito
        return bola.x - bola.raio > x_limite;
    }
}

void desenhar_gol(Gol gol) {
    if (gol.sprite.id == 0) {
        // Imagem não encontrada: gol provisório
        DrawRectangleLinesEx(gol.area, 3, GRAY);
        DrawRectangleRec(travessao_do_gol(gol), DARKGRAY);
        return;
    }
    Rectangle origem = { 0, 0, (float)gol.sprite.width, (float)gol.sprite.height };
    DrawTexturePro(gol.sprite, origem, gol.area, (Vector2){ 0, 0 }, 0, WHITE);
}

void descarregar_gol(Gol gol) {
    if (gol.sprite.id != 0) UnloadTexture(gol.sprite);
}

// ===================== CHUTE =====================

Chute criar_chute(int tecla_chute, int tecla_chute_alt, int tecla_cima, int tecla_baixo) {
    Chute chute = { tecla_chute, tecla_chute_alt, tecla_cima, tecla_baixo, false, 0, ANGULO_INICIAL, 0 };
    return chute;
}

void atualizar_chute(Chute *chute, Bola *bola, Rectangle corpo, bool olhando_direita, float escala) {
    float dt = GetFrameTime();

    // Recarga: conta o tempo até poder chutar de novo
    if (chute->recarga > 0) {
        chute->recarga -= dt;
        if (chute->recarga < 0) chute->recarga = 0;
    }

    bool segurando = IsKeyDown(chute->tecla_chute)    || IsKeyDown(chute->tecla_chute_alt);
    bool apertou   = IsKeyPressed(chute->tecla_chute) || IsKeyPressed(chute->tecla_chute_alt);

    // Começa a carregar (precisa apertar de novo: segurar desde antes não vale)
    if (!chute->carregando) {
        if (apertou && chute->recarga <= 0) {
            chute->carregando = true;
            chute->carga = 0;
            chute->angulo = ANGULO_INICIAL;
        }
        return;
    }

    // Carregando: a força sobe com o tempo e cima/baixo mudam o ângulo
    chute->carga += dt;
    if (chute->carga > TEMPO_CARGA_MAX) chute->carga = TEMPO_CARGA_MAX;
    if (IsKeyDown(chute->tecla_cima))  chute->angulo += VELOCIDADE_MIRA * dt;
    if (IsKeyDown(chute->tecla_baixo)) chute->angulo -= VELOCIDADE_MIRA * dt;
    if (chute->angulo > ANGULO_MAX) chute->angulo = ANGULO_MAX;
    if (chute->angulo < ANGULO_MIN) chute->angulo = ANGULO_MIN;

    if (segurando) return;   // ainda segurando: continua carregando

    // Soltou a tecla: chuta (se a bola estiver ao alcance) e começa a recarga
    chute->carregando = false;
    chute->recarga = RECARGA_CHUTE;

    // Zona do chute = o corpo do jogador + um pouco para a frente (a direção em que ele olha)
    float alcance = ALCANCE_CHUTE * escala;
    Rectangle zona = { corpo.x, corpo.y - alcance * 0.4f, corpo.width + alcance, corpo.height + alcance * 0.8f };
    if (!olhando_direita) zona.x -= alcance;

    if (CheckCollisionCircleRec((Vector2){ bola->x, bola->y }, bola->raio, zona)) {
        float forca = (FORCA_CHUTE_MIN + (FORCA_CHUTE_MAX - FORCA_CHUTE_MIN) * (chute->carga / TEMPO_CARGA_MAX)) * escala;
        float rad = chute->angulo * DEG2RAD;
        bola->velocidade_x = (olhando_direita ? 1 : -1) * forca * cosf(rad);
        bola->velocidade_y = -forca * sinf(rad);
    }
    chute->carga = 0;
}

// Barra de força + seta do ângulo enquanto carrega; barrinha de recarga depois do chute
void desenhar_chute(Chute chute, Rectangle corpo, bool olhando_direita, float escala) {
    float topo = corpo.y - 14;
    if (topo < 4) topo = 4;                                  // não sai da tela quando pula alto
    Rectangle barra = { corpo.x, topo, corpo.width, 8 };

    if (chute.carregando) {
        float t = chute.carga / TEMPO_CARGA_MAX;
        Color cor = { 255, (unsigned char)(230 - 200 * t), 0, 255 };   // amarelo -> vermelho

        DrawRectangleRec(barra, DARKGRAY);
        DrawRectangleRec((Rectangle){ barra.x, barra.y, barra.width * t, barra.height }, cor);
        DrawRectangleLinesEx(barra, 2, BLACK);

        // seta do ângulo: sai da altura do pé, na direção em que o jogador olha
        float rad = chute.angulo * DEG2RAD;
        float dir = olhando_direita ? 1.0f : -1.0f;
        Vector2 origem = { olhando_direita ? corpo.x + corpo.width : corpo.x, corpo.y + corpo.height * 0.85f };
        float comprimento = (35 + 45 * t) * escala;
        Vector2 ponta = { origem.x + dir * cosf(rad) * comprimento, origem.y - sinf(rad) * comprimento };
        DrawLineEx(origem, ponta, 3, cor);
        DrawCircleV(ponta, 5, cor);
    }
    else if (chute.recarga > 0) {
        float pronto = 1.0f - chute.recarga / RECARGA_CHUTE;  // enche até poder chutar de novo
        DrawRectangleRec(barra, GRAY);
        DrawRectangleRec((Rectangle){ barra.x, barra.y, barra.width * pronto, barra.height }, SKYBLUE);
        DrawRectangleLinesEx(barra, 2, BLACK);
    }
}
