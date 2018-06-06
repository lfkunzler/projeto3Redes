/*
 * File:   main.c
 * Author: Luis Felipe Kunzler
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

    TRISDbits.RD0 = 0; // saida
    TRISDbits.RD1 = 0;
    TRISDbits.RD2 = 1; // entrada
    TRISDbits.RD3 = 1;
    /* end init reg*/

    /* init UART */
    uart_init();
    /* end init UART */

    /* variables declaration */
    //uint8_t buf[DATA_LEN]; // buffer to data storage
    //uint8_t count; // counter of how many data was stored
    //short __data_flag; // if received all data
    dados_t dados;
    lcd_t lcd;

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