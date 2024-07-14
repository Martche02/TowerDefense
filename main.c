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
#define MAX_MONSTROS 50
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
void carregarEstado(const char* filename, struct Estado* estado) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo de estado");
        return;
    }

    // Ler metadados
    fscanf(file, "vidas:%d\n", &estado->vidas);
    fscanf(file, "vidaJogador:%d\n", &estado->vidaJogador);
    fscanf(file, "recursos:%d\n", &estado->recursos);
    fscanf(file, "tempo:%d\n", &estado->tempo);
    fscanf(file, "qtdMonstros:%d\n", &estado->qtdMonstros);
    fscanf(file, "qtdFrutinhas:%d\n", &estado->qtdFrutinhas);
    fscanf(file, "qtdPortais:%d\n", &estado->qtdPortais);
    fscanf(file, "qtdParedes:%d\n", &estado->qtdParedes);
    fscanf(file, "comprimentoTrilha:%d\n", &estado->comprimentoTrilha);

    // Limpar posições atuais
    memset(estado->posMonstros, 0, sizeof(estado->posMonstros));
    memset(estado->posFrutinhas, 0, sizeof(estado->posFrutinhas));
    memset(estado->posPortais, 0, sizeof(estado->posPortais));
    memset(estado->posParedes, 0, sizeof(estado->posParedes));
    memset(estado->posArmadilhas, 0, sizeof(estado->posArmadilhas));

    // Carregar trilha
    for (int i = 0; i < estado->comprimentoTrilha; i++) {
        fscanf(file, "%f,%f;", &estado->trilha[i].x, &estado->trilha[i].y);
    }

    // Carregar muros
    for (int i = 0; i < estado->qtdParedes; i++) {
        fscanf(file, "%f,%f;", &estado->posParedes[i].x, &estado->posParedes[i].y);
    }

    // Carregar portais
    for (int i = 0; i < estado->qtdPortais; i++) {
        fscanf(file, "%f,%f;", &estado->posPortais[i].x, &estado->posPortais[i].y);
    }

    // Carregar monstros
    for (int i = 0; i < estado->qtdMonstros; i++) {
        fscanf(file, "%f,%f;%d\n", &estado->posMonstros[i].x, &estado->posMonstros[i].y, &estado->indexMonstro[i]);
    }

    // Carregar frutinhas
    for (int i = 0; i < estado->qtdFrutinhas; i++) {
        fscanf(file, "%f,%f;", &estado->posFrutinhas[i].x, &estado->posFrutinhas[i].y);
    }

    // Carregar tempos de spawn
    for (int i = 0; i < estado->qtdMonstros; i++) {
        fscanf(file, "%d;", &estado->spawTimes[i]);
    }

    // Carregar posição do jogador e da base
    fscanf(file, "posJogador:%f,%f\n", &estado->posJogador.x, &estado->posJogador.y);
    fscanf(file, "posBase:%f,%f\n", &estado->posBase.x, &estado->posBase.y);

    fclose(file);
}

// Função para salvar o estado do jogo
void salvarEstado(const char* filename, struct Estado* estado) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao abrir arquivo para salvar");
        return;
    }

    // Salvar metadados
    fprintf(file, "vidas:%d\n", estado->vidas);
    fprintf(file, "vidaJogador:%d\n", estado->vidaJogador);
    fprintf(file, "recursos:%d\n", estado->recursos);
    fprintf(file, "tempo:%d\n", estado->tempo);
    fprintf(file, "qtdMonstros:%d\n", estado->qtdMonstros);
    fprintf(file, "qtdFrutinhas:%d\n", estado->qtdFrutinhas);
    fprintf(file, "qtdPortais:%d\n", estado->qtdPortais);
    fprintf(file, "qtdParedes:%d\n", estado->qtdParedes);
    fprintf(file, "comprimentoTrilha:%d\n", estado->comprimentoTrilha);

    // Salvar trilha
    for (int i = 0; i < estado->comprimentoTrilha; i++) {
        fprintf(file, "%f,%f;", estado->trilha[i].x, estado->trilha[i].y);
    }
    fprintf(file, "\n");

    // Salvar muros
    for (int i = 0; i < estado->qtdParedes; i++) {
        fprintf(file, "%f,%f;", estado->posParedes[i].x, estado->posParedes[i].y);
    }
    fprintf(file, "\n");

    // Salvar portais
    for (int i = 0; i < estado->qtdPortais; i++) {
        fprintf(file, "%f,%f;", estado->posPortais[i].x, estado->posPortais[i].y);
    }
    fprintf(file, "\n");

    // Salvar monstros
    for (int i = 0; i < estado->qtdMonstros; i++) {
        fprintf(file, "%f,%f;%d\n", estado->posMonstros[i].x, estado->posMonstros[i].y, estado->indexMonstro[i]);
    }
    fprintf(file, "\n");

    // Salvar frutinhas
    for (int i = 0; i < estado->qtdFrutinhas; i++) {
        fprintf(file, "%f,%f;", estado->posFrutinhas[i].x, estado->posFrutinhas[i].y);
    }
    fprintf(file, "\n");

    // Salvar tempos de spawn
    for (int i = 0; i < estado->qtdMonstros; i++) {
        fprintf(file, "%d;", estado->spawTimes[i]);
    }
    fprintf(file, "\n");

    // Salvar posição do jogador e da base
    fprintf(file, "posJogador:%f,%f\n", estado->posJogador.x, estado->posJogador.y);
    fprintf(file, "posBase:%f,%f\n", estado->posBase.x, estado->posBase.y);

    fclose(file);
}

// Função para atualizar o estado do jogo
struct Estado atualizarEstado(char k, struct Estado estado) {
    estado.tempo++;
    Vector2 novaPosJogador = estado.posJogador;
    Vector2 novaPosMonstros[MAX_MONSTROS];

    switch (k) {
        case 'a':
        case 'A':
        case KEY_LEFT:
            novaPosJogador.x -= QUAD_SIZE;
            break;
        case 'd':
        case 'D':
        case KEY_RIGHT:
            novaPosJogador.x += QUAD_SIZE;
            break;
        case 'w':
        case 'W':
        case KEY_UP:
            novaPosJogador.y -= QUAD_SIZE;
            break;
        case 's':
        case 'S':
        case KEY_DOWN:
            novaPosJogador.y += QUAD_SIZE;
            break;
        case 'g':
        case 'G':
            // Colocar armadilha
            for (int i = 0; i < MAX_FRUTAS; i++) {
                if (estado.posArmadilhas[i].x == 0 && estado.posArmadilhas[i].y == 0) {
                    estado.posArmadilhas[i] = estado.posJogador;
                    break;
                }
            }
            break;
    }

    // Verificar colisões
    for (int i = 0; i < estado.qtdMonstros; i++) {
        // Atualizar posição dos monstros
        if (estado.tempo % 60 == 0) { // Mover monstros a cada 60 quadros
            int currentIndex = estado.indexMonstro[i];
            if (currentIndex < estado.comprimentoTrilha - 1) {
                estado.indexMonstro[i]++;
                novaPosMonstros[i] = estado.trilha[estado.indexMonstro[i]];
            } else {
                novaPosMonstros[i] = estado.trilha[currentIndex];
            }
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
            if (estado.posArmadilhas[i].x != -1 && estado.posArmadilhas[i].y != -1) {
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

    // Colisão jogador/parede
    for (int i = 0; i < estado.qtdParedes; i++) {
        if (verificarColisao(novaPosJogador, estado.posParedes[i])) {
            novaPosJogador = estado.posJogador; // Reverter posição
        }
    }

    // Colisão jogador/portal
    for (int i = 0; i < estado.qtdPortais; i++) {
        if (verificarColisao(novaPosJogador, estado.posPortais[i])) {
            switch (k) {
                case 'a':
                case 'A':
                case KEY_LEFT:
                    novaPosJogador.x -= QUAD_SIZE;
                    break;
                case 'd':
                case 'D':
                case KEY_RIGHT:
                    novaPosJogador.x += QUAD_SIZE;
                    break;
                case 'w':
                case 'W':
                case KEY_UP:
                    novaPosJogador.y -= QUAD_SIZE;
                    break;
                case 's':
                case 'S':
                case KEY_DOWN:
                    novaPosJogador.y += QUAD_SIZE;
                    break;
            }
        }
    }

    // Atualizar posição do jogador
    estado.posJogador = novaPosJogador;
    // Atualizar posição dos monstros
    for (int i = 0; i < estado.qtdMonstros; i++) {
        estado.posMonstros[i] = novaPosMonstros[i];
    }

    // Spawn de monstros
    if (estado.tempo % 180 == 0) { // Spawn de monstros a cada 180 quadros
        for (int j = 0; j < MAX_MONSTROS; j++) {
            if (estado.posMonstros[j].x == -1 && estado.posMonstros[j].y == -1) {
                estado.posMonstros[j] = estado.trilha[0]; // Spawn no início da trilha
                estado.indexMonstro[j] = 0; // Iniciar no primeiro índice da trilha
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
        int comprimento = rand() % 3 + 2;

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
    estado->qtdMonstros = rand() % MAX_MONSTROS;
    for (int i = 0; i < estado->qtdMonstros; i++) {
        estado->posMonstros[i] = (Vector2){-1, -1};
        estado->spawTimes[i] = i * 180; // Spawna a cada 180 quadros
    }

    // Gerar frutinhas
    estado->qtdFrutinhas = estado->qtdMonstros;
    for (int i = 0; i < estado->qtdFrutinhas; i++) {
        int x, y;
        bool posicaoValida;
        posicaoValida = true;
        x = rand() % MAP_WIDTH;
        y = rand() % MAP_HEIGHT;
        Vector2 pos = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};
        do {
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
        estado->posFrutinhas[i] = pos;
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
    printf("Digite 1 para carregar um estado salvo, 2 para gerar um novo mapa aleatório: ");
    int escolha;
    scanf("%d", &escolha);

    if (escolha == 1) {
        carregarEstado("savegame.txt", &estado);
    } else if (escolha == 2) {
        gerarMapaAleatorio(&estado);
        salvarEstado("savegame.txt", &estado); // Salva o estado inicial gerado
    }

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        char k = GetCharPressed();
        estado = atualizarEstado(k, estado);

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

        // Desenhar elementos do jogo
        DrawRectangleV(estado.posJogador, (Vector2){QUAD_SIZE, QUAD_SIZE}, BLUE);
        DrawRectangleV(estado.posBase, (Vector2){QUAD_SIZE, QUAD_SIZE}, DARKGRAY);

        for (int i = 0; i < estado.qtdMonstros; i++) {
            if (estado.posMonstros[i].x != -1 && estado.posMonstros[i].y != -1) {
                DrawRectangleV(estado.posMonstros[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, RED);
            }
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
            if (estado.posArmadilhas[i].x != -1 && estado.posArmadilhas[i].y != -1) {
                DrawRectangleV(estado.posArmadilhas[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, YELLOW);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
