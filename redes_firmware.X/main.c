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

void zera_dados(dados_t *dados)
{
    for (uint8_t i = 0; i < DATA_LEN; i++) {
        dados->buff[i] = 0x00;
    }
    dados->count = 0;
    dados->data_flag = 0;
}

void main(void)
{

    /* init reg */
    ADCON1 = 0x0F; // all pins [an12:an0] digital

    TRISDbits.RD0 = 0; // saida
    TRISDbits.RD1 = 0; // saida
    /* end init reg*/

    /* init UART */
    uart_init();
    /* end init UART */

    /* variables declaration */
    //uint8_t buf[DATA_LEN]; // buffer to data storage
    //uint8_t count; // counter of how many data was stored
    //short __data_flag; // if received all data
    dados_t dados;
    /* end variables declaration */


    /* init variables*/
    LED1 = 1;
    LED2 = 1;
    zera_dados(&dados);

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
            /* send byte */
            switch (check_data(&dados)) {
            case ERR_STX:
                send_string_data((char *)"erro no stx\0", &dados);

                break;
            case ERR_IGNORE_MSG:
                send_string_data((char *)"nao eh para mim\0", &dados);

                break;
            case ERR_BCC:
                send_string_data((char *)"BCCERR\0", &dados);
                break;
            case ERR_NAK:
                send_string_data((char *)"oi nak\0", &dados);
                break;
            case LIGA_LED1:
                LED1 = 0;
                send_string_data("liguei o led1\0", &dados);
                break;
            case DESLIGA_LED1:
                LED1 = 1;
                break;
            case LIGA_LED2:
                LED2 = 0;
                break;
            case DESLIGA_LED2:
                LED2 = 1;
                break;
            default:
                send_string_data((char *)"nao sei o que houve\0", &dados);
                break;
            }

            zera_dados(&dados);
            /* end sent byte */
        }

        //        if (__dados) { // muitos ciclos sem dados, recebeu tudo
        //            uint8_t retorno = check_data(dados);
        //            switch (retorno) {                
        //                case LIGA_LED1:
        //                    LED1 = 0;
        //                    break;
        //                case DESLIGA_LED1:
        //                    LED1 = 1;
        //                    break;
        //                case LIGA_LED2:
        //                    LED2 = 0;
        //                    break;
        //                case DESLIGA_LED2:
        //                    LED2 = 1;
        //                    break;
        //                default:
        //                    //lcd_clean_all(&lcd);
        //                    lcd_write(&lcd, 1, 1, dados);
        //                    break;
        //            }
        //            zera_dados(dados, LEN_DADOS);
        //            count = 0;
        //            __dados = 0;
        //        }
    }
    /* end infinity loop */

    return;
}
