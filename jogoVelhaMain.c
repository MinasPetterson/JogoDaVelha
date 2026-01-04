/******************************************************************************

JOGO DA VELHA

Por: Evandro Souza
12/12/2025

Obs.: No incio ia ser apenas uma jogo com a CPU fazendo
jogadas aleatorias e testando se alguem tivesses ganhado...
Mas eu achei algumas regras (dicas): Estrategias comuns para jogar:
Jogar no centro, cantos opostos, laterais, etc...
de maneira mais inteligente, e implementei nesse
pequeno projeto. Espero que caiba na memoria do PIC 12F675 (e coube!)

*******************************************************************************/


#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 4000000 // Clock interno. Tem de definir quando se implementa delay ms ou us (timers)

// Configuration Bits:
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = ON       // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

// Pinos:
#define clkPin GPIO5 // clock pin 4017 (14). pin 2 do PIC
#define resPin GPIO4  // reset pin 4017 (15). pin 3 do PIC
#define coluna1 GPIO0 // Coluna 1 do tabuleiro. pin 7 do PIC
#define coluna2 GPIO1 // Coluna 2 do tabuleiro. pin 6 do PIC
#define coluna3 GPIO2 // Coluna 3 do tabuleiro. pin 5 do PIC
#define teclaPin GPIO3 // Pino que vai ler as teclas. pin 4 do PIC
// VCC eh o pino 1 e o GND eh o pino 8  do PIC

// Quem estah em cada local do tabuleiro:
#define VAZIO   0 // Posicao vazia
#define USUARIO 1 // Usuario
#define CPU     2 // PIC

// Tabuleiro: 9 posicoes (1 a 9)
// 1 | 2 | 3
// 4 | 5 | 6
// 7 | 8 | 9

char tab[10]; // Tabuleiro - Cria o Tabuleiro.
char tecla = 0; // Tecla pressionada
char displayCount = 0; // para mostrar cada linha na interrupcao.

// SETUP

void setUp() { // Configuracao dos Registradores especiais:

    TRISIO = 0b00001000; // Diz quais pinos sao output(0) e quais sao input(1). Pin 4 do PIC (GPIO3) eh input (teclas)
    ANSEL = 0b00000000; // Configura ANSEL. Todos digitais
    CMCON = 0b00000111; // Desabilita os comparadores
    VRCON = 0b00000000; // Voltagem de ref dos comparadores;
    WPU = 0b00000000; // PullUp Off

    OPTION_REG = 0b10000010; // PullUp Off, Prescaler is assigned to the timer TMR0
    // Ajustar para que os LEDs nao fiquem piscando (usei 0b10 o que da 1:8);

    INTCON = 0b10100000; // Habilita as interrupcoes no TMR0 e Global Interrupts (GIE e T0IE)
    GPIO = 0b00001000; // Saidas em zero
    TMR0 = 0; // Timer0 resetado
}

// Rotina da interrupcao:
// Aqui se acendem os LEDs e
// Le cada tecla, se pressionada, salva na variavel global.

void __interrupt() isr() { //interruption vector

    TMR0 = 0; // Zera o timer0
    TMR0IF = 0; //  limpa o flag do timer0 interrupt

    // Eu usei um 4017 (contador ate 10) para habilitar cada linha do jogo
    // Ele habilita a linha 1 e eu chaveio as colunas
    // Ao mesmo tempo em que leio as teclas dessa linha.
    // Cada saida do 4017 eh uma tecla.

    //Reseta o 4017
    resPin = 1;
    resPin = 0;

    // Coloca as colunas em nivel alto (desabilita os LEDs):
    coluna1 = 1;
    coluna2 = 1;
    coluna3 = 1;

    displayCount++;
    if (displayCount > 6) displayCount = 0;

    if (displayCount == 0) {
        // LINHA 1:
        // Linha 1 - LEDs vermelhos (CPU) Saida 0 do 4017:
        if (tab[1] == CPU) coluna1 = 0;
        if (tab[2] == CPU) coluna2 = 0;
        if (tab[3] == CPU) coluna3 = 0;
        if (teclaPin) tecla = 1; // Le a tecla 1
        return;
    }

    /// Da um pulso de clock:
    clkPin = 1;
    clkPin = 0;

    if (displayCount == 1) {
        // Linha 1 - LEDs verdes (Jogador)  Saida 1 do 4017:
        if (tab[1] == USUARIO) coluna1 = 0;
        if (tab[2] == USUARIO) coluna2 = 0;
        if (tab[3] == USUARIO) coluna3 = 0;
        if (teclaPin) tecla = 2; // Le a tecla 2
        return;
    }

    // Da um pulso de clock :
    clkPin = 1;
    clkPin = 0;

    if (displayCount == 2) {
        // LINHA 2:
        // Linha 2 - LEDs vermelhos (CPU)  Saida 2 do 4017:
        if (tab[4] == CPU) coluna1 = 0;
        if (tab[5] == CPU) coluna2 = 0;
        if (tab[6] == CPU) coluna3 = 0;
        if (teclaPin) tecla = 3; // Le a tecla 3
        return;
    }

    // Da um pulso de clock :
    clkPin = 1;
    clkPin = 0;

    if (displayCount == 3) {
        // Linha 2 - LEDs verdes (Jogador)  Saida 3 do 4017:
        if (tab[4] == USUARIO) coluna1 = 0;
        if (tab[5] == USUARIO) coluna2 = 0;
        if (tab[6] == USUARIO) coluna3 = 0;
        if (teclaPin) tecla = 4; // Le a tecla 4
        return;
    }

    // Da um pulso de clock :
    clkPin = 1;
    clkPin = 0;

    if (displayCount == 4) {
        // LINHA 3:
        // Linha 3 - LEDs vermelhos (CPU)  Saida 4 do 4017:
        if (tab[7] == CPU) coluna1 = 0;
        if (tab[8] == CPU) coluna2 = 0;
        if (tab[9] == CPU) coluna3 = 0;
        if (teclaPin) tecla = 5; // Le a tecla 5
        return;
    }

    // Da um pulso de clock:
    clkPin = 1;
    clkPin = 0;

    if (displayCount == 5) {
        // Linha 3 - LEDs verdes (Jogador)  Saida 5 do 4017:
        if (tab[7] == USUARIO) coluna1 = 0;
        if (tab[8] == USUARIO) coluna2 = 0;
        if (tab[9] == USUARIO) coluna3 = 0;
        if (teclaPin) tecla = 6; // Le a tecla 6
        return;
    }

    // Da um pulso de clock e continua para ler as demais teclas:
    clkPin = 1;
    clkPin = 0;
    if (teclaPin) tecla = 7; // Le a tecla 7. Saida 6 do 4017:

    clkPin = 1;
    clkPin = 0;
    if (teclaPin) tecla = 8; // Le a tecla 8. Saida 7 do 4017:

    clkPin = 1;
    clkPin = 0;
    if (teclaPin) tecla = 9; // Le a tecla 9. Saida 8 do 4017:

}

// Checa de preencheu alguma das linhas (vitoria):

char checarLinhas(char jogador) {
    if (
            (tab[1] == jogador && tab[2] == jogador && tab[3] == jogador) ||
            (tab[4] == jogador && tab[5] == jogador && tab[6] == jogador) ||
            (tab[7] == jogador && tab[8] == jogador && tab[9] == jogador))
        return 1;
    return 0;
}

// Checa de preencheu alguma das colunas (vitoria):

char checarColunas(char jogador) {
    if (
            (tab[1] == jogador && tab[4] == jogador && tab[7] == jogador) ||
            (tab[2] == jogador && tab[5] == jogador && tab[8] == jogador) ||
            (tab[3] == jogador && tab[6] == jogador && tab[9] == jogador))
        return 1;
    return 0;

}

// Checa de preencheu alguma das diagonais (vitoria):

char checarDiagonais(char jogador) {
    if (
            (tab[1] == jogador && tab[5] == jogador && tab[9] == jogador) ||
            (tab[3] == jogador && tab[5] == jogador && tab[7] == jogador))
        return 1;
    return 0;

}

// Funcao para verificar a vitoria de um jogador (1: Jogador ou 2: CPU):

char checar_vitoria(char jogador) {

    return (checarLinhas(jogador) ||
            checarColunas(jogador) ||
            checarDiagonais(jogador));
}


// Aqui ele mostra o vencedor, piscando as posicoes de vitoria:
// Poderia melhorar, piscando apenas a parte que da a vitoria, mas nao tem
// memoria ROM suficiente para o codigo. Usei 99% de toda a ROM disponivel.

void mostrar_vencedor(char jogador) {

    if (jogador == 255) return;

    // Apaga do tabuleiro o jogador que perdeu;
    for (int x = 1; x < 10; x++) if (tab[x] != jogador) tab[x] = VAZIO;

    for (int x = 1; x < 10; x++) {
        GIE = 0; // Desabilita interrupções globais (Apaga todos os LEDs)
        __delay_ms(200);
        GIE = 1; // Habilita interrupções globais (Acende os LEDs, no caso, somente do vencedor)
        __delay_ms(200);
    }
}

// Logica da jogada da CPU (retorna posicao 1 a 9 ou 255 se tabuleiro estiver cheio)
// Aqui esta toda a "inteligencia das jogadas em ordem de possibilidade de ganhar o jogo":
// Peguei isso na internet e transcrevi as estrategias para a o jogo em C:

char cpu_joga() {

    char i;

    // Fluxo de jogadas:
    // Estrategia: 1. Pode vencer?
    // Testa todas as possibilidades:
    for (i = 1; i < 10; i++) {
        if (tab[i] == VAZIO) {
            tab[i] = CPU; // Joga em todas as posicoes possiveis
            if (checar_vitoria(CPU)) return i;
            tab[i] = VAZIO; // desfaz o jogo
        }
    }

    // Estrategia: 2. Precisa bloquear o jogador de ganhar?
    // Testa todas as possibilidades:
    for (i = 1; i < 10; i++) {
        if (tab[i] == VAZIO) {
            tab[i] = USUARIO; // Joga em todas as posicoes possiveis (Jogador)
            if (checar_vitoria(USUARIO)) {
                tab[i] = CPU;
                return i;
            }
            tab[i] = VAZIO; // desfaz o jogo...
        }
    }

    // Tudo certo, nao vence e nem perde.
    // Entao vamos jogar:

    // Estrategia 2.5 : Vamos bloquear a jogada do canto isolado:
    if (tab[5] == VAZIO) {
        if (tab[1] == USUARIO && tab[3] == VAZIO) return 3;
        if (tab[3] == USUARIO && tab[9] == VAZIO) return 9;
        if (tab[7] == USUARIO && tab[1] == VAZIO) return 1;
        if (tab[9] == USUARIO && tab[7] == VAZIO) return 7;
    };
    //

    // Estrategia: 3. Jogar no centro (posicao 5):
    if (tab[5] == VAZIO) return 5;

    // Centro ja ocupado...
    // Vamos usar outras estrategicas:

    // Estrategia: 4. Cantos jogados e resposta em canto oposto:
    // Cantos jogados: 1, 3, 7, 9 ????
    // Opostos: 1->9, 3->7
    // Opostos: 9->1, 7->3
    if (tab[1] == USUARIO && tab[9] == VAZIO) return 9;
    if (tab[9] == USUARIO && tab[1] == VAZIO) return 1;
    if (tab[3] == USUARIO && tab[7] == VAZIO) return 7;
    if (tab[7] == USUARIO && tab[3] == VAZIO) return 3;

    //
    // Estrategia: 5. Qualquer canto vazio:
    // Jogar no que estiver disponivel:
    // Jogadas em cantos aleatorios disponiveis:
    static const char cantos[4] = {1, 3, 9, 7}; // ordem base: superior-esquerdo, superior-direito, inferior-direito, inferior-esquerdo
    char offset = TMR0 & 0b00000111; // TMR0 & 0x03 => valor entre 0 e 3 - Aqui fiz um Random usando TMR0 para o jogo nao ser repetitivo.

    // Verifica os cantos em ordem rotativa: offset, offset+1, ...
    for (char i = 0; i < 4; i++) {
        char pos = cantos[(offset + i) % 4];
        if (tab[pos] == VAZIO) return pos;
    }

    // Estrategia: 6. Laterais: 2, 4, 6, 8
    // Jogar na primeiro lateral que estiver disponivel:
    // Aqui poderia ser mais inteligente, mas quis poupar processamento
    // Em vez de analisar as possibilidades, deixei o jogo mais facil
    // para o jogador:

    if (tab[2] == VAZIO) return 2;
    if (tab[4] == VAZIO) return 4;
    if (tab[6] == VAZIO) return 6;
    if (tab[8] == VAZIO) return 8;

    return 255; // tabuleiro cheio (empate)
}


// Funcao principal

int main(void) {

    // SetUp do sistema (Registradores Especiais)
    setUp();

    // Quem inicia a rodade e quem eh a vez de jogar (durante o jogo):
    char vezDeJogar = USUARIO;
    char iniciaJogo = USUARIO;

    // Loop principal
    while (1) {

        for (char i = 1; i < 10; i++) tab[i] = VAZIO; // reinicia o tabuleiro

        // Inicia as jogadas
        char jogadas = 0;
        char vencedor = 255; // Por enquanto, nenhum jogador venceu
        vezDeJogar = iniciaJogo;

        while (jogadas < 9 && vencedor == 255) { // Jogo valendo

            if (vezDeJogar == USUARIO) { // Soh sai daqui com uma jogada valida.

                // Aguarda e valida jogada do usuario:
loop:
                tecla = 0;
                while (tecla == 0) {
                    // fica preso nesse loop enquanto:
                    // For a vez do usuario jogar
                    // E nenhuma tecla sendo pressionada
                }
                // E se a tecla pressionada não for numa casa vazia.
                if (tab[tecla] != VAZIO) goto loop;

                // Passou pelo loop. USUARIO pressionou uma tecla valida
                tab[tecla] = USUARIO; // Atribui a casa ao USUARIO
                tecla = 0; // Zera a variavel da leitura (embora isso seja feito no ISR)
                __delay_ms(500); // Espera 500 ms
            }

            // Checa se o usuario venceu:
            if (checar_vitoria(USUARIO)) {
                vencedor = USUARIO;
                iniciaJogo = USUARIO; // Proxima rodada, USUARIO inicia
                break;
            }

            // Jogada final e deu empate:
            if (jogadas >= 9) break; // empate

            jogadas++;

            // CPU joga
            char cpu_pos = cpu_joga();
            if (cpu_pos == 255) break; // tabuleiro cheio
            tab[cpu_pos] = CPU;
            __delay_ms(500);

            jogadas++;
            vezDeJogar = USUARIO;

            // Checa se a cpu venceu:
            if (checar_vitoria(CPU)) {
                vencedor = CPU;
                iniciaJogo = CPU; // Proxima rodada, CPU inicia
                break;
            }
        }

        // Exibe resultado
        if (vencedor == 255) {

            vencedor = 0; // empate

            // Alterna quem comeca, quando tem empate:
            if (iniciaJogo == USUARIO) iniciaJogo = CPU;
            else iniciaJogo = USUARIO;
        }

        // Mostra o vencedor:
        mostrar_vencedor(vencedor);
    }
    return 0;
}

