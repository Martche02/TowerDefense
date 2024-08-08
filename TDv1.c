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

// Defini��o da estrutura Estado
typedef struct Estado {
    int vidas;
    int vidaJogador;
    int qtdMonstros;
    int qtdFrutinhas;
    int qtdPortais;
    int qtdParedes;
    Vector2 posMonstros[MAX_MONSTROS];
    int indexMonstro[MAX_MONSTROS];  // �ndice atual na trilha
    Vector2 posFrutinhas[MAX_FRUTAS];
    Vector2 posPortais[MAX_PORTAIS];
    Vector2 posParedes[MAX_PAREDES];
    Vector2 posJogador;
    Vector2 posArmadilhas[MAX_FRUTAS];
    Vector2 posBase;
    int vitoria;
    int derrota;
    int tempo;
    Vector2 trilha[MAP_WIDTH * MAP_HEIGHT];
    int comprimentoTrilha;
    int spawTimes[MAX_MONSTROS];
    int recursos;
    int nivelatual;
}ESTADO;

// Fun��o para verificar colis�o
int verificarColisao(Vector2 a, Vector2 b) {
    return (a.x == b.x && a.y == b.y);
}

// Fun��o para carregar o estado do jogo
void carregarEstado(char* filename, ESTADO* estado) {
    FILE* file = fopen(filename, "r+b");
    if (!file) {
        perror("Erro ao abrir arquivo para carregar");
        return;
    }
    fread(estado, sizeof(ESTADO), 1, file);
    fclose(file);
}

// Fun��o para salvar o estado do jogo
void salvarEstado(char* filename, ESTADO* estado) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Erro ao abrir arquivo para salvar");
        return;
    }
    fwrite(estado, sizeof(ESTADO), 1, file);
    fclose(file);
}

void getAcao(int k, ESTADO *estado, Vector2 *novaPosJogador)
{
    switch(k)
    {
        case 87:
        case 265:
            novaPosJogador->y -= QUAD_SIZE; // Mover para cima
            break;

        case 83:
        case 264:
            novaPosJogador->y += QUAD_SIZE; // Mover para baixo
            break;

        case 65:
        case 263:
            novaPosJogador->x -= QUAD_SIZE; // Mover para a esquerda
            break;

        case 68:
        case 262:
            novaPosJogador->x += QUAD_SIZE; // Mover para a direita
            break;

        case 71:
            // Colocar armadilha
            if (estado->recursos > 0) {
                for (int i = 0; i < MAX_FRUTAS; i++) {
                    if (estado->posArmadilhas[i].x == 0 && estado->posArmadilhas[i].y == 0) {
                        estado->posArmadilhas[i] = estado->posJogador;
                        estado->recursos--;
                        break;
                    }
                }
            }
            break;
    }
}

void novasposMonstros(ESTADO *estado, Vector2 novaPosMonstros[])
{
// Atualizar posi��o dos monstros
    for (int i = 0; i < estado->qtdMonstros; i++) {
        if (estado->indexMonstro[i] < estado->comprimentoTrilha - 1) {
            if (estado->tempo % 60 == 0) {
                estado->indexMonstro[i]++;
            }
            novaPosMonstros[i] = estado->trilha[estado->indexMonstro[i]];
        } else {
            novaPosMonstros[i] = estado->posMonstros[i];
        }
    }
}

void colisaoJogadorRecurso(ESTADO *estado, Vector2 novaPosJogador)
{
// Colis�o jogador/recurso
    for (int i = 0; i < estado->qtdFrutinhas; i++) {
        if (verificarColisao(novaPosJogador, estado->posFrutinhas[i])) {
            estado->recursos++;
            estado->posFrutinhas[i] = (Vector2){-1, -1}; // Remover frutinha
        }
    }
}

void colisaoJogadorParede(ESTADO *estado, Vector2 *novaPosJogador)
{
// Colis�o jogador/parede
    for (int i = 0; i < estado->qtdParedes; i++) {
        if (verificarColisao(*novaPosJogador, estado->posParedes[i])) {
            *novaPosJogador = estado->posJogador; // Reverter posi��o
        }
    }
}

void colisaoJogadorPortal(int k, ESTADO *estado, Vector2 *novaPosJogador)
{
    // Colis�o jogador/portal
    for (int i = 0; i < estado->qtdPortais; i++) {
        if (verificarColisao(*novaPosJogador, estado->posPortais[i])) {
            // Verifica se as teclas WASD est�o pressionadas
            int pos;
            switch(k)
            {
                case 87:
                case 265:
                     // Mover para cima
                    pos = ALTURA;
                    for (int j = 0; j < estado->qtdPortais; j++) {
                            if(estado->posPortais[j].x == novaPosJogador->x && estado->posPortais[j].y < pos){
                                pos = estado->posPortais[j].y;
                            }
                    }
                    if(pos!=ALTURA){
                        novaPosJogador->y = pos-QUAD_SIZE;
                    } else{
                        novaPosJogador->y -= QUAD_SIZE;
                    }

                    break;

                case 83:
                case 264:
                   //pra baixo
                    pos = 0;
                    for (int j = 0; j < estado->qtdPortais; j++) {
                            if(estado->posPortais[j].x == novaPosJogador->x && estado->posPortais[j].y >pos){
                                pos = estado->posPortais[j].y;
                            }
                    }
                    if(pos!=0){
                        novaPosJogador->y = pos+QUAD_SIZE;
                    } else{
                        novaPosJogador->y += QUAD_SIZE;
                    }
                    break;

                case 65:
                case 263:
                     // Mover para esquerda
                    pos = LARGURA;
                    for (int j = 0; j < estado->qtdPortais; j++) {
                            if(estado->posPortais[j].y == novaPosJogador->y && estado->posPortais[j].x <pos){
                                pos = estado->posPortais[j].x;
                            }
                    }
                    if(pos!=LARGURA){
                        novaPosJogador->x = pos-QUAD_SIZE;
                    } else{
                        novaPosJogador->x -= QUAD_SIZE;
                    }

                    break;

                case 68:
                case 262:
                    pos = 0;
                    for (int j = 0; j < estado->qtdPortais; j++) {
                            if(estado->posPortais[j].y == novaPosJogador->y && estado->posPortais[j].x >pos){
                                pos = estado->posPortais[j].x;
                            }
                    }
                    if(pos!=0){
                        novaPosJogador->x = pos+QUAD_SIZE;
                    } else{
                        novaPosJogador->x += QUAD_SIZE;
                    }
                    break;

            }
        }
    }
}

// Fun��o para atualizar o estado do jogo
ESTADO atualizarEstado(ESTADO estado) {
    estado.tempo++;
    Vector2 novaPosJogador = estado.posJogador;
    Vector2 novaPosMonstros[MAX_MONSTROS];

    int k = GetKeyPressed();

    getAcao(k, &estado, &novaPosJogador);

    novasposMonstros(&estado, novaPosMonstros);

    colisaoJogadorRecurso(&estado, novaPosJogador);

    colisaoJogadorParede(&estado, &novaPosJogador);


    // Colis�o armadilha/monstro
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
                    j--; // Ajustar �ndice ap�s a remo��o
                    break;
                }
            }
        }
    }

    // Colis�o monstro/torre
    for (int i = 0; i < estado.qtdMonstros; i++) {
        if (verificarColisao(estado.posMonstros[i], estado.posBase)) {
            for (int j = i; j < estado.qtdMonstros - 1; j++) {
                estado.posMonstros[j] = estado.posMonstros[j + 1];
                estado.indexMonstro[j] = estado.indexMonstro[j + 1];
            }
            printf("matou");
            estado.qtdMonstros--;
            estado.vidas--;
            i--; // Ajustar �ndice ap�s a remo��o
        }
    }

    if (estado.vidas <= 0)
        estado.derrota = true;
    if (estado.qtdMonstros == 0 && estado.tempo>180)
        estado.vitoria = true;

    colisaoJogadorPortal(k, &estado, &novaPosJogador);

     //Colis�o jogador/monstro
    for (int i = 0; i < estado.qtdMonstros - 1; i++) {
        if (verificarColisao(estado.posMonstros[i], novaPosJogador))estado.derrota = true;
    }

    // Atualizar posi��o do jogador
    estado.posJogador = novaPosJogador;
    // Atualizar posi��o dos monstros
    for (int i = 0; i < estado.qtdMonstros; i++) {
        estado.posMonstros[i] = novaPosMonstros[i];
    }

    // Spawn de monstros
    if (estado.qtdMonstros < MAX_MONSTROS && estado.tempo % 180 == 0) { // Spawn de monstros a cada 180 quadros
        for (int j = 0; j < MAX_MONSTROS; j++) {
            if (estado.indexMonstro[j] == -1) {
                estado.posMonstros[j] = estado.trilha[0]; // Spawn no in�cio da trilha
                estado.indexMonstro[j] = 0; // Iniciar no primeiro �ndice da trilha
                estado.qtdMonstros++;
                break;
            }
        }
    }

    return estado;
}

void carregarMapaDeArquivo(ESTADO* estado, const char* caminhoArquivo) {
    FILE* arquivo = fopen(caminhoArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

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

    memset(estado->trilha, 0, sizeof(estado->trilha));
    memset(estado->posParedes, 0, sizeof(estado->posParedes));
    memset(estado->posPortais, 0, sizeof(estado->posPortais));
    memset(estado->posMonstros, 0, sizeof(estado->posMonstros));
    memset(estado->indexMonstro, -1, sizeof(estado->indexMonstro));
    memset(estado->spawTimes, 0, sizeof(estado->spawTimes));
    memset(estado->posFrutinhas, 0, sizeof(estado->posFrutinhas));
    memset(&estado->posJogador, 0, sizeof(estado->posJogador));

    char mapa[MAP_HEIGHT][MAP_WIDTH] = {0}; // Para armazenar o mapa temporariamente
    char linha[MAP_WIDTH + 2]; // +2 para o '\n' e '\0'
    int y = 0;

    while (fgets(linha, sizeof(linha), arquivo) && y < MAP_HEIGHT) {
        for (int x = 0; x < MAP_WIDTH && linha[x] != '\n'; x++) {
            if (linha[x] == ' ') {
                continue; // Ignorar espaços em branco
            }
            mapa[y][x] = linha[x]; // Armazenar o caractere no mapa
            Vector2 pos = {x * QUAD_SIZE, y * QUAD_SIZE};
            switch (linha[x]) {
                case 'J':
                    estado->posJogador = pos;
                    break;
                case 'M':
                    if (estado->qtdMonstros < MAX_MONSTROS) {
                        estado->posMonstros[estado->qtdMonstros++] = pos;
                    }
                    break;
                case 'R':
                    estado->posFrutinhas[estado->qtdFrutinhas++] = pos;
                    break;
                case 'H':
                    estado->posPortais[estado->qtdPortais++] = pos;
                    break;
                case 'W':
                    estado->posParedes[estado->qtdParedes++] = pos;
                    break;
                case 'S':
                    estado->posBase = pos;
                    break;
                case 'I':
                    estado->trilha[estado->comprimentoTrilha++] = pos;
                    break;
            }
        }
        y++;
    }

    fclose(arquivo);

    // Função auxiliar para verificar se a posição está dentro dos limites do mapa
    bool dentroDosLimites(int x, int y) {
        return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
    }

    // Função auxiliar para verificar se a posição já está na trilha
    bool jaNaTrilha(Vector2 pos, Vector2* trilha, int comprimentoTrilha) {
        for (int i = 0; i < comprimentoTrilha; i++) {
            if (trilha[i].x == pos.x && trilha[i].y == pos.y) {
                return true;
            }
        }
        return false;
    }

    // Encontrar e construir a trilha
    int dx[] = {0, 0, -1, 1}; // Direções de movimento: cima, baixo, esquerda, direita
    int dy[] = {-1, 1, 0, 0};

    Vector2 atual = estado->trilha[0];
    while (true) {
        bool encontrado = false;
        for (int i = 0; i < 4; i++) {
            int nx = atual.x / QUAD_SIZE + dx[i];
            int ny = atual.y / QUAD_SIZE + dy[i];
            if (dentroDosLimites(nx, ny)) {
                Vector2 proximo = {nx * QUAD_SIZE, ny * QUAD_SIZE};
                if (!jaNaTrilha(proximo, estado->trilha, estado->comprimentoTrilha)) {
                    if (mapa[ny][nx] == 'B') {
                        estado->trilha[estado->comprimentoTrilha++] = proximo;
                        encontrado = true;
                        break;
                    } else if (mapa[ny][nx] == '.') {
                        estado->trilha[estado->comprimentoTrilha++] = proximo;
                        atual = proximo;
                        encontrado = true;
                        break;
                    }
                }
            }
        }
        if (!encontrado) {
            break; // Se não encontrou nenhum próximo ponto na trilha, parar a busca
        }
    }
}

void carregaNivel(ESTADO* estado)
{
    char mapa[20];
    snprintf(mapa, sizeof(mapa), "Mapa%d.txt", estado->nivelatual);
    carregarMapaDeArquivo(estado, mapa);
}

int main() {
    InitWindow(LARGURA, ALTURA, "Tower Defense");
    SetTargetFPS(60);

    ESTADO estado = {0};

    // Escolher entre carregar um estado salvo ou gerar um novo mapa aleat�rio
    //abreMenu(&estado);

    //carregarMapaDeArquivo(&estado, "mapa.txt");
    estado.nivelatual = 1;
    carregaNivel(&estado);

    // Loop principal do jogo
    while (!WindowShouldClose()) {


        estado = atualizarEstado(estado);

        char text[100];
        sprintf(text, "Fase atual: 0    Armadilhas: %d    Vidas da torre: %d   Vidas do jogador: 1   Monstros restantes: %d",
               estado.recursos, estado.vidas, estado.qtdMonstros);

        // Salvar estado quando a tecla 'S' � pressionada
        if (IsKeyPressed(KEY_K)) {
            salvarEstado("savegame.txt", &estado);
        }

        // Carregar estado quando a tecla 'L' � pressionada
        if (IsKeyPressed(KEY_L)) {
            carregarEstado("savegame.txt", &estado);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Desenhar elementos do jogo
        //for (int i = 0; i < estado.comprimentoTrilha; i++) {
          //  DrawRectangleV(estado.trilha[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, PINK);}

        DrawRectangleV(estado.posJogador, (Vector2){QUAD_SIZE, QUAD_SIZE}, BLUE);
        DrawRectangleV(estado.posBase, (Vector2){QUAD_SIZE, QUAD_SIZE}, DARKGRAY);

        for (int i = 0; i < estado.qtdMonstros; i++) {
            DrawRectangleV(estado.posMonstros[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, RED);
        }

        for (int i = 0; i < estado.qtdFrutinhas; i++) {
            DrawRectangleV(estado.posFrutinhas[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, GREEN);
        }


        for (int i = 0; i < MAX_FRUTAS; i++) {
            if (estado.posArmadilhas[i].x != 0 && estado.posArmadilhas[i].y != 0) {
                DrawRectangleV(estado.posArmadilhas[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, YELLOW);
            }
        }

         for (int i = 0; i < estado.qtdPortais; i++) {
            DrawRectangleV(estado.posPortais[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, PURPLE);
        }

        for (int i = 0; i < estado.qtdParedes; i++) {
            DrawRectangleV(estado.posParedes[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, BROWN);
        }

        DrawText(text, 10, 3, 19, WHITE);

        if (estado.vitoria)
        {

            printf("vitoria");
            ClearBackground(BLACK);
            EndDrawing();
            estado.nivelatual ++;
            int e = estado.nivelatual;
            //estado = {0};
            memset(&estado, 0, sizeof(estado));
            estado.nivelatual = e;
            carregaNivel(&estado);
        }

        if (estado.derrota)
        {
           printf("derrota");
           ClearBackground(BLACK);
           EndDrawing();

            ESTADO estado = {0};

            // Escolher entre carregar um estado salvo ou gerar um novo mapa aleat�rio
            //abreMenu(&estado);

            //carregarMapaDeArquivo(&estado, "mapa.txt");

            carregaNivel(&estado);

        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
