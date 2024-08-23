#ifndef FUNCOES_H
#define FUNCOES_H


//verifica colisao com base na posicao de dois objetos
int verificarColisao(POSICAO a, POSICAO b) {
    return (a.x == b.x && a.y == b.y);
}

//carrega o jogo salvo anteriormente
void carregarEstado(char* filename, ESTADO* estado) {
    FILE* file = fopen(filename, "r+b");
    if (!file) {
        printf("Erro ao abrir arquivo para carregar");
        return;
    }
    fread(estado, sizeof(ESTADO), 1, file);
    fclose(file);
}

//salva o jogo atual
void salvarEstado(char* filename, ESTADO* estado) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Erro ao abrir arquivo para salvar");
        return;
    }
    fwrite(estado, sizeof(ESTADO), 1, file);
    fclose(file);
}


//compara últimas 11 teclas com trapaças
void cheat(int k, int *ultimasteclas, ESTADO* estado)
{
    int codigoTrapaca[11] = {257, 65, 66, 262, 263, 262, 263, 264, 264, 265, 265};
    int rocky[11] = {65, 79, 66, 76, 65, 66, 89, 75, 67, 79, 82};

    //armazena teclas digitadas
    if (k != 0) {
        for (int i = 10; i > 0; i--) {
            ultimasteclas[i] = ultimasteclas[i - 1];
        }
        ultimasteclas[0] = k;
    }

    //se ativou o codigoTrapaca, passa de fase
    if (memcmp(ultimasteclas, codigoTrapaca, sizeof(codigoTrapaca)) == 0) {
        estado->vitoria = 1;
        ultimasteclas[0] = 0;
    }

    //se ativou o rocky, toca a música de Rocky
    if (memcmp(ultimasteclas, rocky, sizeof(codigoTrapaca)) == 0) {
        estado->musica.qualmusica = 9;
        ultimasteclas[0] = 0;
    }

}

//recebe tecla apertada
void getAcao(int k, ESTADO *estado, POSICAO *novaPosJogador)
{
    switch(k)
    {
        case 87:
        case 265:
            novaPosJogador->y -= QUAD_SIZE; //Mover para cima
            break;

        case 83:
        case 264:
            novaPosJogador->y += QUAD_SIZE; //Mover para baixo
            break;

        case 65:
        case 263:
            novaPosJogador->x -= QUAD_SIZE; //Mover para a esquerda
            estado->direcao = -1;
            break;

        case 68:
        case 262:
            novaPosJogador->x += QUAD_SIZE; //Mover para a direita
            estado->direcao = 1;
            break;

        case 71:
            // Colocar armadilha e reduzir armadilhas disponíveis
            if (estado->armadilhasdisp > 0) {
                if ('A' != estado->mapa[novaPosJogador->y/QUAD_SIZE][novaPosJogador->x/QUAD_SIZE]){
                    estado->mapa[novaPosJogador->y/QUAD_SIZE][novaPosJogador->x/QUAD_SIZE]='A';
                    estado->posArmadilhas[estado->qtdArmadilhas].x=novaPosJogador->x;
                    estado->posArmadilhas[estado->qtdArmadilhas].y=novaPosJogador->y;
                    estado->armadilhasdisp--;
                    estado->qtdArmadilhas++;
                }
            }
            break;

        case 258:
            estado->menu = 5; //abre menu de pause
            break;
    }
}

//atualiza posicao dos inimigos
void novaposinimigo(INIMIGO *inimigo)
{
    inimigo->pos.x += inimigo->dx * QUAD_SIZE;
    inimigo->pos.y -= inimigo->dy * QUAD_SIZE;
}

//pegar recursos
void colisaoJogadorRecurso(ESTADO *estado, POSICAO novaPosJogador)
{
    for (int i = 0; i < estado->qtdRecursos; i++) {
        if (verificarColisao(novaPosJogador, estado->posRecursos[i])) {
            estado->armadilhasdisp++;
            estado->posRecursos[i] = (POSICAO){-1, -1}; // Remove recurso do mapa
        }
    }
}

//limitar movimento
void colisaoJogadorParede(char mapa[][MAP_WIDTH], POSICAO *posJogador, POSICAO *novaPosJogador)
{
    if ('W' == mapa[novaPosJogador->y/QUAD_SIZE][novaPosJogador->x/QUAD_SIZE]) {
        *novaPosJogador = *posJogador; // Reverte posicao
    }
}

//utilizar túneis. primeiro checa se há outros além na linha ou na coluna, depois executa conforme
void colisaoJogadorTunel(int k, int qtdTuneis, POSICAO posTuneis[], POSICAO *novaPosJogador)
{
    int pos;
    switch(k)
    {
        //tunel para cima
        case 87:
        case 265:
            pos = ALTURA;
            for (int j = 0; j < qtdTuneis; j++) {
                    if(posTuneis[j].x == novaPosJogador->x && posTuneis[j].y < pos){
                        pos = posTuneis[j].y;
                    }
            }
            if(pos!=ALTURA){
                novaPosJogador->y = pos-QUAD_SIZE;
            } else{
                novaPosJogador->y -= QUAD_SIZE;
            }

            break;

        //tunel para baixo
        case 83:
        case 264:
            pos = 0;
            for (int j = 0; j < qtdTuneis; j++) {
                    if(posTuneis[j].x == novaPosJogador->x && posTuneis[j].y >pos){
                        pos = posTuneis[j].y;
                    }
            }
            if(pos!=0){
                novaPosJogador->y = pos+QUAD_SIZE;
            } else{
                novaPosJogador->y += QUAD_SIZE;
            }
            break;

        //tunel para direita
        case 65:
        case 263:
            pos = LARGURA;
            for (int j = 0; j < qtdTuneis; j++) {
                    if(posTuneis[j].y == novaPosJogador->y && posTuneis[j].x <pos){
                        pos = posTuneis[j].x;
                    }
            }
            if(pos!=LARGURA){
                novaPosJogador->x = pos-QUAD_SIZE;
            } else{
                novaPosJogador->x -= QUAD_SIZE;
            }

            break;

        //tunel para esquerda
        case 68:
        case 262:
            pos = 0;
            for (int j = 0; j < qtdTuneis; j++) {
                    if(posTuneis[j].y == novaPosJogador->y && posTuneis[j].x >pos){
                        pos = posTuneis[j].x;
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

//limita movimento inimigo seguindo uma sequência de tomas de movimento em caso de bloqueios
void colisaoInimigoParede(char mapa[][MAP_WIDTH], int qtdInicialinimigos, INIMIGO inimigo[])
{
    int i, guarda_dx, guarda_dy;
    for (i = 0; i < qtdInicialinimigos; i++){
        if(inimigo[i].vida == 1){
            if ('W' == mapa[inimigo[i].pos.y/QUAD_SIZE][inimigo[i].pos.x/20] || 'H' == mapa[inimigo[i].pos.y/QUAD_SIZE][inimigo[i].pos.x/QUAD_SIZE])
            {
                inimigo[i].pos.x -= inimigo[i].dx * QUAD_SIZE;
                inimigo[i].pos.y += inimigo[i].dy * QUAD_SIZE;
                guarda_dx=inimigo[i].dx;
                guarda_dy=inimigo[i].dy;

                if(inimigo[i].dx != 0)
                {
                    inimigo[i].dx=0;
                    inimigo[i].dy=1;
                    novaposinimigo(&inimigo[i]);

                    if ('W' == mapa[inimigo[i].pos.y/QUAD_SIZE][inimigo[i].pos.x/QUAD_SIZE] || 'H' == mapa[inimigo[i].pos.y/QUAD_SIZE][inimigo[i].pos.x/QUAD_SIZE])
                    {
                        inimigo[i].pos.y += inimigo[i].dy * QUAD_SIZE;
                        inimigo[i].dy= -1;
                        novaposinimigo(&inimigo[i]);

                        if('W' == mapa[inimigo[i].pos.y/QUAD_SIZE][inimigo[i].pos.x/QUAD_SIZE] || 'H' == mapa[inimigo[i].pos.y/QUAD_SIZE][inimigo[i].pos.x/QUAD_SIZE])
                        {
                            inimigo[i].pos.y += inimigo[i].dy * QUAD_SIZE;
                            inimigo[i].dy=0;
                            inimigo[i].dx= -guarda_dx;
                            novaposinimigo(&inimigo[i]);
                        }
                    }
                }
                else if (inimigo[i].dy != 0)
                {
                    inimigo[i].dy=0;
                    inimigo[i].dx=-1;
                    novaposinimigo(&inimigo[i]);

                    if ('W' == mapa[inimigo[i].pos.y/QUAD_SIZE][inimigo[i].pos.x/QUAD_SIZE] || 'H' == mapa[inimigo[i].pos.y/QUAD_SIZE][inimigo[i].pos.x/QUAD_SIZE])
                    {
                        inimigo[i].pos.x -= inimigo[i].dx * QUAD_SIZE;
                        inimigo[i].dx=1;
                        novaposinimigo(&inimigo[i]);

                        if('W' == mapa[inimigo[i].pos.y/QUAD_SIZE][inimigo[i].pos.x/QUAD_SIZE] || 'H' == mapa[inimigo[i].pos.y/QUAD_SIZE][inimigo[i].pos.x/QUAD_SIZE])
                        {
                            inimigo[i].pos.x -= inimigo[i].dx * QUAD_SIZE;
                            inimigo[i].dx=0;
                            inimigo[i].dy= -guarda_dy;
                            novaposinimigo(&inimigo[i]);
                        }
                    }
                }
            }
        }
    }
}

//mata inimigo e remove armadilha
void colisaoArmadilhaInimigo(char mapa[][MAP_WIDTH], int qtdInicialinimigos, int *qtdInimigos, INIMIGO inimigo[])
{
    for (int j = 0; j < qtdInicialinimigos; j++)
    {
        if(inimigo[j].vida ==1)
        {
            if(('A' == mapa[inimigo[j].pos.y/QUAD_SIZE][inimigo[j].pos.x/QUAD_SIZE]))
            {
                mapa[inimigo[j].pos.y/QUAD_SIZE][inimigo[j].pos.x/QUAD_SIZE] = ' ';
                *qtdInimigos=*qtdInimigos-1;
                inimigo[j].vida=0;
            }
        }
    }
}

//reduz vida da torre e mata inimigo
void colisaoInimigoTorre(ESTADO *estado)
{
     for (int i = 0; i < estado->qtdInicialinimigos; i++) {
        if(estado->inimigo[i].vida == 1){
            if (verificarColisao(estado->inimigo[i].pos, estado->posBase)) {
                estado->inimigo[i].vida=0;
                estado->vidas--;
                estado->qtdInimigos--;
                estado->atingido = 1; //registra que torre foi atingida
            }
        }
    }
}

//mata inimigo e reduz vida do jogador em 1
void colisaoJogadorInimigo(INIMIGO inimigo[], int qtdInicialinimigos, int *vidaJogador, int *qtdInimigos, POSICAO novaPosJogador)
{
    for (int i = 0; i < qtdInicialinimigos; i++) {
        if(inimigo[i].vida==1){
            if (verificarColisao(inimigo[i].pos, novaPosJogador))
            {
                *vidaJogador= *vidaJogador-1;
                *qtdInimigos= *qtdInimigos-1;
                inimigo[i].vida = 0;
            }
        }
    }

}

//recebe inputs, atualiza posicoes e chama verificacoes
void atualizarEstado(ESTADO *estado, int ultimasteclas[]) {
    estado->tempo++;
    POSICAO novaPosJogador = estado->posJogador;
    int i; //laços
    int k = GetKeyPressed(); //inputs

    if(estado->menu == 7)cheat(k, ultimasteclas, estado);//checa últimas 11 teclas paa ver se trapaca ativou

    getAcao(k, estado, &novaPosJogador);

    colisaoJogadorInimigo(estado->inimigo, estado->qtdInicialinimigos, &estado->vidaJogador, &estado->qtdInimigos, novaPosJogador);

    //Atualiza posicoes dos inimigos vivos
    if(estado->tempo % (60/VELOCIDADE) == 0) {
        for (i = 0; i < estado->qtdInicialinimigos; i++){
            if(estado->inimigo[i].vida == 1) novaposinimigo(&estado->inimigo[i]);
        }
    }

    colisaoJogadorRecurso(estado, novaPosJogador);

    if ('H' == estado->mapa[novaPosJogador.y/QUAD_SIZE][novaPosJogador.x/QUAD_SIZE]){
        colisaoJogadorTunel(k, estado->qtdTuneis, estado->posTuneis, &novaPosJogador);
    }

    colisaoJogadorParede(estado->mapa, &estado->posJogador, &novaPosJogador);

    //repete apenas quando inimigo movimenta
    if(estado->tempo % (60/VELOCIDADE) == 0)
    {
        colisaoInimigoParede(estado->mapa, estado->qtdInicialinimigos, estado->inimigo);
        colisaoArmadilhaInimigo(estado->mapa, estado->qtdInicialinimigos, &estado->qtdInimigos, estado->inimigo);
        colisaoInimigoTorre(estado);
    }

    //checagens para ver se jogador ou a torre perderam todas as vidas ou todos os inimigos foram derrotados
    if (estado->vidas <= 0 || estado->vidaJogador <= 0)
        estado->derrota = 1;
    if (estado->qtdInimigos == 0)
        estado->vitoria = 1;

    estado->posJogador = novaPosJogador;
}

//converte o arquivo txt do mapa da fase atual em uma matriz
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

    int i, y, j, x; //laços

    // Inicializa valores de estado
    estado->qtdInimigos = 0;
    estado->qtdRecursos = 0;
    estado->qtdTuneis = 0;
    estado->qtdParedes = 0;
    estado->qtdArmadilhas = 0;
    estado->vitoria = 0;
    estado->derrota = 0;
    estado->tempo = 0;
    estado->armadilhasdisp = 0;
    estado->qtdInicialinimigos =0;

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

    char linha[MAP_WIDTH + 2]; // +2 para o '\n' e o '\0'
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
                    estado->inimigo[estado->qtdInimigos].pos = pos;
                    estado->inimigo[estado->qtdInimigos].dx= -1;
                    estado->inimigo[estado->qtdInimigos].dy= 0;
                    estado->inimigo[estado->qtdInimigos].vida=1;
                    estado->qtdInimigos++;
                    estado->qtdInicialinimigos++;

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

//denha o jogo
void desenho(ESTADO *estado, Texture2D texturas[])
{
        //desenha fundo
        DrawTexture(texturas[2], 0, 0, WHITE);

        char text[100];
        sprintf(text, "Fase atual: %d    Vidas da torre: %d   Vidas do jogador: %d    Armadilhas: %d   Inimigos restantes: %d",
               estado->nivelatual, estado->vidas, estado->vidaJogador, estado->armadilhasdisp, estado->qtdInimigos);

        //desenha inimigos mortos
        for (int i = 0; i < estado->qtdInicialinimigos; i++)
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

        //desenha inimigos vivos
        for (int i = 0; i < MAX_INIMIGOS; i++)
        {
            if(estado->inimigo[i].vida == 1)DrawTexture(texturas[11], estado->inimigo[i].pos.x, estado->inimigo[i].pos.y, WHITE);
        }

        //desenha armadilhas
        for (int i = 0; i < estado->qtdArmadilhas; i++) {
            if(('A' == estado->mapa[estado->posArmadilhas[i].y/QUAD_SIZE][estado->posArmadilhas[i].x/QUAD_SIZE])) {
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

        //desenha texto com informações para o jogador
        DrawText(text, 9, 3, 20, WHITE);
}

//tela entre as fases
void novaFase(ESTADO *estado, Texture2D texturas[])
{
    DrawTexture(texturas[8], 0, 0, WHITE);
    char text1[50];
    sprintf(text1, "Rambo resistiu a onda %d de inimigos!", estado->nivelatual-1);
    DrawText(text1, 240, 20, 40, WHITE);
    DrawText("pressione ENTER para continuar", 850, 570, 20, WHITE);
    if(IsKeyPressed(KEY_ENTER))estado->menu = 7;//continua
}

//texto, tutorial e dicas do começo do jogo
int cutsceneinicial(ESTADO *estado, Texture2D texturas[])
{
    ClearBackground(BLACK);

    char *frase1 = "\n\n\n   Nas entranhas das linhas inimigas, um ponto vital foi \n\n\n             capturado pelas tropas dos EUA.";
    char *frase2 = " Fatalmente, a posicao desta instalacao foi comprometida.\n\n\n        Unidades inimigas foram detectadas ao Leste.";
    char *frase3 = "  Ela precisara resistir ate a chegada de novas tropas.";
    char *frase4 = "   Agora, resta apenas um homem que pode defende-la.";
    char *avanca = "pressione ENTER para continuar";

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

        //próximo caso se enter for precionada
        if (IsKeyPressed(KEY_ENTER))
        {
            estado->pagina++;
        }
        return 0;
    }

//texto em caso de derrota
int cutscenederrota(ESTADO *estado)
{
    ClearBackground(BLACK);

    char *frase5 = "A base americana nao resistiu as ofensivas inimigas.";
    char *frase6 = "Os homens que a protegiam foram levados como\n\n\n                      prisioneiros.";
    char *frase7 = "O antigo Boina Verde John Rambo nunca foi encontrado.";
    char *avanca = "pressione ENTER para continuar";

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

        //próximo caso se enter for precionada
        if (IsKeyPressed(KEY_ENTER))
        {
            estado->pagina++;
        }
    return 0;
}

//texto em caso de vitória
int cutscenefim(ESTADO *estado)
{
    ClearBackground(BLACK);

    char *frase8 = "Apesar de sucessivas incursoes inimigas, a guarnicao\n\n\n                 americana manteve-se de pe.";
    char *frase9 = " O antigo Boina Verde John Rambo foi mais uma vez\n\n\nreconhecido por sua bravura e sua deteminacao no\n\n\n                      campo de batalha.";
    char *frase10 = "Ele, enfim, pode voltar para casa.";
    char *avanca = "pressione ENTER para continuar";

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

        //próximo caso se enter for precionada
        if (IsKeyPressed(KEY_ENTER))
        {
            estado->pagina++;
        }
    return 0;
}

//função que checa estado atual do jogo, se está em algum menu ou jogando
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

            for (int i = 0; i < MENU_INICIO; i++) {
                if (i == *selecionado) {
                    DrawText(">", 20, 148 + i * 81, 50, BLACK);
                }
            }

            //direciona conforme escolhido
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
            if (cutsceneinicial(estado, texturas) == 1) //faz cutscene, quando ela acabar, continua
            {
                estado->vidas = 3;
                estado->vidaJogador = 1;
                estado->nivelatual = 1;
                carregarMapaDeArquivo(estado); //reseta o nível
                estado->menu = 7; //continua o jogo
            }
            else
            {
                estado->menu = 1; //mantém na cutscene
            }
            break;

        case 4: // nova fase
            novaFase(estado, texturas); //função para tela entre fases
            break;

        case 5: // menu de pause
            DrawTexture(texturas[1], 0, 0, WHITE);

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

            //muda voluem da música com setas no menu
            if (*selecionado == 4) {
                DrawText(TextFormat("%d%%", (int)(estado->musica.volume * 100)), 600, 440, 40, WHITE);

                if (IsKeyPressed(KEY_RIGHT) && estado->musica.volume < 1.0f) {
                    estado->musica.volume += 0.05f; // Aumenta o volume
                    SetMusicVolume(playlist[musicaAtual], estado->musica.volume);
                }
                if (IsKeyPressed(KEY_LEFT) && estado->musica.volume > 0.0f) {
                    estado->musica.volume -= 0.05f; // Diminui o volume
                    SetMusicVolume(playlist[musicaAtual], estado->musica.volume);
                }
            }

            //sai do menu de pause
            int tab = GetKeyPressed();
            if(tab == 258)estado->menu = 7; // Alterna o estado do menu

            //direciona conforme escolhido
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

                //direciona conforme escolhido
                if (IsKeyPressed(KEY_ENTER)) {
                    TraceLog(LOG_INFO, "Tecla Enter pressionada! Opcao selecionada: %d", *selecionado);
                    switch (*selecionado) {
                        case 0: //voltar ao menu inicial
                            estado->derrota = 0;
                            estado->menu = 0;
                            *selecionado = 0;
                            break;
                        case 1: //carregar jogo
                            estado->derrota = 0;
                            carregarEstado("save/savegame.bin", estado);
                            *selecionado = 0;
                            estado->menu = 7;
                            break;
                        case 2: //reiniciar jogo
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

//adequa musica ao estado atual
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
        case 8:
            *musicaAtual = 2;
            break;
        case 9:
            *musicaAtual = 4;
            break;

    }
}

//inicializa variaveis
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
    texturas[7] = LoadTexture("texturas/bandana.png"); //imagem derrota
    texturas[8] = LoadTexture("texturas/vitoria.png"); //imagem passa de fase
    texturas[9] = LoadTexture("texturas/fim.png"); //imagem final do jogo
    texturas[10] = LoadTexture("texturas/johnrambo.png"); //Rambo olhando mpar a direita
    texturas[11] = LoadTexture("texturas/inimigo.png"); //"monstros"
    texturas[12] = LoadTexture("texturas/base.png"); //torre
    texturas[13] = LoadTexture("texturas/johnrambo2.png"); //Rambo olhando para esquerda
    texturas[14] = LoadTexture("texturas/tutorial2.png"); //dicas

    playlist[0] = LoadMusicStream("musicas/musicamenu.mp3");
    playlist[1] = LoadMusicStream("musicas/musicafases.mp3");
    playlist[2] = LoadMusicStream("musicas/musicacutscene.mp3");
    playlist[3] = LoadMusicStream("musicas/musicafim.mp3");
    playlist[4] = LoadMusicStream("musicas/rockysong.mp3");


}

//avcança de fase
void vitoria(ESTADO* estado, Texture2D* texturas, Music* playlist){
    ClearBackground(BLACK);
    EndDrawing();
    if(estado->qtdRecursos == estado->qtdArmadilhas+estado->armadilhasdisp)estado->vidas++;//se jogador pegou todos os recursos, base ganha uma vida
    if(estado->atingido == 0)estado->vidaJogador++;//se a base não foi invadida, jogador ganha uma vida
    estado->nivelatual ++;

    //registra o que for necessario do estado
    int e = estado->nivelatual;
    int v = estado->vidas;
    int vj = estado->vidaJogador;

    //zera estado
    memset(estado, 0, sizeof(ESTADO));

    //preenche com o que foi salvo
    estado->nivelatual = e;
    estado->vidas = v;
    estado->vidaJogador = vj;
    estado->menu = 4;

    //se passou a última fase finaliza partida
    if(estado->nivelatual > ULTIMAFASE)//venceu o jogo
    {
        estado->nivelatual = 1;
        estado->menu = 6;
    }
    carregarMapaDeArquivo(estado);
}


#endif
