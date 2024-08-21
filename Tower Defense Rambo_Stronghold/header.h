#ifndef HEADER_H
#define HEADER_H

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

typedef struct Musica{
    int qualmusica;
    float volume;

}MUSICA;

// Defini  o da estrutura Estado
typedef struct Estado {
    int vidas;
    int vidaJogador;
    int qtdInimigos;
    int qtdRecursos;
    int qtdArmadilhas;
    int qtdTuneis;
    int qtdParedes;
    POSICAO posRecursos[MAX_RECURSOS];
    POSICAO posTuneis[MAX_PORTAIS];
    POSICAO posParedes[MAX_PAREDES];
    POSICAO posJogador;
    POSICAO posArmadilhas[MAX_RECURSOS];
    POSICAO posBase;
    char mapa[MAP_HEIGHT][MAP_WIDTH];
    int vitoria;
    int derrota;
    int tempo;
    int armadilhasdisp;
    int nivelatual;
    int menu;
    MUSICA musica;
    int pagina;
    INIMIGO inimigo[MAX_INIMIGOS];
    int atingido;
    int direcao;
}ESTADO;


int verificarColisao(POSICAO a, POSICAO b);//verifica colisao com base na posicao de dois objetos
void carregarEstado(char* filename, ESTADO* estado);//carrega o jogo salvo anteriormente
void salvarEstado(char* filename, ESTADO* estado); //salva o jogo atual
void cheat(int k, int *ultimasteclas, ESTADO* estado);//registra ultimas teclas e usa trapaçcs
void getAcao(int k, ESTADO *estado, POSICAO *novaPosJogador); //trata inputs para movimentacao e pause
void novaposinimigo(INIMIGO *inimigo);//atualiza posicao dos inimigos

//funcoes para cada caso de colisao
void colisaoJogadorRecurso(ESTADO *estado, POSICAO novaPosJogador);
void colisaoJogadorParede(ESTADO *estado, POSICAO *novaPosJogador);
void colisaoJogadorTunel(int k, ESTADO *estado, POSICAO *novaPosJogador);
void colisaoInimigoParede(ESTADO *estado);
void colisaoArmadilhaInimigo(ESTADO *estado);
void colisaoInimigoTorre(ESTADO *estado);
void colisaoJogadorInimigo(ESTADO *estado, POSICAO novaPosJogador);


ESTADO atualizarEstado(ESTADO estado, int ultimasteclas[]); //recebe inputs, atualiza posicoes e chama verificacoes
void carregarMapaDeArquivo(ESTADO* estado); //converte o arquivo txt em uma matriz
void desenho(ESTADO *estado, Texture2D texturas[]);
void novaFase(ESTADO *estado, Texture2D texturas[]);//funcao para tela entre fases
int cutsceneinicial(ESTADO *estado, Texture2D texturas[]); //roda cutscene inicial
int cutscenederrota(ESTADO *estado); //roda cutscene derrota
int cutscenefim(ESTADO *estado); //roda cutscene fim
void menuControle(ESTADO *estado, int *selecionado, Music playlist[], int musicaAtual, Texture2D texturas[]); // checa condicao do jogo a cada ciclo, jogando, menus
void selecionaMusica(ESTADO *estado, int *musicaAtual); //adequa musica ao estado atual
void inicializa(ESTADO* estado, Texture2D* texturas, Music* playlist);
void vitoria(ESTADO* estado, Texture2D* texturas, Music* playlist);
#endif
