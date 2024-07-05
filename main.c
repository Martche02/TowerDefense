#include "raylib.h"
#include <string.h>
#include <stdio.h>

#define LARGURA 1200
#define ALTURA 600
#define QUAD_SIZE 20
#define MONSTROS 50
#define FRUTAS 50
#define PORTAIS 50
#define PAREDES 200

// Definição da estrutura Estado
struct Estado {
    int vidas;
    bool vidaJogador;
    int qtdMonstros;
    int qtdFrutinhas;
    int qtdPortais;
    int qtdParedes;
    Vector2 posMonstros[MONSTROS];
    Vector2 posFrutinhas[FRUTAS];
    Vector2 posPortais[PORTAIS];
    Vector2 posParedes[PAREDES];
    Vector2 posJogador;
    Vector2 posArmadilhas[FRUTAS];
    Vector2 posBase;
    bool vitoria;
    bool derrota;
    int tempo;
    Vector2 trilha[MONSTROS];
    int spawTimes[MONSTROS];
    int recursos;
};

// Função para verificar colisão
bool verificarColisao(Vector2 a, Vector2 b) {
    return (a.x == b.x && a.y == b.y);
}

// Função para carregar o mapa de um arquivo de texto
void carregarMapa(const char* filename, struct Estado* estado) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo de mapa");
        return;
    }

    char linha[61];
    int lin = 0;
    while (fgets(linha, sizeof(linha), file)) {
        for (int col = 0; col < 60; col++) {
            switch (linha[col]) {
                case 'J':
                    estado->posJogador = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case 'M':
                    estado->posMonstros[estado->qtdMonstros++] = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case 'R':
                    estado->posFrutinhas[estado->qtdFrutinhas++] = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case 'H':
                    estado->posPortais[estado->qtdPortais++] = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case 'S':
                    estado->posBase = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case 'W':
                    estado->posParedes[estado->qtdParedes++] = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case ' ':
                    // Espaço em branco - área de trânsito
                    break;
            }
        }
        lin++;
    }

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

    // Criar uma matriz temporária para representar o mapa
    char mapa[LARGURA / QUAD_SIZE][ALTURA / QUAD_SIZE] = {0};

    // Preencher a matriz temporária com os elementos do jogo
    for (int i = 0; i < estado->qtdParedes; i++) {
        Vector2 pos = estado->posParedes[i];
        mapa[(int)(pos.y / QUAD_SIZE)][(int)(pos.x / QUAD_SIZE)] = 'W';
    }
    for (int i = 0; i < estado->qtdMonstros; i++) {
        Vector2 pos = estado->posMonstros[i];
        mapa[(int)(pos.y / QUAD_SIZE)][(int)(pos.x / QUAD_SIZE)] = 'M';
    }
    for (int i = 0; i < estado->qtdFrutinhas; i++) {
        Vector2 pos = estado->posFrutinhas[i];
        mapa[(int)(pos.y / QUAD_SIZE)][(int)(pos.x / QUAD_SIZE)] = 'R';
    }
    for (int i = 0; i < estado->qtdPortais; i++) {
        Vector2 pos = estado->posPortais[i];
        mapa[(int)(pos.y / QUAD_SIZE)][(int)(pos.x / QUAD_SIZE)] = 'H';
    }
    for (int i = 0; i < FRUTAS; i++) {
        Vector2 pos = estado->posArmadilhas[i];
        if (pos.x != 0 || pos.y != 0) {
            mapa[(int)(pos.y / QUAD_SIZE)][(int)(pos.x / QUAD_SIZE)] = 'A';
        }
    }
    Vector2 pos = estado->posJogador;
    mapa[(int)(pos.y / QUAD_SIZE)][(int)(pos.x / QUAD_SIZE)] = 'J';
    pos = estado->posBase;
    mapa[(int)(pos.y / QUAD_SIZE)][(int)(pos.x / QUAD_SIZE)] = 'S';

    // Escrever a matriz no arquivo
    for (int i = 0; i < ALTURA / QUAD_SIZE; i++) {
        for (int j = 0; j < LARGURA / QUAD_SIZE; j++) {
            if (mapa[i][j] == 0) {
                fputc(' ', file);
            } else {
                fputc(mapa[i][j], file);
            }
        }
        fputc('\n', file);
    }

    fclose(file);
}

// Função para carregar o estado do jogo
void carregarEstado(const char* filename, struct Estado* estado) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo para carregar");
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

    // Limpar posições atuais
    memset(estado->posMonstros, 0, sizeof(estado->posMonstros));
    memset(estado->posFrutinhas, 0, sizeof(estado->posFrutinhas));
    memset(estado->posPortais, 0, sizeof(estado->posPortais));
    memset(estado->posParedes, 0, sizeof(estado->posParedes));
    memset(estado->posArmadilhas, 0, sizeof(estado->posArmadilhas));

    char linha[61];
    int lin = 0;
    while (fgets(linha, sizeof(linha), file)) {
        for (int col = 0; col < 60; col++) {
            switch (linha[col]) {
                case 'J':
                    estado->posJogador = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case 'M':
                    estado->posMonstros[estado->qtdMonstros++] = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case 'R':
                    estado->posFrutinhas[estado->qtdFrutinhas++] = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case 'H':
                    estado->posPortais[estado->qtdPortais++] = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case 'S':
                    estado->posBase = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case 'W':
                    estado->posParedes[estado->qtdParedes++] = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
                case 'A':
                    estado->posArmadilhas[estado->qtdArmadilhas++] = (Vector2){col * QUAD_SIZE, lin * QUAD_SIZE};
                    break;
            }
        }
        lin++;
    }

    fclose(file);
}

// Função para atualizar o estado do jogo
struct Estado atualizarEstado(char k, struct Estado estado) {
    estado.tempo++;
    Vector2 novaPosJogador = estado.posJogador;
    Vector2 novaPosMonstros[MONSTROS];

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
            for (int i = 0; i < FRUTAS; i++) {
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
        for (int j = 0; j < MONSTROS; j++) { // Considerando que a trilha tem até MONSTROS posições
            if (verificarColisao(estado.posMonstros[i], estado.trilha[j])) {
                if (j < MONSTROS-1) { // Se não estiver na última posição da trilha
                    novaPosMonstros[i] = estado.trilha[j + 1];
                } else {
                    novaPosMonstros[i] = estado.trilha[j]; // Manter na última posição
                }
                break;
            }
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
    for (int i = 0; i < FRUTAS; i++) {
        for (int j = 0; j < estado.qtdMonstros; j++) {
            if (verificarColisao(estado.posArmadilhas[i], novaPosMonstros[j])) {
                novaPosMonstros[j] = (Vector2){0, 0}; // Remover monstro
                estado.posArmadilhas[i] = (Vector2){0, 0}; // Remover armadilha
            }
        }
    }

    // Colisão monstro/torre
    for (int i = 0; i < estado.qtdMonstros; i++) {
        if (verificarColisao(novaPosMonstros[i], estado.posBase)) {
            novaPosMonstros[i] = (Vector2){0, 0}; // Remover monstro
            estado.vidas--;
        }
    }

    // Colisão jogador/parede
    for (int i = 0; i < PAREDES; i++) {
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
    for (int i = 0; i < MONSTROS; i++) {
        if (estado.tempo == estado.spawTimes[i]) {
            for (int j = 0; j < MONSTROS; j++) {
                if (estado.posMonstros[j].x == 0 && estado.posMonstros[j].y == 0) {
                    estado.posMonstros[j] = estado.trilha[0]; // Spawn no início da trilha
                    break;
                }
            }
        }
    }

    return estado;
}

int main(void) {
    // Inicializações
    InitWindow(LARGURA, ALTURA, "Tower Defense"); // Inicializa janela
    SetTargetFPS(60); // Ajusta a execução do jogo para 60 frames por segundo

    struct Estado estado = {0};
    // Inicialização dos valores de estado
    estado.vidas = 3;
    estado.vidaJogador = true;
    estado.qtdMonstros = 0;
    estado.qtdFrutinhas = 0;
    estado.qtdPortais = 0;
    estado.qtdParedes = 0;
    estado.vitoria = false;
    estado.derrota = false;
    estado.tempo = 0;
    estado.recursos = 0;

    carregarMapa("mapa1.txt", &estado);

    // Exemplo de trilha
    for (int i = 0; i < MONSTROS; i++) {
        estado.trilha[i] = (Vector2){i * QUAD_SIZE, ALTURA / 2};
    }

    // Exemplo de tempos de spawn
    for (int i = 0; i < 10; i++) {
        estado.spawTimes[i] = i * 60; // Spawna um monstro a cada 60 frames
    }

    // Loop principal do jogo
    while (!WindowShouldClose()) { // Detecta fechamento da janela ou tecla ESC
        // Trata entrada do usuário e atualiza estado do jogo
        char k = GetCharPressed();
        estado = atualizarEstado(k, estado);

        // Salvar estado quando a tecla 'S' é pressionada
        if (IsKeyPressed(KEY_S)) {
            salvarEstado("savegame.txt", &estado);
        }

        // Carregar estado quando a tecla 'L' é pressionada
        if (IsKeyPressed(KEY_L)) {
            carregarEstado("savegame.txt", &estado);
        }

        // Atualiza a representação visual a partir do estado do jogo
        BeginDrawing(); // Inicia o ambiente de desenho na tela
        ClearBackground(RAYBLACK); // Limpa a tela e define cor de fundo

        // Desenha o jogador
        DrawRectangleV(estado.posJogador, (Vector2){QUAD_SIZE, QUAD_SIZE}, BLUE);

        // Desenha a base
        DrawRectangleV(estado.posBase, (Vector2){QUAD_SIZE, QUAD_SIZE}, DARKGRAY);

        // Desenha os monstros
        for (int i = 0; i < estado.qtdMonstros; i++) {
            DrawRectangleV(estado.posMonstros[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, RED);
        }

        // Desenha as frutinhas
        for (int i = 0; i < estado.qtdFrutinhas; i++) {
            DrawRectangleV(estado.posFrutinhas[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, GREEN);
        }

        // Desenha os portais
        for (int i = 0; i < estado.qtdPortais; i++) {
            DrawRectangleV(estado.posPortais[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, PURPLE);
        }

        // Desenha as paredes
        for (int i = 0; i < estado.qtdParedes; i++) {
            DrawRectangleV(estado.posParedes[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, BROWN);
        }

        // Desenha as armadilhas
        for (int i = 0; i < FRUTAS; i++) {
            if (estado.posArmadilhas[i].x != 0 || estado.posArmadilhas[i].y != 0) {
                DrawRectangleV(estado.posArmadilhas[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, BLACK);
            }
        }

        EndDrawing(); // Finaliza o ambiente de desenho na tela
    }

    CloseWindow(); // Fecha a janela e o contexto OpenGL
    return 0;
}
