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
#define MAX_INIMIGOS 50
#define MAX_RECURSOS 50
#define MAX_PAREDES 1000
#define MAX_PORTAIS 100
#define ULTIMAFASE 4
#define VELOCIDADE 1 //movimentos por segundo
#define MAX_TEXTURAS 15
#define MAX_MUSICAS 5
#define MENU_DERROTA 3
#define MENU_PAUSE 5
#define MENU_INICIO 3

#include "header.h"
#include "funcoes.h"

int main() {

    int ultimasteclas[10] = {0};
    int selecionado = 0;
    int musicaAtual;
    ESTADO estado = {0};
    Texture2D texturas[MAX_TEXTURAS];
    Music playlist[MAX_MUSICAS];
    inicializa(&estado, texturas, playlist);

    // Loop principal do jogo
    while (!WindowShouldClose()) {

        selecionaMusica(&estado, &musicaAtual);

        // Toca a música
        PlayMusicStream(playlist[musicaAtual]);

        UpdateMusicStream(playlist[musicaAtual]);

        if (estado.menu == 7)estado = atualizarEstado(estado, ultimasteclas);//se não está pausado, atualiza estads

        BeginDrawing();

        verificaEstado(&estado, &selecionado, playlist, musicaAtual, texturas);//faz as checagens e direcionamentos
        if (estado.vitoria) vitoria(&estado, texturas, playlist);
        if (estado.derrota) estado.menu = 8;

        EndDrawing();

    }

    // Fecha o dispositivo de áudio
    CloseAudioDevice();

    for (int i = 0; i < MAX_TEXTURAS; i++)UnloadTexture(texturas[i]);

    for (int i = 0; i < MAX_MUSICAS; i++)UnloadMusicStream(playlist[i]);

    CloseWindow();
    return 0;
}
