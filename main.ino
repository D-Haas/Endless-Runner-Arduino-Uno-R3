// Importando a biblioteca para controle do display de LCD
#include <LiquidCrystal.h>

// Definicoes:
// Intervalo em ciclos para o aparecimento de novos barreiras
#define tempo_de_barreira_min 2
#define tempo_de_barreira_max 5
// Tempos minimo e maximo permitidos para cada ciclo em ms
#define tempo_de_ciclo_min 300
#define tempo_de_ciclo_inicial 1000

// Posicao horizontal do player na tela
#define posicao_h_player 2

// Criando a tela
LiquidCrystal tela(2,3,4,5,6,7);

// Usa-se uma array do formato da tela para
// emulacao do cenario
char cenario[16][2];

// A posicao vertical do player, definida pelos numeros 0 ou 1,
// sendo 0 o topo e 1 a base do display
char posicao_v_player = 0;

//Defini-se o tempo de ciclo como uma variavel para
//reduzi-lo conforme o jogador progride no minijogo
short tempo_de_ciclo = tempo_de_ciclo_inicial;

// Cria-se o contador de barreiras, para controlar o
// intervalo aleatorio no qual cada barreira eh gerado
char contador_de_barreira = random(tempo_de_barreira_min, tempo_de_barreira_max+1);

// A pontuacao eh iniciada como zero e deve ser incrementada
// em 1 a cada barreira desviado
unsigned long pontuacao = 0;

void setup()
{
  // Inicia-se a tela com o formato 16x2
  tela.begin(16,2);
  // Desliga-se o autoscroll para evitar que os caracteres
  // sejam empurrados pelos anteriores
  tela.noAutoscroll();
  // O pin 8 é definido como input
  pinMode(8, INPUT_PULLUP);
  
}

void loop()
{
  mover_player();
  mover_barreiras();
  checar_colisao_e_desvio();
  gerar_barreira();
  atualizar_tela();
  delay(tempo_de_ciclo);
}

void mover_player()
// Move o player para a outra pista se o botão estiver pressionado
{
  // Se o botão estiver pressionado
  if (!digitalRead(8)) {
    // Se o player estiver no topo, mova o player para baixo
    // Caso contrário, mova-o para cima
    posicao_v_player = posicao_v_player==0 ? 1 : 0;
  }
}

void mover_barreiras()
// Arrasta as barreiras para a esquerda e deleta as barreiras na
// extrema esquerda da tela
{
  // Para cada linha
  for (char i=0; i<2; i++) {
    // Se houver barreira na posição 0 (esquerda)
    if (cenario[0][i] == 1) {
      // Delete ela do cenário
      cenario[0][i] = 0;
    }
    // Para cada coluna a partir da 1
    for (char j=1; j<16; j++) {
      // Se houver uma barreira nela
      if (cenario[j][i] == 1) {
        // Delete ela da posição atual
        cenario[j][i] = 0;
        // Crie outra barreira à sua esquerda
        cenario[j-1][i] = 1;
        // Teremos a impressão que a barreira se moveu
      }
    }
  }
}

void checar_colisao_e_desvio()
// Testa se existe uma barreira na posição exata do jogador
// e caso sim, inicia-se a tela de game over
// Caso o teste retorne negativo, checa se o player está passando por uma barreira,
// então aumenta-se a pontuação e a velocidade do jogo
{
  // Se houver uma barreira na posição do player
  if (cenario[posicao_h_player][posicao_v_player] == 1) {
    // Inicie a tela de game over
    game_over();
  }
  // Se houver barreira do lado do player
  else if (posicao_v_player == 0 && cenario[posicao_h_player][1] == 1 ||
     posicao_v_player == 1 && cenario[posicao_h_player][0] == 1 ) {
    // Adicione +1 para a pontuação
    pontuacao++;
    // Se o tempo de ciclo não estiver em seu mínimo
    if (tempo_de_ciclo > tempo_de_ciclo_min+10) {
      // Diminua-o em 10 ms
      tempo_de_ciclo -= 10;
    }
  }
}

void gerar_barreira()
// A cada intervalo aleatório de ciclos (dentro de certos limites)
// cria-se uma barreira no cenário, em uma altura escolhida aleatoriamente
{
  // Remove-se 1 da variavel contador de barreira
  contador_de_barreira--;
  // Se o contador chegar a 0
  if (contador_de_barreira <= 0) {
    // Resete-o a um valor aleatorio entre os limites definidos
    contador_de_barreira = random(tempo_de_barreira_min, tempo_de_barreira_max+1);
    // Escolha uma posicao vertical para a nova berreira
    int posicao_barreira = random(2);
    // Crie a barreira na direita da tela
    cenario[15][posicao_barreira] = 1;
  }
}

void atualizar_tela()
// Imprimi-se a situação atual na tela (Jogador e barreiras)
{
  // Limpe a tela
  tela.clear();
  // Para cada posicao da tela
  for (char i=0; i<2; i++) {
    for (char j=0; j<16; j++) {
      // Se houver uma barreira
      if (cenario[j][i]==1) {
        // Coloque o cursor nessa posicao
        tela.setCursor(j,i);
        // Imprima a letra 'O'
        tela.write('O');
      }
    }
  }
  // Coloque o cursor na posicao do player
  tela.setCursor(posicao_h_player, posicao_v_player);
  // Imprima o caracter '>'
  tela.write('>');
}

void game_over()
// Executa a animação de game over e mostra a pontuacao na tela
// então espera-se o pressionar do botao para resetar o jogo
{
  // Limpa a tela
  tela.clear();
  
  // Preenche-se a tela com os caracteres '-'
  for (char j=0; j<8; j++) {
    for (char i=0; i<2; i++) {
      tela.setCursor(j,i);
      tela.write('-');
      tela.setCursor(15-j,i);
      tela.write('-');
    }
    delay(100);
  }
  
  // Preenche-se a tela com os caracteres '='
  for (char j=7; j>=0; j--) {
    for (char i=0; i<2; i++) {
      tela.setCursor(j,i);
      tela.write('=');
      tela.setCursor(15-j,i);
      tela.write('=');
    }
    delay(100);
  }
  
  // Pisque a tela 2 vezes em intervalos de 0.3 segundos
  for (char i=0; i<2; i++) {
    delay(300);
  	tela.noDisplay();
  	delay(300);
  	tela.display();
  }
  
  // Espere 0.3 segundos
  delay(300);
  
  // Escreva GAME OVER na tela
  tela.setCursor(0,0);
  tela.print("      GAME      ");
  tela.setCursor(0,1);
  tela.print("      OVER      ");
  
  // Espere 1 segundo
  delay(1000);
  
  // Imprima a pontuacao na tela junto a uma legenda abaixo
  tela.clear();
  tela.setCursor(0,0);
  tela.print(pontuacao);
  tela.setCursor(0,1);
  tela.print("          POINTS");
  
  // Aguarde o pressionar do botão
  while (digitalRead(8)) {
    delay(100);
  }
  
  // Resete a pontuacao
  pontuacao = 0;
  
  // Limpe o cenario
  for (char j=0; j<16; j++) {
    for (char i=0; i<2; i++) {
      cenario[j][i] = 0;
    }
  }
  
}
