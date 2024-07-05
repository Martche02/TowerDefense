#include "raylib.h"
#include <string.h>

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
    Vector2 posMonstros[MONSTROS]; // Exemplo de até 50 monstros
    Vector2 posFrutinhas[FRUTAS]; // Exemplo de até 50 frutinhas
    Vector2 posPortais[PORTAIS]; // Exemplo de até 200 portais
    Vector2 posParedes[PAREDES]; // Exemplo de até 50 paredes
    char mapa[LARGURA/QUAD_SIZE][ALTURA/QUAD_SIZE]; // Mapa matriz 60x30
    Vector2 posJogador;
    Vector2 posArmadilhas[FRUTAS]; // Exemplo de até 50 armadilhas
    bool vitoria;
    bool derrota;
    int tempo;
    Vector2 trilha[MONSTROS]; // Exemplo de trilha com até 50 posições
    int spawTimes[MONSTROS]; // Exemplo de até 50 tempos de spawn
    int recursos;
};

// Função para verificar colisão
bool verificarColisao(Vector2 a, Vector2 b) {
    return (a.x == b.x && a.y == b.y);
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
        if (verificarColisao(novaPosMonstros[i], (Vector2){LARGURA - QUAD_SIZE, ALTURA / 2})) {
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
    for (int i = 0; i < 2; i++) {
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
    char texto[50] = "Pressione uma seta"; // texto inicial

    // Inicializações
    InitWindow(LARGURA, ALTURA, "Teclas"); // Inicializa janela
    SetTargetFPS(60); // Ajusta a execução do jogo para 60 frames por segundo

    struct Estado estado;
    // Inicialização dos valores de estado
    estado.vidas = 3;
    estado.vidaJogador = true;
    estado.qtdMonstros = 5;
    estado.qtdFrutinhas = 3;
    estado.posJogador = (Vector2){LARGURA / 2, ALTURA / 2};
    estado.vitoria = false;
    estado.derrota = false;
    estado.tempo = 0;
    estado.recursos = 0;

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

        if (IsKeyPressed(KEY_RIGHT)) strcpy(texto, "Direita");
        if (IsKeyPressed(KEY_LEFT)) strcpy(texto, "Esquerda");
        if (IsKeyPressed(KEY_UP)) strcpy(texto, "Cima");
        if (IsKeyPressed(KEY_DOWN)) strcpy(texto, "Baixo");

        // Atualiza a representação visual a partir do estado do jogo
        BeginDrawing(); // Inicia o ambiente de desenho na tela
        ClearBackground(RAYWHITE); // Limpa a tela e define cor de fundo
        DrawText(texto, 300, 200, 40, RED); // Desenha um texto
        // Desenha o jogador
        DrawRectangleV(estado.posJogador, (Vector2){QUAD_SIZE, QUAD_SIZE}, BLUE);
        // Desenha os monstros
        for (int i = 0; i < estado.qtdMonstros; i++) {
            DrawRectangleV(estado.posMonstros[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, RED);
        }
        // Desenha as frutinhas
        for (int i = 0; i < estado.qtdFrutinhas; i++) {
            DrawRectangleV(estado.posFrutinhas[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, GREEN);
        }
        // Desenha as armadilhas
        for (int i = 0; i < 10; i++) {
            if (estado.posArmadilhas[i].x != 0 || estado.posArmadilhas[i].y != 0) {
                DrawRectangleV(estado.posArmadilhas[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, BLACK);
            }
        }
        // Desenha a torre
        DrawRectangle(LARGURA - QUAD_SIZE, ALTURA / 2, QUAD_SIZE, QUAD_SIZE, GRAY);
        EndDrawing(); // Finaliza o ambiente de desenho na tela
    }

    CloseWindow(); // Fecha a janela e o contexto OpenGL
    return 0;
}
