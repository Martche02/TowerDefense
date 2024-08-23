#ifndef HEADER_H
#define HEADER_H

//estrutura posições
typedef struct Posicao{
    int x; //posição horizontal
    int y; //posição vertical
} POSICAO;

//estrutura inimigos
typedef struct inimigo{
    int dx; //esquerda ou direita
    int dy; //cima ou baixo
    int vida; //vivo (1) morto (0)
    POSICAO pos; //posição
} INIMIGO;

//estrutura música
typedef struct Musica{
    int qualmusica;
    float volume;
}MUSICA;

//estrutura geral, carregada como estado salvo
typedef struct Estado {
    int vidas; //vidas da base
    int vidaJogador; //vidas do jogador
    int qtdInimigos; //quantidade de inimigos vivos
    int qtdInicialinimigos; //quantidade de inimigos inicializados
    int qtdRecursos; //quantidade de recursos inicial
    int qtdArmadilhas; //quantidade de recursos pegos
    int armadilhasdisp; //quantidade armadilhas disponíveis para armar
    int qtdTuneis; //quantidade portais no mapa
    int qtdParedes; //quantidade de paredes no mapa
    POSICAO posRecursos[MAX_RECURSOS]; //posição dos recursos
    POSICAO posTuneis[MAX_TUNEIS]; //posição dos buracos
    POSICAO posParedes[MAX_PAREDES]; //posição das paredes
    POSICAO posJogador; //posição jogadors
    POSICAO posArmadilhas[MAX_RECURSOS]; //poisção armadilhas
    POSICAO posBase; //posição base
    char mapa[MAP_HEIGHT][MAP_WIDTH]; //matriz do mapa
    int vitoria; //0 neutro 1 para vitória
    int derrota; //0 para neutro 1 para derrota
    int tempo; //marca intervalos para deslocamneto dos inimigos
    int nivelatual; //número fase atual
    int menu; //utilizada para verificar em qual etapa está o jogo (menus, jogando, cutscenes)
    MUSICA musica; //estrutura das musicas
    int pagina; //marca etapa das cutscenes
    INIMIGO inimigo[MAX_INIMIGOS]; //arranjo de inimigos
    int atingido; //0 neutro quando torre é atingida =1
    int direcao; //checa se jogador está para direita ou esquerda para desenho
}ESTADO;


int verificarColisao(POSICAO a, POSICAO b);//verifica colisao com base na posicao de dois objetos
void carregarEstado(char* filename, ESTADO* estado);//carrega o jogo salvo anteriormente
void salvarEstado(char* filename, ESTADO* estado); //salva o jogo atual
void cheat(int k, int *ultimasteclas, ESTADO* estado);//registra ultimas teclas e usa trapaças
void getAcao(int k, ESTADO *estado, POSICAO *novaPosJogador); //trata inputs para movimentacao e pause
void novaposinimigo(INIMIGO *inimigo);//atualiza posicao dos inimigos

//funcoes para cada caso de colisao
void colisaoJogadorRecurso(ESTADO *estado, POSICAO novaPosJogador);
void colisaoJogadorParede(char mapa[][MAP_WIDTH], POSICAO *posJogador, POSICAO *novaPosJogador);
void colisaoJogadorTunel(int k, int qtdTuneis, POSICAO posTuneis[], POSICAO *novaPosJogador);
void colisaoInimigoParede(char mapa[][MAP_WIDTH], int qtdInicialinimigos, INIMIGO inimigo[]);
void colisaoArmadilhaInimigo(char mapa[][MAP_WIDTH], int qtdInicialinimigos, int *qtdInimigos, INIMIGO inimigo[]);
void colisaoInimigoTorre(ESTADO *estado);
void colisaoJogadorInimigo(INIMIGO inimigo[], int qtdInicialinimigos, int *vidaJogador, int *qtdInimigos, POSICAO novaPosJogador);

void atualizarEstado(ESTADO *estado, int ultimasteclas[]); //recebe inputs, atualiza posicoes e chama verificacoes
void carregarMapaDeArquivo(ESTADO* estado); //converte o arquivo txt em uma matriz
void desenho(ESTADO *estado, Texture2D texturas[]); //desenha jogo em andamento
void novaFase(ESTADO *estado, Texture2D texturas[]);//funcao para tela entre fases
int cutsceneinicial(ESTADO *estado, Texture2D texturas[]); //roda cutscene inicial
int cutscenederrota(ESTADO *estado); //roda cutscene derrota
int cutscenefim(ESTADO *estado); //roda cutscene fim
void verificaEstado(ESTADO *estado, int *selecionado, Music playlist[], int musicaAtual, Texture2D texturas[]); // checa condicao do jogo a cada ciclo (menus, jogando, cutscenes)
void selecionaMusica(ESTADO *estado, int *musicaAtual); //adequa musica ao estado atual
void inicializa(ESTADO* estado, Texture2D* texturas, Music* playlist); //inicializa variaveis
void vitoria(ESTADO* estado, Texture2D* texturas, Music* playlist); //avcança de fase
#endif
