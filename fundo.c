// fundo.c: fundo animado do jogo (GIF da torcida)
#include "fundo.h"
#include <stddef.h>   // define o NULL

Fundo criar_fundo(const char *arquivo, float duracao_quadro) {
    Fundo f = { 0 };
    f.duracao_quadro = duracao_quadro;

    // Se o GIF não existir, o fundo fica desligado e o jogo continua normal
    if (!FileExists(arquivo)) return f;

    f.animacao = LoadImageAnim(arquivo, &f.quadros);
    if (f.animacao.data == NULL) { f.quadros = 0; return f; }

    f.textura = LoadTextureFromImage(f.animacao);          // começa no 1º quadro
    SetTextureFilter(f.textura, TEXTURE_FILTER_POINT);     // pixel art nítida
    return f;
}

void atualizar_fundo(Fundo *f) {
    if (f->quadros <= 1) return;

    f->tempo += GetFrameTime();
    if (f->tempo < f->duracao_quadro) return;
    f->tempo = 0;

    f->quadro_atual = (f->quadro_atual + 1) % f->quadros;
    unsigned int deslocamento = f->animacao.width * f->animacao.height * 4 * f->quadro_atual;
    UpdateTexture(f->textura, ((unsigned char *)f->animacao.data) + deslocamento);
}

void desenhar_fundo(Fundo f, int largura, int altura) {
    if (f.textura.id == 0) return;

    // Corta o GIF para ter a proporção da janela, mantendo a parte de baixo (gramado)
    float altura_util = f.textura.width * ((float)altura / largura);
    if (altura_util > f.textura.height) altura_util = (float)f.textura.height;
    Rectangle origem  = { 0, f.textura.height - altura_util, (float)f.textura.width, altura_util };
    Rectangle destino = { 0, 0, (float)largura, (float)altura };
    DrawTexturePro(f.textura, origem, destino, (Vector2){ 0, 0 }, 0, WHITE);
}

void descarregar_fundo(Fundo f) {
    if (f.textura.id != 0) UnloadTexture(f.textura);
    if (f.animacao.data != NULL) UnloadImage(f.animacao);
}
