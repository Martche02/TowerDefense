#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//tamanho da tela
#define LARGURA 1200
#define ALTURA 600
#define MAP_WIDTH (LARGURA / QUAD_SIZE)
#define MAP_HEIGHT (ALTURA / QUAD_SIZE)

//tamanho unidade
#define QUAD_SIZE 20

//limites
#define MAX_INIMIGOS 50
#define MAX_RECURSOS 50
#define MAX_PAREDES 1800
#define MAX_TUNEIS 100
#define MAX_TEXTURAS 15
#define MAX_MUSICAS 5
#define MAX_FASES 99

//numero de fases
#define ULTIMAFASE 6

//movimentos por segundo
#define VELOCIDADE 1

//tamanho menus
#define MENU_DERROTA 3
#define MENU_PAUSE 5
#define MENU_INICIO 3

#include "header.h"
#include "funcoes.h"

int main() {

    int ultimasteclas[11] = {0}; //registra ultimas 11 teclas
    int selecionado = 0; //opcao marcada nos menus
    int musicaAtual; //numero da musica que deve tocar
    ESTADO estado = {0}; //zera estado
    Texture2D texturas[MAX_TEXTURAS]; //vetor com texturas .png
    Music playlist[MAX_MUSICAS]; //vetor musicas .mp3
    inicializa(&estado, texturas, playlist); //preenche estruturas e vetores com os valores iniciais

    // Loop principal do jogo
    while (!WindowShouldClose()) {

        selecionaMusica(&estado, &musicaAtual); //adequa musica ao estado atual

        PlayMusicStream(playlist[musicaAtual]); //inicia musica

        UpdateMusicStream(playlist[musicaAtual]); //atualiza frame da musica

        if(estado.menu == 7) atualizarEstado(&estado, ultimasteclas);//se não está pausado, atualiza estado
        BeginDrawing();

        verificaEstado(&estado, &selecionado, playlist, musicaAtual, texturas);//faz as checagens e direcionamentos
        if (estado.derrota) estado.menu = 8; //perder o jogo
        if (estado.vitoria && !estado.derrota) vitoria(&estado, texturas, playlist); //passar de fase ou vencer o jogo

        EndDrawing();

    }

    CloseAudioDevice(); //fechga dispositivo de audio

    for (int i = 0; i < MAX_TEXTURAS; i++)UnloadTexture(texturas[i]); //descarrega texturas

    for (int i = 0; i < MAX_MUSICAS; i++)UnloadMusicStream(playlist[i]); //descarrega musicas

    CloseWindow();
    return 0;
}
