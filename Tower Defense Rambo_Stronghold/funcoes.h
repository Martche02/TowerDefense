#ifndef FUNCOES_H
#define FUNCOES_H

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

//tamanho "pixel"
#define QUAD_SIZE 20

#define MAX_INIMIGOS 50
#define MAX_RECURSOS 50
#define MAX_PAREDES 1000
#define MAX_PORTAIS 100
#define ULTIMAFASE 4
#define VELOCIDADE 1 //movimentos por segundo dos inimigos
#define MAX_TEXTURAS 14
#define MAX_MUSICAS 5

/// Fun  o para verificar colis o
int verificarColisao(POSICAO a, POSICAO b) {
    return (a.x == b.x && a.y == b.y);
}

// Fun  o para carregar o estado do jogo
void carregarEstado(char* filename, ESTADO* estado) {
    FILE* file = fopen(filename, "r+b");
    if (!file) {
        printf("Erro ao abrir arquivo para carregar");
        return;
    }
    fread(estado, sizeof(ESTADO), 1, file);
    fclose(file);
}

// Fun  o para salvar o estado do jogo
void salvarEstado(char* filename, ESTADO* estado) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Erro ao abrir arquivo para salvar");
        return;
    }
    fwrite(estado, sizeof(ESTADO), 1, file);
    fclose(file);
}

void cheat(int k, int *ultimasteclas, ESTADO* estado)
{
    int codigoTrapaca[11] = {257, 65, 66, 262, 263, 262, 263, 264, 264, 265, 265};
    int rocky[11] = {65, 79, 66, 76, 65, 66, 89, 75, 67, 79, 82};


    if (k != 0) {
        // Desloca as teclas anteriores para a direita
        for (int i = 10; i > 0; i--) {
            ultimasteclas[i] = ultimasteclas[i - 1];
        }
        // Armazena a nova tecla pressionada na primeira posicao
        ultimasteclas[0] = k;
    }
    // Compara as duas arrays para verificar se o Konami Code foi inserido
    if (memcmp(ultimasteclas, codigoTrapaca, sizeof(codigoTrapaca)) == 0) {
        estado->vitoria = 1;
        ultimasteclas[0] = 0;
    }
    if (memcmp(ultimasteclas, rocky, sizeof(codigoTrapaca)) == 0) {
        estado->musica.qualmusica = 9;
        ultimasteclas[0] = 0;
    }

}


void getAcao(int k, ESTADO *estado, POSICAO *novaPosJogador)
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
            estado->direcao = -1;
            break;

        case 68:
        case 262:
            novaPosJogador->x += QUAD_SIZE; // Mover para a direita
            estado->direcao = 1;
            break;

        case 71:
            // Colocar armadilha
            if (estado->armadilhasdisp > 0) {
                if ('A' != estado->mapa[novaPosJogador->y/20][novaPosJogador->x/20]){
                    estado->mapa[novaPosJogador->y/20][novaPosJogador->x/20]='A';
                    estado->posArmadilhas[estado->qtdArmadilhas].x=novaPosJogador->x;
                    estado->posArmadilhas[estado->qtdArmadilhas].y=novaPosJogador->y;
                    estado->armadilhasdisp--;
                    estado->qtdArmadilhas++;
                }
            }
            break;
        case 258:
            estado->menu = 5;
            break;
    }
}

void novaposinimigo(INIMIGO *inimigo)
{
    inimigo->pos.x += inimigo->dx * QUAD_SIZE;
    inimigo->pos.y -= inimigo->dy * QUAD_SIZE;
}

void colisaoJogadorRecurso(ESTADO *estado, POSICAO novaPosJogador)
{
// Colisao jogador/recurso
    for (int i = 0; i < estado->qtdRecursos; i++) {
        if (verificarColisao(novaPosJogador, estado->posRecursos[i])) {
            estado->armadilhasdisp++;
            estado->posRecursos[i] = (POSICAO){-1, -1}; // Remover frutinha
        }
    }
}

void colisaoJogadorParede(ESTADO *estado, POSICAO *novaPosJogador)
{
    if ('W' == estado->mapa[novaPosJogador->y/20][novaPosJogador->x/20]) {
        *novaPosJogador = estado->posJogador; // Reverte posicao
    }
}

void colisaoJogadorTunel(int k, ESTADO *estado, POSICAO *novaPosJogador)
{
    // Colis o jogador/portal
    if ('H' == estado->mapa[novaPosJogador->y/20][novaPosJogador->x/20])
    {
        int pos;
        switch(k)
        {
            case 87:
            case 265:
                 // Mover para cima
                pos = ALTURA;
                for (int j = 0; j < estado->qtdTuneis; j++) {
                        if(estado->posTuneis[j].x == novaPosJogador->x && estado->posTuneis[j].y < pos){
                            pos = estado->posTuneis[j].y;
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
                for (int j = 0; j < estado->qtdTuneis; j++) {
                        if(estado->posTuneis[j].x == novaPosJogador->x && estado->posTuneis[j].y >pos){
                            pos = estado->posTuneis[j].y;
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
                for (int j = 0; j < estado->qtdTuneis; j++) {
                        if(estado->posTuneis[j].y == novaPosJogador->y && estado->posTuneis[j].x <pos){
                            pos = estado->posTuneis[j].x;
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
                for (int j = 0; j < estado->qtdTuneis; j++) {
                        if(estado->posTuneis[j].y == novaPosJogador->y && estado->posTuneis[j].x >pos){
                            pos = estado->posTuneis[j].x;
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

void colisaoInimigoParede(ESTADO *estado)
{
    int i, guarda_dx, guarda_dy;
    for (i = 0; i < MAX_INIMIGOS; i++){
        if(estado->inimigo[i].vida == 1){
            if ('W' == estado->mapa[estado->inimigo[i].pos.y/20][estado->inimigo[i].pos.x/20] || 'H' == estado->mapa[estado->inimigo[i].pos.y/20][estado->inimigo[i].pos.x/20])
            {
                estado->inimigo[i].pos.x -= estado->inimigo[i].dx * QUAD_SIZE;
                estado->inimigo[i].pos.y += estado->inimigo[i].dy * QUAD_SIZE;
                guarda_dx=estado->inimigo[i].dx;
                guarda_dy=estado->inimigo[i].dy;

                if(estado->inimigo[i].dx != 0)
                {
                    estado->inimigo[i].dx=0;
                    estado->inimigo[i].dy=1;
                    novaposinimigo(&estado->inimigo[i]);

                    if ('W' == estado->mapa[estado->inimigo[i].pos.y/20][estado->inimigo[i].pos.x/20] || 'H' == estado->mapa[estado->inimigo[i].pos.y/20][estado->inimigo[i].pos.x/20])
                    {
                        estado->inimigo[i].pos.y += estado->inimigo[i].dy * QUAD_SIZE;
                        estado->inimigo[i].dy= -1;
                        novaposinimigo(&estado->inimigo[i]);

                        if('W' == estado->mapa[estado->inimigo[i].pos.y/20][estado->inimigo[i].pos.x/20] || 'H' == estado->mapa[estado->inimigo[i].pos.y/20][estado->inimigo[i].pos.x/20])
                        {
                            estado->inimigo[i].pos.y += estado->inimigo[i].dy * QUAD_SIZE;
                            estado->inimigo[i].dy=0;
                            estado->inimigo[i].dx= -guarda_dx;
                            novaposinimigo(&estado->inimigo[i]);
                        }
                    }
                }
                else if (estado->inimigo[i].dy != 0)
                {
                    estado->inimigo[i].dy=0;
                    estado->inimigo[i].dx=-1;
                    novaposinimigo(&estado->inimigo[i]);

                    if ('W' == estado->mapa[estado->inimigo[i].pos.y/20][estado->inimigo[i].pos.x/20] || 'H' == estado->mapa[estado->inimigo[i].pos.y/20][estado->inimigo[i].pos.x/20])
                    {
                        estado->inimigo[i].pos.x -= estado->inimigo[i].dx * QUAD_SIZE;
                        estado->inimigo[i].dx=1;
                        novaposinimigo(&estado->inimigo[i]);

                        if('W' == estado->mapa[estado->inimigo[i].pos.y/20][estado->inimigo[i].pos.x/20] || 'H' == estado->mapa[estado->inimigo[i].pos.y/20][estado->inimigo[i].pos.x/20])
                        {
                            estado->inimigo[i].pos.x -= estado->inimigo[i].dx * QUAD_SIZE;
                            estado->inimigo[i].dx=0;
                            estado->inimigo[i].dy= -guarda_dy;
                            novaposinimigo(&estado->inimigo[i]);
                        }
                    }
                }
            }
        }

    }
}

void colisaoArmadilhaInimigo(ESTADO *estado)
{
    for (int j = 0; j < MAX_INIMIGOS; j++)
    {
        if(estado->inimigo[j].vida ==1)
        {
            if(('A' == estado->mapa[estado->inimigo[j].pos.y/20][estado->inimigo[j].pos.x/20]))
            {
                estado->mapa[estado->inimigo[j].pos.y/20][estado->inimigo[j].pos.x/20] = ' ';
                estado->qtdInimigos--;
                estado->inimigo[j].vida=0;

            }
        }
    }
}

void colisaoInimigoTorre(ESTADO *estado)
{
     for (int i = 0; i < MAX_INIMIGOS; i++) {
        if(estado->inimigo[i].vida == 1){
            if (verificarColisao(estado->inimigo[i].pos, estado->posBase)) {
                estado->inimigo[i].vida=0;
                estado->vidas--;
                estado->qtdInimigos--;
                i--;
                estado->atingido = 1;
                break;
            }
        }
    }
}

void colisaoJogadorInimigo(ESTADO *estado, POSICAO novaPosJogador)
{
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        if(estado->inimigo[i].vida==1){
            if (verificarColisao(estado->inimigo[i].pos, novaPosJogador))
            {
                estado->vidaJogador--;
                estado->inimigo[i].vida = 0;
            }
        }
    }

}

ESTADO atualizarEstado(ESTADO estado, int ultimasteclas[]) {
    estado.tempo++;
    POSICAO novaPosJogador = estado.posJogador;
    int i;
    int k = GetKeyPressed();
    if(estado.menu == 7)cheat(k, ultimasteclas, &estado);//checa últimas 11 teclas pra ver se trapaca funcionou
    getAcao(k, &estado, &novaPosJogador);

    colisaoJogadorInimigo(&estado, novaPosJogador);

    //Atualiza posicoes dos inimigos
    if(estado.tempo % (60/VELOCIDADE) == 0) {
        for (i = 0; i < MAX_INIMIGOS; i++){
            if(estado.inimigo[i].vida == 1) novaposinimigo(&estado.inimigo[i]);
        }
    }

    colisaoJogadorRecurso(&estado, novaPosJogador);

    colisaoJogadorTunel(k, &estado, &novaPosJogador);

    colisaoJogadorParede(&estado, &novaPosJogador);

    if(estado.tempo % (60/VELOCIDADE) == 0)
    {
        colisaoInimigoParede(&estado);
        colisaoArmadilhaInimigo(&estado);
        colisaoInimigoTorre(&estado);
    }

    if (estado.vidas <= 0 || estado.vidaJogador <= 0)
        estado.derrota = 1;
    if (estado.qtdInimigos == 0)
        estado.vitoria = 1;


    estado.posJogador = novaPosJogador;


    return estado;
}


void carregarMapaDeArquivo(ESTADO *estado) {
    char* caminhoArquivo;
    char mapa[20];
    snprintf(mapa, sizeof(mapa), "mapas/mapa%d.txt", estado->nivelatual);
    caminhoArquivo = mapa;
    FILE* arquivo = fopen(caminhoArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    int i, y, j, x;
    // Inicializar valores de estado
    estado->qtdInimigos = 0;
    estado->qtdRecursos = 0;
    estado->qtdTuneis = 0;
    estado->qtdParedes = 0;
    estado->qtdArmadilhas = 0;
    estado->vitoria = 0;
    estado->derrota = 0;
    estado->tempo = 0;
    estado->armadilhasdisp = 0;

    memset(estado->posParedes, 0, sizeof(estado->posParedes));
    memset(estado->posTuneis, 0, sizeof(estado->posTuneis));
    memset(estado->inimigo, 0, sizeof(estado->inimigo));
    memset(estado->posRecursos, 0, sizeof(estado->posRecursos));
    memset(&estado->posJogador, 0, sizeof(estado->posJogador));

    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            estado->mapa[i][j] = ' ';
        }
    }

    char linha[MAP_WIDTH + 2]; // +2 para o '\n' e '\0'
    y = 0;

    while (fgets(linha, sizeof(linha), arquivo) && y < MAP_HEIGHT) {
        for (x = 0; x < MAP_WIDTH && linha[x] != '\n'; x++) {
            estado->mapa[y][x] = linha[x]; // Armazenar o caractere no mapa
            POSICAO pos = {x * QUAD_SIZE, y * QUAD_SIZE};
            switch (linha[x]) {
                case 'J':
                    estado->posJogador = pos;
                    break;
                case 'M':
                    estado->inimigo[estado->qtdInimigos].pos = pos; ///////////////////////////////////Mexi aqui
                    estado->inimigo[estado->qtdInimigos].dx= -1;
                    estado->inimigo[estado->qtdInimigos].dy= 0;
                    estado->inimigo[estado->qtdInimigos].vida=1;
                    estado->qtdInimigos++;

                    break;
                case 'R':
                    estado->posRecursos[estado->qtdRecursos++] = pos;
                    break;
                case 'H':
                    estado->posTuneis[estado->qtdTuneis++] = pos;
                    break;
                case 'W':
                    estado->posParedes[estado->qtdParedes++] = pos;
                    break;
                case 'S':
                    estado->posBase = pos;
                    break;
            }
        }
        y++;
    }

    fclose(arquivo);
}


void desenho(ESTADO *estado, Texture2D texturas[])
{
        DrawTexture(texturas[2], 0, 0, WHITE);

        char text[100];
        sprintf(text, "Fase atual: %d    Vidas da torre: %d   Vidas do jogador: %d    Armadilhas: %d   Inimigos restantes: %d",
               estado->nivelatual, estado->vidas, estado->vidaJogador, estado->armadilhasdisp, estado->qtdInimigos);

        //desenha inimigos mortos
        for (int i = 0; i < MAX_INIMIGOS; i++)
        {
            if(estado->inimigo[i].vida == 0)
            {
                DrawRectangle(estado->inimigo[i].pos.x + 5, estado->inimigo[i].pos.y + 7, 10, 2, WHITE);
                DrawRectangle(estado->inimigo[i].pos.x + 9, estado->inimigo[i].pos.y + 3, 2, 14, WHITE);
            }

        }



        //desenha armadilhasdisp
        for (int i = 0; i < estado->qtdRecursos; i++) {
            DrawTexture(texturas[3], estado->posRecursos[i].x, estado->posRecursos[i].y, WHITE);
        }

        //desenha inimigos vivos
        for (int i = 0; i < MAX_INIMIGOS; i++)
        {
            if(estado->inimigo[i].vida == 1)DrawTexture(texturas[11], estado->inimigo[i].pos.x, estado->inimigo[i].pos.y, WHITE);
        }

        //desenha armadilhas
        for (int i = 0; i < estado->qtdArmadilhas; i++) {
            if(('A' == estado->mapa[estado->posArmadilhas[i].y/20][estado->posArmadilhas[i].x/20])) {
                DrawRectangle(estado->posArmadilhas[i].x + 8, estado->posArmadilhas[i].y, 4, QUAD_SIZE, DARKGRAY);
                DrawRectangle(estado->posArmadilhas[i].x , estado->posArmadilhas[i].y + 8, QUAD_SIZE, 4, DARKGRAY);
            }
        }

        //desenha jogador
        if(estado->direcao == 1)DrawTexture(texturas[10], estado->posJogador.x, estado->posJogador.y, WHITE);
        else DrawTexture(texturas[13], estado->posJogador.x, estado->posJogador.y, WHITE);

        //desenha buracos
        for (int i = 0; i < estado->qtdTuneis; i++) {
            DrawTexture(texturas[4], estado->posTuneis[i].x, estado->posTuneis[i].y, WHITE);

        }

        //desenho barreiras
        for (int i = 0; i < estado->qtdParedes; i++) {
            DrawTexture(texturas[5], estado->posParedes[i].x, estado->posParedes[i].y, WHITE);
        }

        //desenha base
        DrawTexture(texturas[12], estado->posBase.x, estado->posBase.y, WHITE);

        DrawText(text, 9, 3, 20, WHITE);
}

void novaFase(ESTADO *estado, Texture2D texturas[])//funcao para tela entre fases
{
    DrawTexture(texturas[8], 0, 0, WHITE);
    char text1[50];
    sprintf(text1, "Rambo resistiu a onda %d de inimigos!", estado->nivelatual-1);
    DrawText(text1, 240, 20, 40, WHITE);
    DrawText("pressione ENTER para continuar", 850, 570, 20, WHITE);
    if(IsKeyPressed(KEY_ENTER))estado->menu = 7;//continua
}


int cutsceneinicial(ESTADO *estado, Texture2D texturas[])
{
    //Inicializa a tela para a cutscene
    ClearBackground(BLACK);


    // Texto a ser exibido
    const char *frase1 = "\n\n\n   Nas entranhas das linhas inimigas, um ponto vital foi \n\n\n             capturado pelas tropas dos EUA.";
    const char *frase2 = " Fatalmente, a posicao desta instalacao foi comprometida.\n\n\n        Unidades inimigas foram detectadas ao Leste.";
    const char *frase3 = "  Ela precisara resistir ate a chegada de novas tropas.";
    const char *frase4 = "   Agora, resta apenas um homem que pode defende-la.";
    const char *avanca = "pressione ENTER para continuar";

    // pagina atual da cutscene

        switch(estado->pagina)
        {
            case 0:
                DrawText(avanca, 850, 570, 20, WHITE);
                break;

            case 1:
                DrawText(avanca, 850, 570, 20, WHITE);
                DrawText(frase1, 10, 10, 40, WHITE);
                break;

            case 2:
                DrawText(avanca, 850, 570, 20, WHITE);
                DrawText(frase1, 10, 10, 40, WHITE);
                DrawText(frase2, 10, 190, 40, WHITE);
                break;

            case 3:
                DrawText(avanca, 850, 570, 20, WHITE);
                DrawText(frase1, 10, 10, 40, WHITE);
                DrawText(frase2, 10, 190, 40, WHITE);
                DrawText(frase3, 10, 320, 40, WHITE);
                break;

             case 4:
                DrawText(avanca, 850, 570, 20, WHITE);
                DrawText(frase1, 10, 10, 40, WHITE);
                DrawText(frase2, 10, 190, 40, WHITE);
                DrawText(frase3, 10, 320, 40, WHITE);
                DrawText(frase4, 20, 400, 40, WHITE);
                break;

             case 5:
                DrawTexture(texturas[6], 0, 0, WHITE);
                DrawText(avanca, 850, 570, 20, BLACK);
                break;

            case 6:
                DrawTexture(texturas[14], 0, 0, WHITE);
                DrawText(avanca, 850, 570, 20, BLACK);
                break;

             case 7:
                estado->pagina = 0;
                return 1;
                break;
        }

        // Avanca para o proximo passo se a tecla Enter for pressionada
        if (IsKeyPressed(KEY_ENTER))
        {
            estado->pagina++;
        }
        return 0;
    }

int cutscenederrota(ESTADO *estado)
{
    //Inicializa a tela para a cutscene
    ClearBackground(BLACK);


    // Texto a ser exibido
    const char *frase5 = "A base americana nao resistiu as ofensivas inimigas.";
    const char *frase6 = "Os homens que a protegiam foram levados como\n\n\n                      prisioneiros.";
    const char *frase7 = "O antigo Boina Verde John Rambo nunca foi encontrado.";
    const char *avanca = "pressione ENTER para continuar";

    // pagina atual da cutscene

        switch(estado->pagina)
        {
            case 0:
                DrawText(avanca, 850, 570, 20, WHITE);
                break;

            case 1:
                DrawText(avanca, 850, 570, 20, WHITE);
                DrawText(frase5, 70, 80, 40, WHITE);
                break;

            case 2:
                DrawText(avanca, 850, 570, 20, WHITE);
                DrawText(frase5, 70, 80, 40, WHITE);
                DrawText(frase6, 100, 210, 40, WHITE);
                break;

            case 3:
                DrawText(avanca, 850, 570, 20, WHITE);
                DrawText(frase5, 70, 80, 40, WHITE);
                DrawText(frase6, 100, 210, 40, WHITE);
                DrawText(frase7, 30, 360, 40, WHITE);
                break;

             case 4:
                return 1;
                break;
        }

        // Avanca para o proximo passo se a tecla Enter for pressionada
        if (IsKeyPressed(KEY_ENTER))
        {
            estado->pagina++;
        }
    return 0;
}

int cutscenefim(ESTADO *estado)
{
    //Inicializa a tela para a cutscene
    ClearBackground(BLACK);


    // Texto a ser exibido
    const char *frase8 = "Apesar de sucessivas incursoes inimigas, a guarnicao\n\n\n                 americana manteve-se de pe.";
    const char *frase9 = " O antigo Boina Verde John Rambo foi mais uma vez\n\n\nreconhecido por sua bravura e sua deteminacao no\n\n\n                      campo de batalha.";
    const char *frase10 = "Ele, enfim, pode voltar para casa.";
    const char *avanca = "pressione ENTER para continuar";

    // pagina atual da cutscene

        switch(estado->pagina)
        {
            case 0:
                DrawText(avanca, 850, 570, 20, WHITE);
                break;

            case 1:
                DrawText(avanca, 850, 570, 20, WHITE);
                DrawText(frase8, 40, 80, 40, WHITE);
                break;

            case 2:
                DrawText(avanca, 850, 570, 20, WHITE);
                DrawText(frase8, 40, 80, 40, WHITE);
                DrawText(frase9, 60, 230, 40, WHITE);
                break;

            case 3:
                DrawText(avanca, 850, 570, 20, WHITE);
                DrawText(frase8, 40, 80, 40, WHITE);
                DrawText(frase9, 60, 230, 40, WHITE);
                DrawText(frase10, 240, 410, 40, WHITE);
                break;

             case 4:
                return 1;
                break;
        }

        // Avanca para o proximo passo se a tecla Enter for pressionada
        if (IsKeyPressed(KEY_ENTER))
        {
            estado->pagina++;
        }
    return 0;
}


void verificaEstado(ESTADO *estado, int *selecionado, Music playlist[], int musicaAtual, Texture2D texturas[]) // checa condicao a cada ciclo
{
    switch (estado->menu)
    {
        case 0: // menu inicio
            estado->pagina = 0;
            DrawTexture(texturas[0], 0, 0, WHITE);

            // Navegacao pelo menu
            if (IsKeyPressed(KEY_DOWN)) *selecionado = (*selecionado + 1) % MENU_INICIO;
            if (IsKeyPressed(KEY_UP)) *selecionado = (*selecionado - 1 + MENU_INICIO) % MENU_INICIO;

            // Desenha as opcoes do menu com a seta de selecao
            for (int i = 0; i < MENU_INICIO; i++) {
                if (i == *selecionado) {
                    DrawText(">", 20, 148 + i * 81, 50, BLACK);
                }
            }

            // Acao baseada na opcao selecionada
            if (IsKeyPressed(KEY_ENTER)) {
                switch (*selecionado) {
                    case 0:
                        StopMusicStream(playlist[musicaAtual]);  // Para a música
                        estado->menu = 1;
                        break; // novo jogo
                    case 1:
                        StopMusicStream(playlist[musicaAtual]);  // Para a música
                        carregarEstado("save/savegame.bin", estado);
                        estado->menu = 7; // continua o jogo
                        break; // carregar jogo
                    case 2:
                        StopMusicStream(playlist[musicaAtual]);  // Para a música
                        CloseWindow();
                        break; // sair
                }
            }
            break;

        case 1: // novo jogo
            if (cutsceneinicial(estado, texturas) == 1)
            {
                estado->vidas = 3;
                estado->vidaJogador = 1;
                estado->nivelatual = 1;
                carregarMapaDeArquivo(estado); // reseta o nível
                estado->menu = 7; // continua o jogo
            }
            else
            {
                estado->menu = 1;
            }
            break;

        case 4: // nova fase
            novaFase(estado, texturas); // função para tela entre fases
            break;

        case 5: // menu de pause
            DrawTexture(texturas[1], 0, 0, WHITE);

            // Atualize o número total de opcoes
            DrawText("Pausado", 400, 100, 40, WHITE);
            DrawText("Continuar", 400, 200, 40, WHITE);
            DrawText("Carregar Jogo", 400, 260, 40, WHITE);
            DrawText("Salvar Jogo", 400, 320, 40, WHITE);
            DrawText("Voltar ao Menu", 400, 380, 40, WHITE);
            DrawText("Volume", 400, 440, 40, WHITE); // Nova opcao de volume
            DrawText("Sair", 400, 500, 40, WHITE);

            // Navegacao pelo menu de pause
            if (IsKeyPressed(KEY_DOWN)) *selecionado = (*selecionado + 1) % MENU_PAUSE;
            if (IsKeyPressed(KEY_UP)) *selecionado = (*selecionado - 1 + MENU_PAUSE) % MENU_PAUSE;

            for (int i = 0; i < MENU_PAUSE; i++) {
                if (i == *selecionado) {
                    DrawText(">", 350, 200 + i * 60, 40, WHITE);
                }
            }

            // Exibe e ajusta o volume se a opcao de volume estiver selecionada
            if (*selecionado == 4) {
                DrawText(TextFormat("%d%%", (int)(estado->musica.volume * 100)), 600, 440, 40, WHITE); // Mostra o volume atual

                // Ajusta o volume
                if (IsKeyPressed(KEY_RIGHT) && estado->musica.volume < 1.0f) {
                    estado->musica.volume += 0.05f; // Aumenta o volume
                    SetMusicVolume(playlist[musicaAtual], estado->musica.volume); // Aplica o volume a música atual na playlist
                }
                if (IsKeyPressed(KEY_LEFT) && estado->musica.volume > 0.0f) {
                    estado->musica.volume -= 0.05f; // Diminui o volume
                    SetMusicVolume(playlist[musicaAtual], estado->musica.volume); // Aplica o volume a música atual na playlist
                }
            }

            // Acao para alternar o estado do menu com a tecla Tab
            int tab = GetKeyPressed();
            if(tab == 258)estado->menu = 7; // Alterna o estado do menu

            if (IsKeyPressed(KEY_ENTER)) {
                switch (*selecionado) {
                    case 0:
                        estado->menu = 7;
                        *selecionado = 0;
                        break; // continuar jogo
                    case 1:
                        carregarEstado("save/savegame.bin", estado);
                        *selecionado = 0;
                        break; // carregar jogo
                    case 2:
                        salvarEstado("save/savegame.bin", estado);
                        *selecionado = 0;
                        break; // salvar jogo
                    case 3:
                        estado->menu = 0;
                        *selecionado = 0;
                        break; // voltar ao menu inicial
                    case 5:
                        CloseWindow();
                        *selecionado = 0;
                        break; // sair
                }
            }
            break;


        case 6: // venceu jogo
            if (cutscenefim(estado) == 0)estado->menu = 6; // continua a cutscene
            else{
            DrawTexture(texturas[9], 0, 0, WHITE);
            DrawText("Parabens, voce concluiu a missao!!!", 270, 20, 40, WHITE);
            DrawText("pressione ENTER para voltar ao menu", 800, 570, 20, BLACK);
            if (IsKeyPressed(KEY_ENTER)) estado->menu = 0; // retorna ao menu inicial
            }
            break;

        case 7: // operando normalmente
            desenho(estado, texturas);
            break;

        case 8: // game over
            if (cutscenederrota(estado) == 0)estado->menu = 8; // continua a cutscene
            else
            {
                DrawTexture(texturas[7], 0, 0, WHITE);
                DrawText("GAME OVER", 360, 160, 40, WHITE);
                DrawText("Voltar ao MENU", 360, 230, 40, WHITE);
                DrawText("Carregar Jogo Salvo", 360, 290, 40, WHITE);
                DrawText("Reiniciar", 360, 350, 40, WHITE);

                // Navegacao pelo menu de game over
                if (IsKeyPressed(KEY_DOWN)) *selecionado = (*selecionado + 1) % MENU_DERROTA;
                if (IsKeyPressed(KEY_UP)) *selecionado = (*selecionado - 1 + MENU_DERROTA) % MENU_DERROTA;

                for (int i = 0; i < MENU_DERROTA; i++) {
                    if (i == *selecionado) {
                        DrawText(">", 320, 230 + i * 60, 45, WHITE);
                    }
                }

                // Acao baseada na opcao selecionada
                if (IsKeyPressed(KEY_ENTER)) {
                    TraceLog(LOG_INFO, "Tecla Enter pressionada! Opcao selecionada: %d", *selecionado);
                    switch (*selecionado) {
                        case 0: // voltar ao menu inicial
                            estado->derrota = 0;
                            estado->menu = 0;
                            *selecionado = 0;
                            break;
                        case 1: // carregar jogo salvo
                            estado->derrota = 0;
                            carregarEstado("save/savegame.bin", estado);
                            *selecionado = 0;
                            estado->menu = 7;
                            break;
                        case 2: // reiniciar jogo
                            estado->derrota = 0;
                            estado->menu = 1;
                            *selecionado = 0;
                            break;
                    }
                }
            }
            break;
    }
}

void selecionaMusica(ESTADO *estado, int *musicaAtual)
{
    if (estado->musica.qualmusica != 9)estado->musica.qualmusica = estado->menu;
    switch(estado->musica.qualmusica)
    {
        case 0:
            *musicaAtual = 0;
            break;
        case 1:
            *musicaAtual = 2;
            break;
        case 6:
            *musicaAtual = 3;
            break;
        case 7:
            *musicaAtual = 1;
            break;
        case 9:
            *musicaAtual = 4;
            break;

    }
}

void inicializa(ESTADO* estado, Texture2D* texturas, Music* playlist){
    InitWindow(LARGURA, ALTURA, "Tower Defense");
    SetTargetFPS(60);
    estado->nivelatual = 1;
    estado->vidas = 3;
    estado->vidaJogador = 1;
    carregarMapaDeArquivo(estado);
    estado->menu = 0;
    estado->pagina = 0;
    estado->musica.volume = 0.5;

    // Inicializa o dispositivo de áudio
    InitAudioDevice();
    texturas[0] = LoadTexture("texturas/imagemmenu2.png"); //imagem rambo menu principal
    texturas[1] = LoadTexture("texturas/imagempause.png"); //imagem pause
    texturas[2] = LoadTexture("texturas/grama.png"); //grama
    texturas[3] = LoadTexture("texturas/recursos.png"); //caixas
    texturas[4] = LoadTexture("texturas/buraco.png"); //tuneis
    texturas[5] = LoadTexture("texturas/arvore.png"); //paredes
    texturas[6] = LoadTexture("texturas/tutorial.png"); //tutorial
    texturas[7] = LoadTexture("texturas/bandana.png");
    texturas[8] = LoadTexture("texturas/vitoria.png");
    texturas[9] = LoadTexture("texturas/fim.png");
    texturas[10] = LoadTexture("texturas/johnrambo.png");
    texturas[11] = LoadTexture("texturas/inimigo.png");
    texturas[12] = LoadTexture("texturas/base.png");
    texturas[13] = LoadTexture("texturas/johnrambo2.png");
    texturas[14] = LoadTexture("texturas/tutorial2.png");

    playlist[0] = LoadMusicStream("musicas/musicamenu.mp3");
    playlist[1] = LoadMusicStream("musicas/musicafases.mp3");
    playlist[2] = LoadMusicStream("musicas/musicacutscene.mp3");
    playlist[3] = LoadMusicStream("musicas/musicafim.mp3");
    playlist[4] = LoadMusicStream("musicas/rockysong.mp3");


}

void vitoria(ESTADO* estado, Texture2D* texturas, Music* playlist){
    ClearBackground(BLACK);
    EndDrawing();
    if(estado->qtdRecursos == estado->qtdArmadilhas+estado->armadilhasdisp)estado->vidas++;//se jogador pegou todos os recursos, base ganha uma vida
    if(estado->atingido == 0)estado->vidaJogador++;//se a base não foi invadida, jogador ganha uma vida
    estado->nivelatual ++;
    int e = estado->nivelatual;
    int v = estado->vidas;
    int vj = estado->vidaJogador;
    memset(estado, 0, sizeof(ESTADO));
    estado->nivelatual = e;
    estado->vidas = v;
    estado->vidaJogador = vj;
    estado->menu = 4;
    if(estado->nivelatual > ULTIMAFASE)//venceu o jogo
    {
        estado->nivelatual = 1;
        estado->menu = 6;
    }
    carregarMapaDeArquivo(estado);
}


#endif
