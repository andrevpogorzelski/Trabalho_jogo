# Futebol

Jogo de futebol 2D para 2 jogadores, feito em C com raylib.

## Como jogar
1. Baixe o `Futebol-Windows.zip` na aba **Releases**.
2. **Extraia o ZIP** (não rode de dentro dele) e abra `futebol.exe`.
3. Se o Windows avisar "Windows protegeu o computador": clique em *Mais informações* e depois em *Executar assim mesmo*.

## Controles
| | Jogador 1 | Jogador 2 |
|---|---|---|
| Andar | A / D | Setas esquerda / direita |
| Pular | W | Seta para cima |
| Chutar | E (segure para carregar, solte para chutar) | Shift direito |
| Ângulo do chute | W / S enquanto segura | Seta cima / baixo enquanto segura |

## Compilar (para quem quiser mexer no código)
1. Instale o raylib para Windows (inclui o compilador): https://www.raylib.com
2. Na pasta do projeto, rode:
   gcc main.c gravidade.c fundo.c menu.c -o futebol.exe -IC:/raylib/raylib/src -LC:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
