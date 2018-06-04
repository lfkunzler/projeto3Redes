/*
 * File:   main.c
 * Author: Luis Felipe Kunzler
 *
 * Created on 26 de Maio de 2018, 14:47
 */

#include "main.h"
#include "comunicacao.h"

void my_delay(uint16_t milis)
{
    for (uint16_t t = 0; t < milis; t++) {
        __delay_ms(1);
    }
}

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
    char msg[32] = {0};
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
            default:

                break;
            }
            
            write_zero(&dados);
            /* end sent byte */
        }
    }
    /* end infinity loop */

    return;
}
