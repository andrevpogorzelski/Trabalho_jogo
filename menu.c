// menu.c: tela inicial, configurações e tela de fim de jogo
#include "menu.h"

static const int OPCOES_TEMPO[] = { 2, 3, 5 };   // minutos; para ter outro tempo, é só incluir aqui
#define QTD_TEMPOS ((int)(sizeof(OPCOES_TEMPO) / sizeof(OPCOES_TEMPO[0])))

static const char *TEXTO_MENU[TOTAL_ITENS_MENU] = {
    [ITEM_JOGAR]  = "Jogar",
    [ITEM_CONFIG] = "Configurações",
};

// ---------- teclas (aceita as teclas dos dois jogadores e as setas) ----------
static bool tecla_cima(void)  { return IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W); }
static bool tecla_baixo(void) { return IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S); }
static bool tecla_esq(void)   { return IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A); }
static bool tecla_dir(void)   { return IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D); }
static bool tecla_ok(void)    { return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_SPACE); }

// move a seleção para cima/baixo, dando a volta no começo/fim da lista
static int navegar(int selecionado, int total) {
    if (tecla_baixo()) selecionado = (selecionado + 1) % total;
    if (tecla_cima())  selecionado = (selecionado + total - 1) % total;
    return selecionado;
}

// ---------- criação ----------
Menu criar_menu(void) {
    Menu m = { 0, 0, 1 };    // começa em "3 min"
    return m;
}

int minutos_da_partida(Menu menu) {
    return OPCOES_TEMPO[menu.indice_tempo];
}

// ---------- lógica ----------
AcaoMenu atualizar_menu(Menu *m) {
    m->sel_menu = navegar(m->sel_menu, TOTAL_ITENS_MENU);

    if (tecla_ok()) {
        if (m->sel_menu == ITEM_JOGAR)  return ACAO_JOGAR;
        if (m->sel_menu == ITEM_CONFIG) return ACAO_CONFIG;
    }
    return ACAO_NENHUMA;
}

bool atualizar_config(Menu *m) {
    m->sel_config = navegar(m->sel_config, TOTAL_ITENS_CONFIG);

    if (m->sel_config == CONFIG_TEMPO) {
        if (tecla_dir() || tecla_ok()) m->indice_tempo = (m->indice_tempo + 1) % QTD_TEMPOS;
        if (tecla_esq())               m->indice_tempo = (m->indice_tempo + QTD_TEMPOS - 1) % QTD_TEMPOS;
    }
    if (m->sel_config == CONFIG_VOLTAR && tecla_ok()) return true;
    return false;
}

// ---------- desenho ----------
// texto centralizado em cx, com uma sombra para ler bem sobre o fundo
static void texto_centralizado(const char *texto, int cx, int y, int tamanho, Color cor) {
    int x = cx - MeasureText(texto, tamanho) / 2;
    DrawText(texto, x + 3, y + 3, tamanho, Fade(BLACK, 0.7f));
    DrawText(texto, x, y, tamanho, cor);
}

static void escurecer_fundo(int largura, int altura) {
    DrawRectangle(0, 0, largura, altura, Fade(BLACK, 0.55f));
}

void desenhar_menu(Menu m, int largura, int altura) {
    float k = altura / 650.0f;     // tudo proporcional ao tamanho da janela
    int cx = largura / 2;

    escurecer_fundo(largura, altura);
    texto_centralizado("FUTEBOL", cx, (int)(altura * 0.16f), (int)(90 * k), RAYWHITE);

    for (int i = 0; i < TOTAL_ITENS_MENU; i++) {
        Color cor = (i == m.sel_menu) ? ORANGE : LIGHTGRAY;
        texto_centralizado(TEXTO_MENU[i], cx, (int)(altura * 0.46f + i * 72 * k), (int)(44 * k), cor);
    }

    texto_centralizado("W/S ou setas: navegar     ENTER: escolher     ESC: sair",
                       cx, altura - (int)(50 * k), (int)(20 * k), LIGHTGRAY);
}

void desenhar_config(Menu m, int largura, int altura) {
    float k = altura / 650.0f;
    int cx = largura / 2;

    escurecer_fundo(largura, altura);
    texto_centralizado("Configurações", cx, (int)(altura * 0.16f), (int)(70 * k), RAYWHITE);

    const char *textos[TOTAL_ITENS_CONFIG];
    textos[CONFIG_TEMPO]  = TextFormat("Tempo da partida:  <  %d min  >", OPCOES_TEMPO[m.indice_tempo]);
    textos[CONFIG_VOLTAR] = "Voltar";

    for (int i = 0; i < TOTAL_ITENS_CONFIG; i++) {
        Color cor = (i == m.sel_config) ? ORANGE : LIGHTGRAY;
        texto_centralizado(textos[i], cx, (int)(altura * 0.46f + i * 72 * k), (int)(40 * k), cor);
    }

    texto_centralizado("W/S ou setas: navegar     A/D ou setas: alterar     ESC: voltar",
                       cx, altura - (int)(50 * k), (int)(20 * k), LIGHTGRAY);
}

void desenhar_fim(int gols_jogador1, int gols_jogador2, int largura, int altura) {
    float k = altura / 650.0f;
    int cx = largura / 2;

    escurecer_fundo(largura, altura);
    texto_centralizado("FIM DE JOGO", cx, (int)(altura * 0.18f), (int)(70 * k), RAYWHITE);

    const char *resultado = gols_jogador1 > gols_jogador2 ? "Amarelo venceu!" :
                            gols_jogador2 > gols_jogador1 ? "Azul venceu!"    : "Empate!";
    texto_centralizado(resultado, cx, (int)(altura * 0.40f), (int)(50 * k), ORANGE);
    texto_centralizado(TextFormat("%d  x  %d", gols_jogador1, gols_jogador2),
                       cx, (int)(altura * 0.55f), (int)(60 * k), RAYWHITE);
    texto_centralizado("ENTER: voltar ao menu", cx, altura - (int)(50 * k), (int)(22 * k), LIGHTGRAY);
}
