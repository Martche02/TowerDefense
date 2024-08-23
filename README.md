# Projeto do Grupo 6

## Descrição
Este projeto é um jogo interativo onde o jogador controla John Rambo, que pode interagir com diversos elementos no mapa, como inimigos, recursos, armadilhas, túneis e paredes. O objetivo é sobreviver, derrotar os inimigos e progredir pelas fases, enquanto lida com diferentes desafios e utiliza estratégias para vencer.

## Funcionalidades

### 1. Personagem do Jogador e Interações
- **Execução**: O jogador controla John Rambo, que pode:
  - Colocar armadilhas (se disponíveis).
  - Atravessar túneis.
  - Colidir com inimigos e objetos.
- **Movimentação**:
  - Movimentação com as teclas `setas` ou `WASD`.
  - Ativação de armadilhas com a tecla `G`.
  - O personagem possui animações para movimentação esquerda e direita.
  - O jogador inicia com 1 vida, podendo ganhar mais. Se chegar a 0 vidas, o jogo termina.
- **Funções utilizadas**: `getAcao`.
- **Estruturas e variáveis utilizadas**: `POSICAO posJogador`, `vidaJogador`, `armadilhasdisp`, `direcao`.

### 2. Colisões entre Jogador, Objetos e Inimigos
- **Execução**:
  - O jogador não pode atravessar paredes.
  - Ao colidir com túneis, o jogador é transportado para o outro lado ou para a saída mais distante.
  - Recursos são coletados, e o jogador ganha uma armadilha adicional.
  - Colidir com inimigos resulta na perda de uma vida para ambos. Se o inimigo ou jogador ficar sem vida, eles morrem.
  - Inimigos são limitados por paredes e túneis, mudando sua direção ao encontrá-los.
  - Inimigos que chegam à base reduzem a vida dela e morrem.
  - Inimigos morrem ao colidir com armadilhas.
  - O jogo constantemente verifica colisões e atualiza o estado antes de desenhar o próximo frame.
- **Funções utilizadas**: `verificarColisao`, `colisaoJogadorParede`, `colisaoJogadorTunel`, `colisaoJogadorRecurso`, `colisaoJogadorInimigo`, `colisaoInimigoParede`, `colisaoInimigoTorre`, `colisaoArmadilhaInimigo`, `atualizarEstado`.
- **Estruturas e variáveis utilizadas**: `POSICAO`, `vidaJogador`, `qtdInimigos`, `qtdRecursos`, `qtdArmadilhas`, `armadilhasdisp`, `qtdTuneis`, `qtdParedes`.

### 3. Menus
- **Execução**:
  - O jogo possui 7 estados diferentes: Menu Inicial, Menu de Pause, Menu de Derrota, Começo de Novo Jogo, Jogando, Tela Entre Fases, Tela de Fim de Jogo.
  - Navegação nos menus com setas para cima/baixo, e setas laterais para ajustar o volume.
  - Cada estado tem texturas e músicas específicas.
  - Menu Inicial: Começa o jogo, carrega jogo salvo ou sai.
  - Menu de Pause: Pausa o jogo, permite salvar, carregar jogo salvo, ajustar volume, retornar ao menu principal ou sair.
  - Menu de Derrota: Aparece após perder o jogo, com opções de retornar ao menu principal, começar um novo jogo ou carregar jogo salvo.
  - Novo Jogo: Reinicia o estado, mostrando uma cutscene inicial, um breve tutorial e dicas.
  - Jogando: Atualiza e desenha o mapa e os objetos.
  - Tela Entre Fases: Aparece após derrotar todos os inimigos de uma fase. Pressionar `Enter` carrega o próximo mapa.
  - Tela de Fim de Jogo: Mostra uma cutscene final e parabeniza o jogador, retornando ao menu inicial.
- **Funções utilizadas**: `carregarEstado`, `salvarEstado`, `atualizarEstado`, `carregarMapaDeArquivo`, `novaFase`, `cutsceneinicial`, `cutscenederrota`, `cutscenefim`, `verificaEstado`, `vitoria`.
- **Estruturas e variáveis utilizadas**: `ESTADO`, `pagina`, `mapa`.

## Como Jogar
1. **Movimentação**: Use as teclas `setas` ou `WASD` para mover John Rambo.
2. **Armadilhas**: Ative armadilhas pressionando `G` (se disponíveis).
3. **Interação**: Colete recursos, evite inimigos e use túneis para se mover rapidamente pelo mapa.
4. **Progresso**: Derrote todos os inimigos em cada fase para avançar.

## Requisitos
- Sistema operacional: Windows, macOS, Linux
- Ferramentas: Compilador C++, SDL (Simple DirectMedia Layer)

## Instalação
1. Clone este repositório:
   ```bash
   git clone https://github.com/martche02/towerdefense/project.git
   '''
2. Execute o jogo:
   '''bash
   ./main.exe
   '''
