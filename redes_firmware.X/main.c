/*
 * File:   main.c
 * Authors: Luis Felipe Kunzler & Ruan Carlos Pinto
 *
 * Created on 26 de Maio de 2018, 14:47
 */

#include <xc.h>
#include <stdint.h>

#include "main.h"
#include "comunicacao.h"
#include "uart.h"
#include "display.h"

void main(void)
{
    /* init reg */
    ADCON1 = 0x0F; // all pins [an12:an0] digital
    OSCCON = 0b01110010; // internal clock at 8MHz

    TRISDbits.RD0 = 0; // saida
    TRISDbits.RD1 = 0;
    TRISDbits.RD2 = 1; // entrada
    TRISDbits.RD3 = 1;
    /* end init reg*/

    /* init comunication */
    uart_init();
    TX_EN_TRIS = 0; // saida
    TX_EN_PIN = 1; // high, habilita recepcao
    /* end init comunication */

    /* variables declaration */    
    dados_t dados; // dados utilizados na comunicacao
    uint8_t piscadas; // quantas piscadas da no led ao receber o comando
    uint16_t tempo_ms; // recebe o tempo em segundos e converte para mili
    lcd_t lcd; // variavel de lcd

    /* init LCD */
    lcd_init(&lcd);
    /* end init LCD*/

    char msg[33] = {0}; // 33 bytes, capacidade do display + \0 no final
    uint8_t pos = 0;
    /* end variables declaration */

    /* init variables*/
    LED1 = 1;
    LED2 = 1;
    write_zero(&dados);

    /* end init variables*/

    /* init infinity loop*/
    while (1) {
        /* receive byte */
        if (!uart_check_rx()) { // se ha algum dado para ser recebido
            do {
                // recebe o novo dado
                dados.buff[dados.count++] = RCREG;
            } while (!uart_check_rx());
            dados.data_flag = 1; // a transmissao acabou.
        }
        /* end of receive byte */

        if (dados.data_flag) {
            switch (check_data(&dados)) {
            case ERR_STX:

                break;
            case ERR_IGNORE_MSG:

                break;
            case ERR_BCC:

                break;
            case ERR_NAK:
                msg[0] = CMD_NAK;
                mk_msg(&dados, 1, msg);
                write_cmd(&dados, dados.addr_from);

                break;
            case LIGA_LED1:
                LED1 = 0;

                msg[0] = CMD_ACK;
                mk_msg(&dados, 1, msg);
                write_cmd(&dados, dados.addr_from);

                break;
            case DESLIGA_LED1:
                LED1 = 1;

                msg[0] = CMD_ACK;
                mk_msg(&dados, 1, msg);
                write_cmd(&dados, dados.addr_from);

                break;
            case LIGA_LED2:
                LED2 = 0;

                msg[0] = CMD_ACK;
                mk_msg(&dados, 1, msg);
                write_cmd(&dados, dados.addr_from);

                break;
            case DESLIGA_LED2:
                LED2 = 1;

                msg[0] = CMD_ACK;
                mk_msg(&dados, 1, msg);
                write_cmd(&dados, dados.addr_from);

                break;
            case LE_BOTAO1:
                msg[0] = CMD_ACK;
                msg[1] = (char) BT1;
                mk_msg(&dados, 2, msg);
                write_cmd(&dados, dados.addr_from);

                break;
            case LE_BOTAO2:
                msg[0] = CMD_ACK;
                msg[1] = (char) BT2;
                mk_msg(&dados, 2, msg);
                write_cmd(&dados, dados.addr_from);

                break;
            case PISCA_LED1:
                // TODO: poderia ser feito de uma forma melhor, sem ler o buff
                // recebe o numero de piscadas
                
                piscadas = dados.buff[5];
                // converte pra mili e divide por 2 (tempo maximo = 131 segundos)                
                tempo_ms = dados.buff[6]*500; 

                // responde que entendeu o comando e vai executar a acao
                msg[0] = CMD_ACK;
                mk_msg(&dados, 1, msg);
                write_cmd(&dados, dados.addr_from);
                
                for (uint8_t i = 0; i < piscadas; i++) {
                    LED1 = !LED1;
                    delay_ms(tempo_ms);
                    LED1 = !LED1;
                    delay_ms(tempo_ms);
                }
                
                break;
            case PISCA_LED2:
                // TODO: poderia ser feito de uma forma melhor, sem ler o buff
                // recebe o numero de piscadas                
                piscadas = dados.buff[5];
                // converte pra mili e divide por 2 (tempo maximo = 131 segundos)
                tempo_ms = dados.buff[6]*500; 

                // responde que entendeu o comando e vai executar a acao
                msg[0] = CMD_ACK;
                mk_msg(&dados, 1, msg);
                write_cmd(&dados, dados.addr_from);
                
                for (uint8_t i = 0; i < piscadas; i++) {
                    LED2 = !LED2;
                    delay_ms(tempo_ms);
                    LED2 = !LED2;
                    delay_ms(tempo_ms);
                }                
                
                break;
            case LE_MSG:
                pos = (dados.buff[5]) - 0x80; // pega a posicao e referencia no 0

                // converte para posicao no display
                uint8_t linha = pos < 16 ? 1 : 2;
                uint8_t coluna = (pos % 16) + 1;

                // limpa o vetor de mensagem
                for (uint8_t i = 0; i < 33; i++) {
                    msg[i] = 0;
                }

                // passa a mensagem do buffer para o vetor de msg
                rcv_msg(&dados, msg);

                // escreve no buffer do display
                lcd_clean_all(&lcd);
                lcd_write(&lcd, linha, coluna, msg);

                // comunica que a mensagem foi recebida
                msg[0] = CMD_ACK;
                mk_msg(&dados, 1, msg);
                write_cmd(&dados, dados.addr_from);

                break;
            default:

                break;
            }

            write_zero(&dados);
            /* end sent byte */
        }

        lcd_runtime(&lcd);
    }
    /* end infinity loop */

    return;
}

void delay_ms(uint16_t milis)
{
    for (uint16_t t = 0; t < milis; t++) {
        __delay_ms(1);
    }
}