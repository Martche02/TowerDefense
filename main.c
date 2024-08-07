#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LARGURA 1200
#define ALTURA 600
#define QUAD_SIZE 20
#define MAP_WIDTH (LARGURA / QUAD_SIZE)
#define MAP_HEIGHT (ALTURA / QUAD_SIZE)
#define MAX_MONSTROS 7
#define MAX_FRUTAS 50
#define MAX_PAREDES 200
#define MAX_PORTAIS 20

// Definição da estrutura Estado
struct Estado {
    int vidas;
    bool vidaJogador;
    int qtdMonstros;
    int qtdFrutinhas;
    int qtdPortais;
    int qtdParedes;
    Vector2 posMonstros[MAX_MONSTROS];
    int indexMonstro[MAX_MONSTROS];  // Índice atual na trilha
    Vector2 posFrutinhas[MAX_FRUTAS];
    Vector2 posPortais[MAX_PORTAIS];
    Vector2 posParedes[MAX_PAREDES];
    Vector2 posJogador;
    Vector2 posArmadilhas[MAX_FRUTAS];
    Vector2 posBase;
    bool vitoria;
    bool derrota;
    int tempo;
    Vector2 trilha[MAP_WIDTH * MAP_HEIGHT];
    int comprimentoTrilha;
    int spawTimes[MAX_MONSTROS];
    int recursos;
};

// Função para verificar colisão
bool verificarColisao(Vector2 a, Vector2 b) {
    return (a.x == b.x && a.y == b.y);
}

// Função para carregar o estado do jogo
void carregarEstado(char* filename, struct Estado* estado) {
    FILE* file = fopen(filename, "r+b");
    if (!file) {
        perror("Erro ao abrir arquivo para carregar");
        return;
    }
    fread(estado, sizeof(struct Estado), 1, file);
    fclose(file);
}
// Função para salvar o estado do jogo
void salvarEstado(char* filename, struct Estado* estado) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Erro ao abrir arquivo para salvar");
        return;
    }
    fwrite(estado, sizeof(struct Estado), 1, file);
    fclose(file);
}


int getK()
{
    int k = 0;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) k = 1;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) k = 2;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) k = 3;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) k = 4;
    if (IsKeyDown(KEY_G)) k = 5;

    return k;
}

// Função para atualizar o estado do jogo
struct Estado atualizarEstado(int k, struct Estado estado) {
    estado.tempo++;
    Vector2 novaPosJogador = estado.posJogador;
    Vector2 novaPosMonstros[MAX_MONSTROS];

    switch(k)
    {
        case 87:
        case 265:
            novaPosJogador.y -= QUAD_SIZE; // Mover para cima
            break;

        case 83:
        case 264:
            novaPosJogador.y += QUAD_SIZE; // Mover para baixo
            break;

        case 65:
        case 263:
            novaPosJogador.x -= QUAD_SIZE; // Mover para a esquerda
            break;

        case 68:
        case 262:
            novaPosJogador.x += QUAD_SIZE; // Mover para a direita
            break;

        case 71:
            // Colocar armadilha
            if (estado.recursos > 0) {
                for (int i = 0; i < MAX_FRUTAS; i++) {
                    if (estado.posArmadilhas[i].x == 0 && estado.posArmadilhas[i].y == 0) {
                        estado.posArmadilhas[i] = estado.posJogador;
                        estado.recursos--;
                        break;
                    }
                }
            }
            break;

    }



    // Atualizar posição dos monstros
    for (int i = 0; i < estado.qtdMonstros; i++) {
        if (estado.indexMonstro[i] < estado.comprimentoTrilha - 1) {
            if (estado.tempo % 60 == 0) {
                estado.indexMonstro[i]++;
            }
            novaPosMonstros[i] = estado.trilha[estado.indexMonstro[i]];
        } else {
            novaPosMonstros[i] = estado.posMonstros[i];
        }
    }

    // Colisão jogador/recurso
    for (int i = 0; i < estado.qtdFrutinhas; i++) {
        if (verificarColisao(novaPosJogador, estado.posFrutinhas[i])) {
            estado.recursos++;
            estado.posFrutinhas[i] = (Vector2){0, 0}; // Remover frutinha
        }
    }

    // Colisão armadilha/monstro
    for (int j = 0; j < estado.qtdMonstros; j++) {
        for (int i = 0; i < MAX_FRUTAS; i++) {
            if (estado.posArmadilhas[i].x != 0 && estado.posArmadilhas[i].y != 0) {
                if (verificarColisao(estado.posArmadilhas[i], estado.posMonstros[j])) {
                    // Monstro e armadilha desaparecem
                    for (int k = j; k < estado.qtdMonstros - 1; k++) {
                        estado.posMonstros[k] = estado.posMonstros[k + 1];
                        estado.indexMonstro[k] = estado.indexMonstro[k + 1];
                    }
                    estado.qtdMonstros--;
                    estado.posArmadilhas[i] = (Vector2){-1, -1};
                    j--; // Ajustar índice após a remoção
                    break;
                }
            }
        }
    }

    // Colisão monstro/torre
    for (int i = 0; i < estado.qtdMonstros; i++) {
        if (verificarColisao(estado.posMonstros[i], estado.posBase)) {
            for (int j = i; j < estado.qtdMonstros - 1; j++) {
                estado.posMonstros[j] = estado.posMonstros[j + 1];
                estado.indexMonstro[j] = estado.indexMonstro[j + 1];
            }
            estado.qtdMonstros--;
            estado.vidas--;
            i--; // Ajustar índice após a remoção
        }
    }
    if (estado.vidas <= 0)
        estado.derrota = true;
    if (estado.qtdMonstros == 0 && estado.tempo>180)
        estado.vitoria = true;
    // Colisão jogador/parede
    for (int i = 0; i < estado.qtdParedes; i++) {
        if (verificarColisao(novaPosJogador, estado.posParedes[i])) {
            novaPosJogador = estado.posJogador; // Reverter posição
        }
    }

    // Colisão jogador/portal
    for (int i = 0; i < estado.qtdPortais; i++) {
        if (verificarColisao(novaPosJogador, estado.posPortais[i])) {
            // Verifica se as teclas WASD estão pressionadas
    if (IsKeyDown(KEY_W)) novaPosJogador.y -= QUAD_SIZE; // Mover para cima
    if (IsKeyDown(KEY_S)) novaPosJogador.y += QUAD_SIZE; // Mover para baixo
    if (IsKeyDown(KEY_A)) novaPosJogador.x -= QUAD_SIZE; // Mover para a esquerda
    if (IsKeyDown(KEY_D)) novaPosJogador.x += QUAD_SIZE; // Mover para a direita

    // Verifica se as setas direcionais estão pressionadas
    if (IsKeyDown(KEY_UP)) novaPosJogador.y -= QUAD_SIZE; // Mover para cima
    if (IsKeyDown(KEY_DOWN)) novaPosJogador.y += QUAD_SIZE; // Mover para baixo
    if (IsKeyDown(KEY_LEFT)) novaPosJogador.x -= QUAD_SIZE; // Mover para a esquerda
    if (IsKeyDown(KEY_RIGHT)) novaPosJogador.x += QUAD_SIZE; // Mover para a direita
        }
    }

    // Atualizar posição do jogador
    estado.posJogador = novaPosJogador;
    // Atualizar posição dos monstros
    for (int i = 0; i < estado.qtdMonstros; i++) {
        estado.posMonstros[i] = novaPosMonstros[i];
    }

    // Spawn de monstros
    if (estado.qtdMonstros < MAX_MONSTROS && estado.tempo % 180 == 0) { // Spawn de monstros a cada 180 quadros
        for (int j = 0; j < MAX_MONSTROS; j++) {
            if (estado.indexMonstro[j] == -1) {
                estado.posMonstros[j] = estado.trilha[0]; // Spawn no início da trilha
                estado.indexMonstro[j] = 0; // Iniciar no primeiro índice da trilha
                estado.qtdMonstros++;
                break;
            }
        }
    }

    return estado;
}

// Função para gerar um mapa aleatório
void gerarMapaAleatorio(struct Estado* estado) {
    // Inicializar valores de estado
    estado->vidas = 3;
    estado->vidaJogador = true;
    estado->qtdMonstros = 0;
    estado->qtdFrutinhas = 0;
    estado->qtdPortais = 0;
    estado->qtdParedes = 0;
    estado->vitoria = false;
    estado->derrota = false;
    estado->tempo = 0;
    estado->recursos = 0;
    estado->comprimentoTrilha = 0;

    // Inicializar vetores de posição com zeros
    memset(estado->trilha, 0, sizeof(estado->trilha));
    memset(estado->posParedes, 0, sizeof(estado->posParedes));
    memset(estado->posPortais, 0, sizeof(estado->posPortais));
    memset(estado->posMonstros, 0, sizeof(estado->posMonstros));
    memset(estado->indexMonstro, -1, sizeof(estado->indexMonstro));
    memset(estado->spawTimes, 0, sizeof(estado->spawTimes));
    memset(estado->posFrutinhas, 0, sizeof(estado->posFrutinhas));
    memset(&estado->posJogador, 0, sizeof(estado->posJogador));

    srand(time(NULL));

    // Gerar trilha
    int x = rand() % MAP_WIDTH;
    int y = rand() % MAP_HEIGHT;
    estado->trilha[estado->comprimentoTrilha++] = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};

    while (x != MAP_WIDTH / 2 || y != MAP_HEIGHT / 2) {
        if (rand() % 2) {
            x += (x < MAP_WIDTH / 2) ? 1 : -1;
        } else {
            y += (y < MAP_HEIGHT / 2) ? 1 : -1;
        }
        estado->trilha[estado->comprimentoTrilha++] = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};
    }

    estado->posBase = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};

    // Adicionar muros e portais
    for (int i = 0; i < 5; i++) {
        char tipoMuro = "IOU"[rand() % 3];
        int x = rand() % (MAP_WIDTH - 4) + 2;
        int y = rand() % (MAP_HEIGHT - 4) + 2;
        int comprimento = rand() % 6 + 2;

        if (tipoMuro == 'I') {
            for (int j = 0; j < comprimento; j++) {
                estado->posParedes[estado->qtdParedes++] = (Vector2){(x + j) * QUAD_SIZE, y * QUAD_SIZE};
            }
        } else if (tipoMuro == 'U') {
            for (int j = 0; j < comprimento; j++) {
                estado->posParedes[estado->qtdParedes++] = (Vector2){(x + j) * QUAD_SIZE, y * QUAD_SIZE};
                estado->posParedes[estado->qtdParedes++] = (Vector2){(x + j) * QUAD_SIZE, (y + comprimento - 1) * QUAD_SIZE};
            }
            for (int j = 1; j < comprimento - 1; j++) {
                estado->posParedes[estado->qtdParedes++] = (Vector2){x * QUAD_SIZE, (y + j) * QUAD_SIZE};
            }
        } else if (tipoMuro == 'O') {
            for (int j = 0; j < comprimento; j++) {
                estado->posParedes[estado->qtdParedes++] = (Vector2){(x + j) * QUAD_SIZE, y * QUAD_SIZE};
                estado->posParedes[estado->qtdParedes++] = (Vector2){(x + j) * QUAD_SIZE, (y + comprimento - 1) * QUAD_SIZE};
            }
            for (int j = 1; j < comprimento - 1; j++) {
                estado->posParedes[estado->qtdParedes++] = (Vector2){x * QUAD_SIZE, (y + j) * QUAD_SIZE};
                estado->posParedes[estado->qtdParedes++] = (Vector2){(x + comprimento - 1) * QUAD_SIZE, (y + j) * QUAD_SIZE};
            }
        }

        // Adicionar portais ao redor do muro
        for (int j = 0; j < 2; j++) {
            int px = x + (rand() % comprimento);
            int py = y + (rand() % comprimento);
            estado->posPortais[estado->qtdPortais++] = (Vector2){px * QUAD_SIZE, py * QUAD_SIZE};
        }
    }

    // Gerar monstros
    for (int i = 0; i < MAX_MONSTROS; i++) {
        estado->indexMonstro[i] = -1; // Iniciar como inativo
    }

    // Gerar frutinhas
    estado->qtdFrutinhas = MAX_MONSTROS; // Defina um número fixo de frutinhas
    for (int i = 0; i < estado->qtdFrutinhas; i++) {
        int x, y;
        bool posicaoValida;
        do {
            posicaoValida = true;
            x = rand() % MAP_WIDTH;
            y = rand() % MAP_HEIGHT;
            Vector2 pos = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};
            for (int j = 0; j < estado->comprimentoTrilha; j++) {
                if (verificarColisao(pos, estado->trilha[j])) {
                    posicaoValida = false;
                    break;
                }
            }
            for (int j = 0; j < estado->qtdParedes; j++) {
                if (verificarColisao(pos, estado->posParedes[j])) {
                    posicaoValida = false;
                    break;
                }
            }
            for (int j = 0; j < estado->qtdPortais; j++) {
                if (verificarColisao(pos, estado->posPortais[j])) {
                    posicaoValida = false;
                    break;
                }
            }
        } while (!posicaoValida);
        estado->posFrutinhas[i] = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};
    }

    // Definir a posição de spawn do jogador aleatoriamente fora da trilha e dos muros
    do {
        estado->posJogador = (Vector2){(rand() % MAP_WIDTH) * QUAD_SIZE, (rand() % MAP_HEIGHT) * QUAD_SIZE};
    } while (verificarColisao(estado->posJogador, estado->posBase) || verificarColisao(estado->posJogador, estado->trilha[0]));
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Tower Defense");
    SetTargetFPS(60);

    struct Estado estado = {0};

    // Escolher entre carregar um estado salvo ou gerar um novo mapa aleatório
    int escolha = 2;

    if (escolha == 1) {
        carregarEstado("savegame.txt", &estado);
    } else if (escolha == 2) {
        gerarMapaAleatorio(&estado);
        salvarEstado("savegame.txt", &estado); // Salva o estado inicial gerado
    }

    // Loop principal do jogo
    while (!WindowShouldClose()) {

        int k = GetKeyPressed();
        estado = atualizarEstado(k, estado);

        char text[64];
       sprintf(text, "Armadilhas: %d  Vidas da torre: %d Monstros restantes: %d",
               estado.recursos, estado.vidas, estado.qtdMonstros);

        // Salvar estado quando a tecla 'S' é pressionada
        if (IsKeyPressed(KEY_K)) {
            salvarEstado("savegame.txt", &estado);
        }

        // Carregar estado quando a tecla 'L' é pressionada
        if (IsKeyPressed(KEY_L)) {
            carregarEstado("savegame.txt", &estado);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawText(text, 10, 10, 20, WHITE);

        // Desenhar elementos do jogo
        for (int i = 0; i < estado.comprimentoTrilha; i++) {
            DrawRectangleV(estado.trilha[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, PINK);
        }

        DrawRectangleV(estado.posJogador, (Vector2){QUAD_SIZE, QUAD_SIZE}, BLUE);
        DrawRectangleV(estado.posBase, (Vector2){QUAD_SIZE, QUAD_SIZE}, DARKGRAY);

        for (int i = 0; i < estado.qtdMonstros; i++) {
            DrawRectangleV(estado.posMonstros[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, RED);
        }

        for (int i = 0; i < estado.qtdFrutinhas; i++) {
            DrawRectangleV(estado.posFrutinhas[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, GREEN);
        }

        for (int i = 0; i < estado.qtdPortais; i++) {
            DrawRectangleV(estado.posPortais[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, PURPLE);
        }

        for (int i = 0; i < estado.qtdParedes; i++) {
            DrawRectangleV(estado.posParedes[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, BROWN);
        }

        for (int i = 0; i < MAX_FRUTAS; i++) {
            if (estado.posArmadilhas[i].x != 0 && estado.posArmadilhas[i].y != 0) {
                DrawRectangleV(estado.posArmadilhas[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, YELLOW);
            }
        }



        if (estado.vitoria)
            printf("vitoria");
        if (estado.derrota)
            printf("derrota");
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
