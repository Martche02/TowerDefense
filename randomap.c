#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"

#define LARGURA 1200
#define ALTURA 600
#define QUAD_SIZE 20
#define MAP_WIDTH (LARGURA / QUAD_SIZE)
#define MAP_HEIGHT (ALTURA / QUAD_SIZE)
#define MAX_MONSTROS 50
#define MAX_FRUTAS 50
#define MAX_PAREDES 200
#define MAX_PORTAIS 20

typedef struct {
    Vector2 torre;
    Vector2 trilha[MAP_WIDTH * MAP_HEIGHT];
    int comprimentoTrilha;
    Vector2 muros[MAX_PAREDES];
    int qtdMuros;
    Vector2 portais[MAX_PORTAIS];
    int qtdPortais;
    Vector2 monstros[MAX_MONSTROS];
    int qtdMonstros;
    Vector2 frutinhas[MAX_FRUTAS];
    int qtdFrutas;
    int spawTimes[MAX_MONSTROS];
    Vector2 spawnJogador;
} Mapa;

void inicializarMapa(Mapa *mapa) {
    mapa->comprimentoTrilha = 0;
    mapa->qtdMuros = 0;
    mapa->qtdPortais = 0;
    mapa->qtdMonstros = 0;
    mapa->qtdFrutas = 0;
}

void gerarTrilha(Mapa *mapa) {
    int x = rand() % MAP_WIDTH;
    int y = rand() % MAP_HEIGHT;
    mapa->trilha[mapa->comprimentoTrilha++] = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};

    while (x != MAP_WIDTH / 2 || y != MAP_HEIGHT / 2) {
        if (rand() % 2) {
            x += (x < MAP_WIDTH / 2) ? 1 : -1;
        } else {
            y += (y < MAP_HEIGHT / 2) ? 1 : -1;
        }
        mapa->trilha[mapa->comprimentoTrilha++] = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};
    }

    mapa->torre = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};
}

void adicionarMuro(Mapa *mapa, char tipo) {
    int x = rand() % (MAP_WIDTH - 4) + 2;
    int y = rand() % (MAP_HEIGHT - 4) + 2;
    int comprimento = rand() % 3 + 2;

    if (tipo == 'I') {
        for (int i = 0; i < comprimento; i++) {
            mapa->muros[mapa->qtdMuros++] = (Vector2){(x + i) * QUAD_SIZE, y * QUAD_SIZE};
        }
    } else if (tipo == 'U') {
        for (int i = 0; i < comprimento; i++) {
            mapa->muros[mapa->qtdMuros++] = (Vector2){(x + i) * QUAD_SIZE, y * QUAD_SIZE};
            mapa->muros[mapa->qtdMuros++] = (Vector2){(x + i) * QUAD_SIZE, (y + comprimento - 1) * QUAD_SIZE};
        }
        for (int i = 1; i < comprimento - 1; i++) {
            mapa->muros[mapa->qtdMuros++] = (Vector2){x * QUAD_SIZE, (y + i) * QUAD_SIZE};
        }
    } else if (tipo == 'O') {
        for (int i = 0; i < comprimento; i++) {
            mapa->muros[mapa->qtdMuros++] = (Vector2){(x + i) * QUAD_SIZE, y * QUAD_SIZE};
            mapa->muros[mapa->qtdMuros++] = (Vector2){(x + i) * QUAD_SIZE, (y + comprimento - 1) * QUAD_SIZE};
        }
        for (int i = 1; i < comprimento - 1; i++) {
            mapa->muros[mapa->qtdMuros++] = (Vector2){x * QUAD_SIZE, (y + i) * QUAD_SIZE};
            mapa->muros[mapa->qtdMuros++] = (Vector2){(x + comprimento - 1) * QUAD_SIZE, (y + i) * QUAD_SIZE};
        }
    }

    // Adicionar portais ao redor do muro
    for (int i = 0; i < 2; i++) {
        int px = x + (rand() % comprimento);
        int py = y + (rand() % comprimento);
        mapa->portais[mapa->qtdPortais++] = (Vector2){px * QUAD_SIZE, py * QUAD_SIZE};
    }
}

void gerarElementosAleatorios(Mapa *mapa) {
    mapa->qtdMonstros = rand() % MAX_MONSTROS;
    mapa->qtdFrutas = mapa->qtdMonstros;

    // Posicionar monstros
    for (int i = 0; i < mapa->qtdMonstros; i++) {
        int x = rand() % MAP_WIDTH;
        int y = rand() % MAP_HEIGHT;
        mapa->monstros[i] = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};
        mapa->spawTimes[i] = i * (60 / mapa->qtdMonstros); // Spawna ao longo de um segundo
    }

    // Posicionar frutinhas fora da trilha e dos muros
    for (int i = 0; i < mapa->qtdFrutas; i++) {
        int x, y;
        bool posicaoValida;
        do {
            posicaoValida = true;
            x = rand() % MAP_WIDTH;
            y = rand() % MAP_HEIGHT;
            Vector2 pos = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};
            for (int j = 0; j < mapa->comprimentoTrilha; j++) {
                if (verificarColisao(pos, mapa->trilha[j])) {
                    posicaoValida = false;
                    break;
                }
            }
            for (int j = 0; j < mapa->qtdMuros; j++) {
                if (verificarColisao(pos, mapa->muros[j])) {
                    posicaoValida = false;
                    break;
                }
            }
            for (int j = 0; j < mapa->qtdPortais; j++) {
                if (verificarColisao(pos, mapa->portais[j])) {
                    posicaoValida = false;
                    break;
                }
            }
        } while (!posicaoValida);
        mapa->frutinhas[i] = (Vector2){x * QUAD_SIZE, y * QUAD_SIZE};
    }
}

void salvarMapa(const char* filename, Mapa *mapa) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao abrir arquivo para salvar");
        return;
    }

    // Salvar metadados
    fprintf(file, "torre:%d,%d\n", (int)mapa->torre.x, (int)mapa->torre.y);
    fprintf(file, "spawnJogador:%d,%d\n", (int)mapa->spawnJogador.x, (int)mapa->spawnJogador.y);
    fprintf(file, "qtdMonstros:%d\n", mapa->qtdMonstros);
    fprintf(file, "qtdFrutas:%d\n", mapa->qtdFrutas);

    // Salvar trilha
    fprintf(file, "trilha:");
    for (int i = 0; i < mapa->comprimentoTrilha; i++) {
        fprintf(file, "%d,%d;", (int)mapa->trilha[i].x, (int)mapa->trilha[i].y);
    }
    fprintf(file, "\n");

    // Salvar muros
    fprintf(file, "muros:");
    for (int i = 0; i < mapa->qtdMuros; i++) {
        fprintf(file, "%d,%d;", (int)mapa->muros[i].x, (int)mapa->muros[i].y);
    }
    fprintf(file, "\n");

    // Salvar portais
    fprintf(file, "portais:");
    for (int i = 0; i < mapa->qtdPortais; i++) {
        fprintf(file, "%d,%d;", (int)mapa->portais[i].x, (int)mapa->portais[i].y);
    }
    fprintf(file, "\n");

    // Salvar monstros
    fprintf(file, "monstros:");
    for (int i = 0; i < mapa->qtdMonstros; i++) {
        fprintf(file, "%d,%d;", (int)mapa->monstros[i].x, (int)mapa->monstros[i].y);
    }
    fprintf(file, "\n");

    // Salvar frutinhas
    fprintf(file, "frutinhas:");
    for (int i = 0; i < mapa->qtdFrutas; i++) {
        fprintf(file, "%d,%d;", (int)mapa->frutinhas[i].x, (int)mapa->frutinhas[i].y);
    }
    fprintf(file, "\n");

    // Salvar tempos de spawn
    fprintf(file, "spawTimes:");
    for (int i = 0; i < mapa->qtdMonstros; i++) {
        fprintf(file, "%d;", mapa->spawTimes[i]);
    }
    fprintf(file, "\n");

    fclose(file);
}

int main(void) {
    srand(time(NULL));

    Mapa mapa;
    inicializarMapa(&mapa);
    
    gerarTrilha(&mapa);

    // Adicionar muros e portais
    for (int i = 0; i < 5; i++) {
        adicionarMuro(&mapa, "IOU"[rand() % 3]);
    }

    gerarElementosAleatorios(&mapa);

    // Define a posição de spawn do jogador aleatoriamente fora da trilha e dos muros
    do {
        mapa.spawnJogador = (Vector2){(rand() % MAP_WIDTH) * QUAD_SIZE, (rand() % MAP_HEIGHT) * QUAD_SIZE};
    } while (verificarColisao(mapa.spawnJogador, mapa.torre) || verificarColisao(mapa.spawnJogador, mapa.trilha[0]));

    salvarMapa("mapa_aleatorio.txt", &mapa);

    return 0;
}
