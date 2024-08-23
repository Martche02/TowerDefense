# Tower Defense - Grupo 6

## Integrantes
- Pietro de Lorenzo
- Marcelo Gonda
- Gabriel Richter

## Descrição
Este projeto é um jogo do tipo Tower Defense, onde o jogador controla John Rambo em um cenário de batalha. O jogador pode interagir com diversos elementos do mapa, como recursos, armadilhas, túneis, inimigos e muito mais, para sobreviver e derrotar todos os inimigos.

## Funcionalidades

### 1. Personagem do Jogador e Suas Interações
- **Execução**: 
  - O jogador controla John Rambo, podendo:
    - Colocar armadilhas (se disponíveis).
    - Atravessar túneis.
    - Colidir com inimigos.
    - Movimentar-se livremente pelo mapa, exceto atravessando paredes.
  - Movimentação com teclas `setas` ou `WASD`.
  - Ativação de armadilhas com a tecla `G`.
  - O jogador inicia com 1 vida; ao chegar a 0 vidas, o jogo termina.
- **Funções utilizadas**: `getAcao`.
- **Estruturas e variáveis utilizadas**: `POSICAO posJogador`, `vidaJogador`, `armadilhasdisp`, `direcao`, `k` (registra teclas digitadas).

### 2. Texturas e Músicas
- **Execução**:
  - O personagem Rambo foi escolhido por se adequar perfeitamente à proposta do jogo.
  - Texturas baseadas em cenas de filmes e representações do personagem foram utilizadas.
  - Músicas do filme Rambo foram adicionadas para complementar a experiência de jogo.
- **Funções utilizadas**: `desenho` (para carregar texturas), `inicializa` (para carregar arquivos `.png` e `.mp3`), `selecionaMusica`.
- **Estruturas e variáveis utilizadas**: `MUSICA`, `playlist[]`, `texturas[]`, `musicaAtual`.

### 3. Colisões entre Jogador, Objetos e Inimigos
- **Execução**:
  - O jogador não pode atravessar paredes.
  - Ao colidir com túneis, o jogador é transportado para o outro lado do túnel mais distante na direção da colisão (caso não haja, utiliza a saida do próprio túnel).
  - Coleta de recursos ao colidir com eles, aumentando o número de armadilhas disponíveis.
  - Colidir com inimigos resulta na perda de uma vida para ambos.
  - Inimigos são limitados por paredes e túneis, mudando de direção ao encontrá-los.
  - Inimigos que chegam à base reduzem a vida dela e morrem.
  - Inimigos morrem ao colidir com armadilhas.
  - O jogo verifica constantemente se dois objetos estão na mesma posição para realizar interações antes de atualizar o estado.
- **Funções utilizadas**: `verificarColisao`, `colisaoJogadorParede`, `colisaoJogadorTunel`, `colisaoJogadorRecurso`, `colisaoJogadorInimigo`, `colisaoInimigoParede`, `colisaoInimigoTorre`, `colisaoArmadilhaInimigo`, `atualizarEstado`.
- **Estruturas e variáveis utilizadas**: `POSICAO`, `vidaJogador`, `qtdInimigos`, `qtdRecursos`, `qtdArmadilhas`, `armadilhasdisp`, `qtdTuneis`, `qtdParedes`.

### 4. Menus e Estados do Jogo
- **Execução**:
  - O jogo pode estar em 7 estados diferentes: `Menu inicial`, `Menu de pause`, `Menu de derrota`, `Início de um novo jogo`, `Jogando`, `Tela entre fases` e `Tela de fim de jogo`.
  - Navegação nos menus com setas para cima/baixo e setas laterais para ajustar o volume da música.
  - Cada estado tem texturas e músicas específicas.
  -- Menu inicial: Começa um novo jogo, carrega jogo salvo ou sai do jogo.
  -- Menu de pause: Ativado com a tecla `Tab`, pausa o jogo e permite salvar/carregar o jogo, ajustar o volume, retornar ao menu principal ou sair do jogo.
  -- Menu de derrota: Aparece ao perder o jogo, oferecendo opções de voltar ao menu principal, começar um novo jogo ou carregar jogo salvo.
  -- Novo jogo: Reinicia o estado, com cutscene inicial, tutorial e dicas.
  -- Jogando: Atualiza e desenha o mapa e os objetos.
  -- Tela entre fases: Aparece ao derrotar todos os inimigos de uma fase; pressionar `Enter` carrega o próximo mapa.
  -- Tela de fim de jogo: Exibe uma cutscene final e retorna ao menu inicial.
- **Funções utilizadas**: `carregarEstado`, `salvarEstado`, `atualizarEstado`, `carregarMapaDeArquivo`, `novaFase`, `cutsceneinicial`, `cutscenederrota`, `cutscenefim`, `verificaEstado`, `vitoria`.
- **Estruturas e variáveis utilizadas**: `ESTADO` (controla em qual menu o jogo está), `pagina` (avança cutscenes), `mapa` (matriz que representa o mapa transcrito do arquivo `.txt`).

### 5. Trapaças
- **Execução**:
  - As últimas 11 teclas digitadas são registradas e comparadas com sequências de trapaças.
  - Uma das trapaças, baseada no "konamicode" (`↑ ↑ ↓ ↓ ← → ← → B A ENTER`), permite passar de fase automaticamente.
  - Outra trapaça, `rockybalboa`, para a música atual e toca a música do filme Rocky.
- **Funções utilizadas**: `cheat`.
- **Estruturas e variáveis utilizadas**: `ESTADO` (para executar trapaças), `ultimasteclas`.

## Esquema das Funções e Sequências
Para um diagrama detalhado das funções e suas sequências, acesse o [Milanote do projeto](https://app.milanote.com/1SHd6D1tp9ky26?p=DdUkCF9bMu8).

## Como Jogar
1. **Movimentação**: Use as teclas `setas` ou `WASD` para mover John Rambo.
2. **Armadilhas**: Ative armadilhas pressionando `G` (se disponíveis).
3. **Interação**: Colete recursos, evite inimigos e use túneis para se mover rapidamente pelo mapa.
4. **Progresso**: Derrote todos os inimigos em cada fase para avançar.

## Instalação
1. Clone este repositório:
   ```bash
   git clone https://github.com/Martche02/TowerDefense.git
2. Execute o jogo:
   ```bash
   ./main.exe
