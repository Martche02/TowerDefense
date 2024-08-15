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
#define MAX_PAREDES 1000
#define MAX_PORTAIS 100
#define ULTIMAFASE 4

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
    int menu;
    int pagina;
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

int cheat(int k, int *ultimasteclas, ESTADO* estado)
{
    int códigoTrapaca[11] = {257, 65, 66, 262, 263, 262, 263, 264, 264, 265, 265};

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
    //printf("%d\n", ultimasteclas[0]);
    // Compara as duas arrays para verificar se o Konami Code foi inserido
    if (memcmp(ultimasteclas, códigoTrapaca, sizeof(códigoTrapaca)) == 0) {
        estado->vitoria = true;
        ultimasteclas[0] = 0;
    }

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
        case 258:
            estado->menu = 5;
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
ESTADO atualizarEstado(int k, ESTADO estado) {
    estado.tempo++;
    Vector2 novaPosJogador = estado.posJogador;
    Vector2 novaPosMonstros[MAX_MONSTROS];

    getAcao(k, &estado, &novaPosJogador);

    novasposMonstros(&estado, novaPosMonstros);

    colisaoJogadorRecurso(&estado, novaPosJogador);

    colisaoJogadorPortal(k, &estado, &novaPosJogador);

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
    int dentroDosLimites(int x, int y) {
        return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
    }

    // Função auxiliar para verificar se a posição já está na trilha
    int jaNaTrilha(Vector2 pos, Vector2* trilha, int comprimentoTrilha) {
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
        int encontrado = false;
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
    printf("%s", mapa);
    carregarMapaDeArquivo(estado, mapa);
}

void desenhaRambo(ESTADO *estado) {
    // Definindo as cores
    Color MARROM = (Color){100, 60, 0, 255};
    Color PELE = {204, 153, 102, 255};

    // Desenhando o boneco com 4 retângulos
    DrawRectangleV((Vector2){estado->posJogador.x, estado->posJogador.y}, (Vector2){QUAD_SIZE, 4}, BLACK);  // Parte preta
    DrawRectangleV((Vector2){estado->posJogador.x, estado->posJogador.y + 4}, (Vector2){QUAD_SIZE, 3}, RED);    // Parte vermelha
    DrawRectangleV((Vector2){estado->posJogador.x, estado->posJogador.y + 7}, (Vector2){QUAD_SIZE, 6}, PELE);   // Parte de pele
    DrawRectangleV((Vector2){estado->posJogador.x, estado->posJogador.y + 13}, (Vector2){QUAD_SIZE, 7}, MARROM); // Parte marrom escuro

}

void desenho(ESTADO *estado)
{
        Texture2D grama = LoadTexture("grama.png");
        DrawTexture(grama, 0, 0, WHITE);

        char text[100];
        sprintf(text, "Fase atual: %d    Armadilhas: %d    Vidas da torre: %d   Vidas do jogador: 1   Inimigos restantes: %d",
               estado->nivelatual, estado->recursos, estado->vidas, estado->qtdMonstros);

        DrawRectangleV(estado->posBase, (Vector2){QUAD_SIZE, QUAD_SIZE}, WHITE);

        for (int i = 0; i < estado->qtdMonstros; i++) {
            DrawRectangleV(estado->posMonstros[i], (Vector2){QUAD_SIZE, QUAD_SIZE}, RED);
        }

        //desenha recursos
        Texture2D recursos = LoadTexture("recursos.png");
        for (int i = 0; i < estado->qtdFrutinhas; i++) {
            DrawTexture(recursos, estado->posFrutinhas[i].x, estado->posFrutinhas[i].y, WHITE);
        }

        //desenha armadilhas
        for (int i = 0; i < MAX_FRUTAS; i++) {
            if (estado->posArmadilhas[i].x != 0 && estado->posArmadilhas[i].y != 0) {
                DrawRectangle(estado->posArmadilhas[i].x + 8, estado->posArmadilhas[i].y, 4, QUAD_SIZE, DARKGRAY);
                DrawRectangle(estado->posArmadilhas[i].x , estado->posArmadilhas[i].y + 8, QUAD_SIZE, 4, DARKGRAY);
            }
        }

        //desenha jogador
        desenhaRambo(estado);

        //desenha buracos
        Texture2D buraco = LoadTexture("buraco.png");
        for (int i = 0; i < estado->qtdPortais; i++) {
            DrawTexture(buraco, estado->posPortais[i].x, estado->posPortais[i].y, WHITE);

        }

        //desenho barreiras
        Texture2D arvores = LoadTexture("arvore.png");
        for (int i = 0; i < estado->qtdParedes; i++) {
            DrawTexture(arvores, estado->posParedes[i].x, estado->posParedes[i].y, WHITE);
        }

        DrawText(text, 10, 3, 19, WHITE);
}

void novaFase(ESTADO *estado)//funcao para tela entre fases
{
    ClearBackground(BLACK);
    DrawText("Passou de fase!\n\n\n\nAperte ENTER para continuar", 250, 150, 40, WHITE);
    if (IsKeyPressed(KEY_ENTER))estado->menu = 7;//continua
}

int cutscene(ESTADO *estado)
{
    //Inicializa a tela para a cutscene
    ClearBackground(BLACK);


    // Texto a ser exibido
    const char *frase1 = "\n\n\n   Nas entranhas das linhas inimigas, um ponto vital foi \n\n\n             capturado pelas tropas dos EUA.";
    const char *frase2 = " Fatalmente, a posição desta instalação foi comprometida.\n\n\n        Unidades inimigas foram detectadas ao Leste.";
    const char *frase3 = "  Ela precisará resistir até a chegada de novas tropas.";
    const char *frase4 = "   Agora, resta apenas um homem que pode defendê-la.";

    // pagina atual da cutscene

        switch(estado->pagina)
        {
            case 0:
                break;

            case 1:
                DrawText(frase1, 10, 10, 40, WHITE);
                break;

            case 2:
                DrawText(frase1, 10, 10, 40, WHITE);
                DrawText(frase2, 10, 190, 40, WHITE);
                break;

            case 3:
                DrawText(frase1, 10, 10, 40, WHITE);
                DrawText(frase2, 10, 190, 40, WHITE);
                DrawText(frase3, 10, 320, 40, WHITE);
                break;

             case 4:
                DrawText(frase1, 10, 10, 40, WHITE);
                DrawText(frase2, 10, 190, 40, WHITE);
                DrawText(frase3, 10, 320, 40, WHITE);
                DrawText(frase4, 10, 400, 40, WHITE);
                break;

             case 5:
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


void menuControle(ESTADO *estado, int *contagemMenu, int *selecionado, Music playlist[], int musicaAtual) // checa condicao a cada ciclo
{
    switch (estado->menu)
    {
        case 0: // menu inicio
            *contagemMenu = 3; // número de opções no menu inicial
            Texture2D imagemmenu = LoadTexture("imagemmenu2.png");
            DrawTexture(imagemmenu, 0, 0, WHITE);

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
                        UnloadTexture(imagemmenu); // Descarrega a textura após o uso
                        StopMusicStream(playlist[musicaAtual]);  // Para a música
                        estado->menu = 1;
                        break; // novo jogo
                    case 1:
                        UnloadTexture(imagemmenu); // Descarrega a textura após o uso
                        StopMusicStream(playlist[musicaAtual]);  // Para a música
                        estado->menu = 2;
                        break; // carregar jogo
                    case 2:
                        UnloadTexture(imagemmenu); // Descarrega a textura após o uso
                        StopMusicStream(playlist[musicaAtual]);  // Para a música
                        estado->menu = 3;
                        break; // sair
                }
            }
            break;

        case 1: // novo jogo
            if (cutscene(estado) == 1)
            {
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
            novaFase(estado); // função para tela entre fases
            break;

        case 5: // menu de pause

            Texture2D imagempause = LoadTexture("imagempause.png");
            DrawTexture(imagempause, 0, 0, WHITE);

            *contagemMenu = 5;
            DrawText("Pausado", 400, 100, 40, WHITE);
            DrawText("Continuar", 400, 200, 40, WHITE);
            DrawText("Carregar Jogo", 400, 260, 40, WHITE);
            DrawText("Salvar Jogo", 400, 320, 40, WHITE);
            DrawText("Voltar ao Menu", 400, 380, 40, WHITE);
            DrawText("Sair", 400, 440, 40, WHITE);

            // Navegação pelo menu de pause
            if (IsKeyPressed(KEY_DOWN)) *selecionado = (*selecionado + 1) % *contagemMenu;
            if (IsKeyPressed(KEY_UP)) *selecionado = (*selecionado - 1 + *contagemMenu) % *contagemMenu;

            for (int i = 0; i < *contagemMenu; i++) {
                if (i == *selecionado) {
                    DrawText(">", 350, 200 + i * 60, 40, WHITE);
                }
            }

            // Ação baseada na opção selecionada
            if (IsKeyPressed(KEY_ENTER)) {
                switch (*selecionado) {
                    case 0:
                        estado->menu = 7;
                        UnloadTexture(imagempause); // Descarrega a textura após o uso
                        break; // continuar jogo
                    case 1:
                        carregarEstado("savegame.txt", estado);
                        UnloadTexture(imagempause); // Descarrega a textura após o uso
                        break; // carregar jogo
                    case 2:
                        salvarEstado("savegame.txt", estado);
                        UnloadTexture(imagempause); // Descarrega a textura após o uso
                        break; // salvar jogo
                    case 3:
                        estado->menu = 0;
                        UnloadTexture(imagempause); // Descarrega a textura após o uso
                        break; // voltar ao menu inicial
                    case 4:
                        estado->menu = 3;
                        UnloadTexture(imagempause); // Descarrega a textura após o uso
                        break; // sair
                }
            }
            break;

        case 6: // venceu jogo
            ClearBackground(BLACK);
            DrawText("Parabéns!!!\n\n\n\nAperte ENTER para voltar ao MENU", 300, 200, 40, WHITE);
            if (IsKeyPressed(KEY_ENTER)) estado->menu = 0; // retorna ao menu inicial
            break;

        case 7: // operando normalmente
            desenho(estado);
            break;

        case 8: // game over
            *contagemMenu = 3;
            ClearBackground(BLACK);
            DrawText("GAME OVER", 300, 200, 40, WHITE);
            DrawText("Voltar ao MENU", 300, 270, 40, WHITE);
            DrawText("Carregar Jogo Salvo", 300, 330, 40, WHITE);
            DrawText("Reiniciar", 300, 390, 40, WHITE);

            // Navegação pelo menu de game over
            if (IsKeyPressed(KEY_DOWN)) *selecionado = (*selecionado + 1) % *contagemMenu;
            if (IsKeyPressed(KEY_UP)) *selecionado = (*selecionado - 1 + *contagemMenu) % *contagemMenu;

            for (int i = 0; i < *contagemMenu; i++) {
                if (i == *selecionado) {
                    DrawText(">", 250, 270 + i * 60, 40, WHITE);
                }
            }

            // Ação baseada na opção selecionada
            if (IsKeyPressed(KEY_ENTER)) {
                switch (*selecionado) {
                    case 0: estado->menu = 0; break; // voltar ao menu inicial
                    case 1: estado->menu = 2; break; // carregar jogo salvo
                    case 2: estado->menu = 1; break; // reiniciar jogo
                }
            }
            break;

    }
}

selecionaMusica(ESTADO *estado, int *musicaAtual)
{
    switch(estado->menu)
    {
        case 0:
            *musicaAtual = 0;
            break;
        case 1:
            *musicaAtual = 2;
            break;
        case 7:
            *musicaAtual = 1;
            break;
    }
}


int main() {
    InitWindow(LARGURA, ALTURA, "Tower Defense");
    SetTargetFPS(60);
    int ultimasteclas[10] = {0};
    int contagemMenu;
    int selecionado;
    int musicaAtual;

    ESTADO estado = {0};
    estado.nivelatual = 1;
    carregaNivel(&estado);
    estado.menu = 0;

    // Inicializa o dispositivo de áudio
    InitAudioDevice();

    Music playlist[10];

    playlist[0] = LoadMusicStream("musicamenu.mp3");
    playlist[1] = LoadMusicStream("musicafases.mp3");
    playlist[2] = LoadMusicStream("musicacutscene.mp3");


    // Loop principal do jogo
    while (!WindowShouldClose()) {

        selecionaMusica(&estado, &musicaAtual);
        printf("%d: musica atual", musicaAtual);

        // Toca a música
        PlayMusicStream(playlist[musicaAtual]);

        UpdateMusicStream(playlist[musicaAtual]);

        int k = GetKeyPressed();

        if (estado.menu != 5)estado = atualizarEstado(k, estado);//se não está pausado, atualiza estads

        cheat(k, ultimasteclas, &estado);//checa últimas 11 teclas pra ver se trapaça funcionou

        BeginDrawing();

        menuControle(&estado, &contagemMenu, &selecionado, playlist, musicaAtual);//faz as checagens e direcionamentos

        if (estado.vitoria)
        {
            printf("vitoria");
            ClearBackground(BLACK);
            EndDrawing();
            estado.nivelatual ++;
            int e = estado.nivelatual;
            memset(&estado, 0, sizeof(estado));
            estado.nivelatual = e;
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
           printf("derrota");
           estado.menu = 8;
           carregaNivel(&estado);

        }

        EndDrawing();

    }

    // Fecha o dispositivo de áudio
    CloseAudioDevice();

    CloseWindow();
    return 0;
}
