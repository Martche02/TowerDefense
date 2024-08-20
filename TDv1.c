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
#define VELOCIDADE 60


typedef struct Posicao{
    int x;
    int y;
} POSICAO;

typedef struct inimigo{
    int dx;
    int dy;
    int vida;
    POSICAO pos;
} INIMIGO;


// Defini��o da estrutura Estado
typedef struct Estado {
    int vidas;
    int vidaJogador;
    int qtdInimigos;
    int qtdRecursos;
    int qtdArmadilhas;
    int qtdPortais;
    int qtdParedes;
    POSICAO posRecursos[MAX_RECURSOS];
    POSICAO posPortais[MAX_PORTAIS];
    POSICAO posParedes[MAX_PAREDES];
    POSICAO posJogador;
    POSICAO posArmadilhas[MAX_RECURSOS];
    POSICAO posBase;
    char mapa[MAP_HEIGHT][MAP_WIDTH];
    int vitoria;
    int derrota;
    int tempo;
    int recursos;
    int nivelatual;
    int menu;
    int musica;
    int pagina;
    float volume;
    INIMIGO inimigo[MAX_INIMIGOS];
    int atingido;
}ESTADO;

/// Fun��o para verificar colis�o
int verificarColisao(POSICAO a, POSICAO b) {
    return (a.x == b.x && a.y == b.y);
}

// Fun��o para carregar o estado do jogo
void carregarEstado(char* filename, ESTADO* estado) {
    FILE* file = fopen(filename, "r+b");
    if (!file) {
        printf("Erro ao abrir arquivo para carregar");
        return;
    }
    fread(estado, sizeof(ESTADO), 1, file);
    fclose(file);
}

// Fun��o para salvar o estado do jogo
void salvarEstado(char* filename, ESTADO* estado) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Erro ao abrir arquivo para salvar");
        return;
    }
    fwrite(estado, sizeof(ESTADO), 1, file);
    fclose(file);
}

int cheat(int k, int *ultimasteclas, ESTADO* estado)
{
    int codigoTrapaca[11] = {257, 65, 66, 262, 263, 262, 263, 264, 264, 265, 265};
    int rocky[11] = {65, 79, 66, 76, 65, 66, 89, 75, 67, 79, 82};


    if (k != 0) {
        // Desloca as teclas anteriores para a direita
        for (int i = 10; i > 0; i--) {
            ultimasteclas[i] = ultimasteclas[i - 1];
            printf("%d\n",ultimasteclas[i]);
        }
        // Armazena a nova tecla pressionada na primeira posição
        ultimasteclas[0] = k;
        printf("%d\n",ultimasteclas[0]);
    }
    // Compara as duas arrays para verificar se o Konami Code foi inserido
    if (memcmp(ultimasteclas, codigoTrapaca, sizeof(codigoTrapaca)) == 0) {
        estado->vitoria = 1;
        ultimasteclas[0] = 0;
    }
    if (memcmp(ultimasteclas, rocky, sizeof(codigoTrapaca)) == 0) {
        estado->musica = 9;
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
            break;

        case 68:
        case 262:
            novaPosJogador->x += QUAD_SIZE; // Mover para a direita
            break;

        case 71:
            // Colocar armadilha
            if (estado->recursos > 0) {
                if ('A' != estado->mapa[novaPosJogador->y/20][novaPosJogador->x/20]){
                    estado->mapa[novaPosJogador->y/20][novaPosJogador->x/20]='A';
                    estado->posArmadilhas[estado->qtdArmadilhas].x=novaPosJogador->x;
                    estado->posArmadilhas[estado->qtdArmadilhas].y=novaPosJogador->y;
                    estado->recursos--;
                    estado->qtdArmadilhas++;
                }
            }
            break;
        case 258:
            estado->menu = 5;
    }
}

void novaposinimigo(INIMIGO *inimigo)
{
    inimigo->pos.x += inimigo->dx * QUAD_SIZE;
    inimigo->pos.y -= inimigo->dy * QUAD_SIZE;
}

void colisaoJogadorRecurso(ESTADO *estado, POSICAO novaPosJogador)
{
// Colisão jogador/recurso
    for (int i = 0; i < estado->qtdRecursos; i++) {
        if (verificarColisao(novaPosJogador, estado->posRecursos[i])) {
            estado->recursos++;
            estado->posRecursos[i] = (POSICAO){-1, -1}; // Remover frutinha
        }
    }
}

void colisaoJogadorParede(ESTADO *estado, POSICAO *novaPosJogador)
{
    if ('W' == estado->mapa[novaPosJogador->y/20][novaPosJogador->x/20]) {
        *novaPosJogador = estado->posJogador; // Reverte posição
    }
}

void colisaoJogadorPortal(int k, ESTADO *estado, POSICAO *novaPosJogador)
{
    // Colis�o jogador/portal
    if ('H' == estado->mapa[novaPosJogador->y/20][novaPosJogador->x/20])
    {
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
    if(estado.menu == 7)cheat(k, ultimasteclas, &estado);//checa últimas 11 teclas pra ver se trapaça funcionou
    getAcao(k, &estado, &novaPosJogador);

    colisaoJogadorInimigo(&estado, novaPosJogador);

    //Atualiza posições dos inimigos
    if(estado.tempo % VELOCIDADE == 0) {
        for (i = 0; i < MAX_INIMIGOS; i++){
            if(estado.inimigo[i].vida == 1) novaposinimigo(&estado.inimigo[i]);
        }
    }

    colisaoJogadorRecurso(&estado, novaPosJogador);

    colisaoJogadorPortal(k, &estado, &novaPosJogador);

    colisaoJogadorParede(&estado, &novaPosJogador);

    if(estado.tempo % VELOCIDADE == 0)
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


void carregarMapaDeArquivo(ESTADO* estado, const char* caminhoArquivo) {
    FILE* arquivo = fopen(caminhoArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    int i, y, j, x;
    // Inicializar valores de estado
    estado->qtdInimigos = 0;
    estado->qtdRecursos = 0;
    estado->qtdPortais = 0;
    estado->qtdParedes = 0;
    estado->qtdArmadilhas = 0;
    estado->vitoria = 0;
    estado->derrota = 0;
    estado->tempo = 0;
    estado->recursos = 0;

    memset(estado->posParedes, 0, sizeof(estado->posParedes));
    memset(estado->posPortais, 0, sizeof(estado->posPortais));
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
                    estado->posPortais[estado->qtdPortais++] = pos;
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

void carregaNivel(ESTADO* estado)
{
    char mapa[20];
    snprintf(mapa, sizeof(mapa), "Mapa%d.txt", estado->nivelatual);
    printf("%s", mapa);
    carregarMapaDeArquivo(estado, mapa);
}

void desenhaRambo(ESTADO *estado) {
    // Definindo as cores
    Color MARROM = (Color){100, 60, 0, 255};
    Color PELE = {204, 153, 102, 255};

    // Desenhando o boneco com 4 retângulos
    DrawRectangle(estado->posJogador.x, estado->posJogador.y, QUAD_SIZE, 4, BLACK);  // Parte preta
    DrawRectangle(estado->posJogador.x, estado->posJogador.y + 4, QUAD_SIZE, 3, RED);    // Parte vermelha
    DrawRectangle(estado->posJogador.x, estado->posJogador.y + 7, QUAD_SIZE, 6, PELE);   // Parte de pele
    DrawRectangle(estado->posJogador.x, estado->posJogador.y + 13, QUAD_SIZE, 7, MARROM); // Parte marrom escuro

}

void desenho(ESTADO *estado, Texture2D texturas[])
{
        DrawTexture(texturas[2], 0, 0, WHITE);

        char text[100];
        sprintf(text, "Fase atual: %d    Vidas da torre: %d   Vidas do jogador: %d    Armadilhas: %d   Inimigos restantes: %d",
               estado->nivelatual, estado->vidas, estado->vidaJogador, estado->recursos, estado->qtdInimigos);

        for (int i = 0; i < MAX_INIMIGOS; i++)
        {
            if(estado->inimigo[i].vida == 0)
            {
                DrawRectangle(estado->inimigo[i].pos.x + 5, estado->inimigo[i].pos.y + 7, 10, 2, WHITE);
                DrawRectangle(estado->inimigo[i].pos.x + 9, estado->inimigo[i].pos.y + 3, 2, 14, WHITE);
            }

        }



        //desenha recursos
        for (int i = 0; i < estado->qtdRecursos; i++) {
            DrawTexture(texturas[3], estado->posRecursos[i].x, estado->posRecursos[i].y, WHITE);
        }

        for (int i = 0; i < MAX_INIMIGOS; i++)
        {
            if(estado->inimigo[i].vida == 1)DrawRectangle(estado->inimigo[i].pos.x, estado->inimigo[i].pos.y, QUAD_SIZE, QUAD_SIZE, RED);
        }

        //desenha armadilhas
        for (int i = 0; i < estado->qtdArmadilhas; i++) {
            if(('A' == estado->mapa[estado->posArmadilhas[i].y/20][estado->posArmadilhas[i].x/20])) {
                DrawRectangle(estado->posArmadilhas[i].x + 8, estado->posArmadilhas[i].y, 4, QUAD_SIZE, DARKGRAY);
                DrawRectangle(estado->posArmadilhas[i].x , estado->posArmadilhas[i].y + 8, QUAD_SIZE, 4, DARKGRAY);
            }
        }

        //desenha jogador
        desenhaRambo(estado);

        //desenha buracos
        for (int i = 0; i < estado->qtdPortais; i++) {
            DrawTexture(texturas[4], estado->posPortais[i].x, estado->posPortais[i].y, WHITE);

        }

        //desenho barreiras
        for (int i = 0; i < estado->qtdParedes; i++) {
            DrawTexture(texturas[5], estado->posParedes[i].x, estado->posParedes[i].y, WHITE);
        }

        DrawRectangle(estado->posBase.x, estado->posBase.y, QUAD_SIZE, QUAD_SIZE, WHITE);

        DrawText(text, 9, 3, 20, WHITE);
}

void novaFase(ESTADO *estado, Texture2D texturas[])//funcao para tela entre fases
{
    DrawTexture(texturas[8], 0, 0, WHITE);
    char text1[50];
    sprintf(text1, "Rambo resistiu a %dº onda de inimigos!", estado->nivelatual-1);
    DrawText(text1, 240, 20, 40, WHITE);
    DrawText("Precione ENTER para continuar", 850, 570, 20, WHITE);
    if (IsKeyPressed(KEY_ENTER))estado->menu = 7;//continua
}

int cutscene(ESTADO *estado, Texture2D texturas[])
{
    //Inicializa a tela para a cutscene
    ClearBackground(BLACK);


    // Texto a ser exibido
    const char *frase1 = "\n\n\n   Nas entranhas das linhas inimigas, um ponto vital foi \n\n\n             capturado pelas tropas dos EUA.";
    const char *frase2 = " Fatalmente, a posição desta instalação foi comprometida.\n\n\n        Unidades inimigas foram detectadas ao Leste.";
    const char *frase3 = "  Ela precisará resistir até a chegada de novas tropas.";
    const char *frase4 = "   Agora, resta apenas um homem que pode defendê-la.";
    const char *avanca = "Precione ENTER para continuar";

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
                estado->pagina = 0;
                return 1;
                break;
        }

        // Avança para o próximo passo se a tecla Enter for pressionada
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
    const char *frase5 = "A base americana não resistiu às ofensivas inimigas.";
    const char *frase6 = "Os homens que a protegiam foram levados como\n\n\n                      prisioneiros.";
    const char *frase7 = "O antigo Boina Verde John Rambo nunca foi encontrado.";
    const char *avanca = "Precione ENTER para continuar";

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

        // Avança para o próximo passo se a tecla Enter for pressionada
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
    const char *frase8 = "Apesar de sucessivas incursões inimigas, a guarnição\n\n\n                 americana manteve-se de pé.";
    const char *frase9 = " O antigo Boina Verde John Rambo foi mais uma vez\n\n\nreconhecido por sua bravura e sua deteminação no\n\n\n                      campo de batalha.";
    const char *frase10 = "Ele, enfim, pôde voltar para casa.";
    const char *avanca = "Precione ENTER para continuar";

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

        // Avança para o próximo passo se a tecla Enter for pressionada
        if (IsKeyPressed(KEY_ENTER))
        {
            estado->pagina++;
        }
    return 0;
}


void menuControle(ESTADO *estado, int *contagemMenu, int *selecionado, Music playlist[], int musicaAtual, Texture2D texturas[]) // checa condicao a cada ciclo
{
    switch (estado->menu)
    {
        case 0: // menu inicio
            estado->pagina = 0;
            *contagemMenu = 3; // número de opções no menu inicial
            DrawTexture(texturas[0], 0, 0, WHITE);

            // Navegação pelo menu
            if (IsKeyPressed(KEY_DOWN)) *selecionado = (*selecionado + 1) % *contagemMenu;
            if (IsKeyPressed(KEY_UP)) *selecionado = (*selecionado - 1 + *contagemMenu) % *contagemMenu;

            // Desenha as opções do menu com a seta de seleção
            for (int i = 0; i < *contagemMenu; i++) {
                if (i == *selecionado) {
                    DrawText(">", 20, 148 + i * 81, 50, BLACK);
                }
            }

            // Ação baseada na opção selecionada
            if (IsKeyPressed(KEY_ENTER)) {
                switch (*selecionado) {
                    case 0:
                        StopMusicStream(playlist[musicaAtual]);  // Para a música
                        estado->menu = 1;
                        break; // novo jogo
                    case 1:
                        StopMusicStream(playlist[musicaAtual]);  // Para a música
                        estado->menu = 2;
                        break; // carregar jogo
                    case 2:
                        StopMusicStream(playlist[musicaAtual]);  // Para a música
                        estado->menu = 3;
                        break; // sair
                }
            }
            break;

        case 1: // novo jogo
            if (cutscene(estado, texturas) == 1)
            {
                estado->vidas = 3;
                estado->vidaJogador = 1;
                estado->nivelatual = 1;
                carregaNivel(estado); // reseta o nível
                estado->menu = 7; // continua o jogo
            }
            else
            {
                estado->menu = 1;
            }
            break;

        case 2: // carregar
            carregarEstado("savegame.txt", estado);
            estado->menu = 7; // continua o jogo
            break;

        case 3: // sair
            CloseWindow();
            break;

        case 4: // nova fase
            novaFase(estado, texturas); // função para tela entre fases
            break;



        case 5: // menu de pause
            DrawTexture(texturas[1], 0, 0, WHITE);

            *contagemMenu = 6; // Atualize o número total de opções
            DrawText("Pausado", 400, 100, 40, WHITE);
            DrawText("Continuar", 400, 200, 40, WHITE);
            DrawText("Carregar Jogo", 400, 260, 40, WHITE);
            DrawText("Salvar Jogo", 400, 320, 40, WHITE);
            DrawText("Voltar ao Menu", 400, 380, 40, WHITE);
            DrawText("Volume", 400, 440, 40, WHITE); // Nova opção de volume
            DrawText("Sair", 400, 500, 40, WHITE);

            // Navegação pelo menu de pause
            if (IsKeyPressed(KEY_DOWN)) *selecionado = (*selecionado + 1) % *contagemMenu;
            if (IsKeyPressed(KEY_UP)) *selecionado = (*selecionado - 1 + *contagemMenu) % *contagemMenu;

            for (int i = 0; i < *contagemMenu; i++) {
                if (i == *selecionado) {
                    DrawText(">", 350, 200 + i * 60, 40, WHITE);
                }
            }

            // Exibe e ajusta o volume se a opção de volume estiver selecionada
            if (*selecionado == 4) {
                DrawText(TextFormat("%d%%", (int)(estado->volume * 100)), 600, 440, 40, WHITE); // Mostra o volume atual

                // Ajusta o volume
                if (IsKeyPressed(KEY_RIGHT) && estado->volume < 1.0f) {
                    estado->volume += 0.05f; // Aumenta o volume
                    SetMusicVolume(playlist[musicaAtual], estado->volume); // Aplica o volume à música atual na playlist
                }
                if (IsKeyPressed(KEY_LEFT) && estado->volume > 0.0f) {
                    estado->volume -= 0.05f; // Diminui o volume
                    SetMusicVolume(playlist[musicaAtual], estado->volume); // Aplica o volume à música atual na playlist
                }
            }

            // Ação para alternar o estado do menu com a tecla Tab
            int tab = GetKeyPressed();
            if(tab == 258)estado->menu = 7; // Alterna o estado do menu

            if (IsKeyPressed(KEY_ENTER)) {
                switch (*selecionado) {
                    case 0:
                        estado->menu = 7;
                        *selecionado = 0;
                        break; // continuar jogo
                    case 1:
                        carregarEstado("savegame.txt", estado);
                        *selecionado = 0;
                        break; // carregar jogo
                    case 2:
                        salvarEstado("savegame.txt", estado);
                        *selecionado = 0;
                        break; // salvar jogo
                    case 3:
                        estado->menu = 0;
                        *selecionado = 0;
                        break; // voltar ao menu inicial
                    case 5:
                        estado->menu = 3;
                        *selecionado = 0;
                        break; // sair
                }
            }
            break;


        case 6: // venceu jogo
            if (cutscenefim(estado) == 0)estado->menu = 6; // continua a cutscene
            else{
            DrawTexture(texturas[9], 0, 0, WHITE);
            DrawText("Parabéns, você concluiu a missão!!!", 270, 20, 40, WHITE);
            DrawText("Precione ENTER para voltar ao menu", 800, 570, 20, BLACK);
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
                *contagemMenu = 3;
                DrawTexture(texturas[7], 0, 0, WHITE);
                DrawText("GAME OVER", 360, 160, 40, WHITE);
                DrawText("Voltar ao MENU", 360, 230, 40, WHITE);
                DrawText("Carregar Jogo Salvo", 360, 290, 40, WHITE);
                DrawText("Reiniciar", 360, 350, 40, WHITE);

                // Navegação pelo menu de game over
                if (IsKeyPressed(KEY_DOWN)) *selecionado = (*selecionado + 1) % *contagemMenu;
                if (IsKeyPressed(KEY_UP)) *selecionado = (*selecionado - 1 + *contagemMenu) % *contagemMenu;

                for (int i = 0; i < *contagemMenu; i++) {
                    if (i == *selecionado) {
                        DrawText(">", 320, 230 + i * 60, 45, WHITE);
                    }
                }

                // Ação baseada na opção selecionada
                if (IsKeyPressed(KEY_ENTER)) {
                    TraceLog(LOG_INFO, "Tecla Enter pressionada! Opção selecionada: %d", *selecionado);
                    switch (*selecionado) {
                        case 0: // voltar ao menu inicial
                            estado->derrota = 0;
                            estado->menu = 0;
                            *selecionado = 0;
                            break;
                        case 1: // carregar jogo salvo
                            estado->derrota = 0;
                            estado->menu = 2;
                            *selecionado = 0;
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
    if (estado->musica != 9)estado->musica = estado->menu;
    switch(estado->musica)
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


int main() {
    InitWindow(LARGURA, ALTURA, "Tower Defense");
    SetTargetFPS(60);
    int ultimasteclas[10] = {0};
    int contagemMenu;
    int selecionado = 0;
    int musicaAtual;

    ESTADO estado = {0};
    estado.nivelatual = 1;
    estado.vidas = 3;
    estado.vidaJogador = 1;
    carregaNivel(&estado);
    estado.menu = 0;
    estado.pagina = 0;
    estado.volume = 0.5;

    // Inicializa o dispositivo de áudio
    InitAudioDevice();


    Texture2D texturas[10];
    texturas[0] = LoadTexture("imagemmenu2.png");//imagem rambo menu principal
    texturas[1] = LoadTexture("imagempause.png");//imagem pause
    texturas[2] = LoadTexture("grama.png");//grama
    texturas[3] = LoadTexture("recursos.png");//caixas
    texturas[4] = LoadTexture("buraco.png");//tuneis
    texturas[5] = LoadTexture("arvore.png");//paredes
    texturas[6] = LoadTexture("tutorial.png");//tutorial
    texturas[7] = LoadTexture("bandana.png");
    texturas[8] = LoadTexture("vitoria.png");
    texturas[9] = LoadTexture("fim.png");

    Music playlist[5];
    playlist[0] = LoadMusicStream("musicamenu.mp3");
    playlist[1] = LoadMusicStream("musicafases.mp3");
    playlist[2] = LoadMusicStream("musicacutscene.mp3");
    playlist[3] = LoadMusicStream("musicafim.mp3");
    playlist[4] = LoadMusicStream("rockysong.mp3");


    // Loop principal do jogo
    while (!WindowShouldClose()) {

        selecionaMusica(&estado, &musicaAtual);

        // Toca a música
        PlayMusicStream(playlist[musicaAtual]);

        UpdateMusicStream(playlist[musicaAtual]);

        if (estado.menu == 7)estado = atualizarEstado(estado, ultimasteclas);//se não está pausado, atualiza estads

        BeginDrawing();

        menuControle(&estado, &contagemMenu, &selecionado, playlist, musicaAtual, texturas);//faz as checagens e direcionamentos

        if (estado.vitoria)
        {
            printf("vitoria");
            ClearBackground(BLACK);
            EndDrawing();
            if(estado.qtdRecursos == estado.qtdArmadilhas+estado.recursos)estado.vidas++;//se jogador pegou todos os recursos, base ganha uma vida
            if(estado.atingido == 0)estado.vidaJogador++;//se a base não foi invadida, jogador ganha uma vida
            estado.nivelatual ++;
            int e = estado.nivelatual;
            int v = estado.vidas;
            int vj = estado.vidaJogador;
            memset(&estado, 0, sizeof(estado));
            estado.nivelatual = e;
            estado.vidas = v;
            estado.vidaJogador = vj;
            estado.menu = 4;//passa fase
            if(estado.nivelatual > ULTIMAFASE)//venceu o jogo
            {
                estado.nivelatual = 1;
                estado.menu = 6;
            }
                carregaNivel(&estado);
        }

        if (estado.derrota)
        {
           estado.menu = 8;
        }

        EndDrawing();

    }

    // Fecha o dispositivo de áudio
    CloseAudioDevice();
    UnloadTexture(texturas[0]);
    UnloadTexture(texturas[1]);
    UnloadTexture(texturas[2]);
    UnloadTexture(texturas[3]);
    UnloadTexture(texturas[4]);
    UnloadTexture(texturas[5]);
    UnloadTexture(texturas[6]);
    UnloadTexture(texturas[7]);
    UnloadTexture(texturas[8]);
    UnloadTexture(texturas[9]);


    CloseWindow();
    return 0;
}
