/*
 * File:   comunicacao.c
 * Author: Luis Felipe Kunzler
 *
 * Created on 26 de Maio de 2018, 15:44
 */
#include "comunicacao.h"

void uart_init(void)
{
    RCSTA = 0x90; //
    TXSTA = 0x24; //
    // baudrate para sync=0, brg16=0, brgh=1
    // baudrate = Fosc/(16(spbrg+1))
    // 16*(spbrg+1) = Fosc/baudrate
    // spbrg + 1 = Fosc/(baudrate*16)
    // spbrg = Fosc/(baudrate*16) - 1
    // spbrg = 80000000/(9600*16) - 1 = 51    
    SPBRG = 51;
}

short uart_check_rx(void)
{
    uint16_t tempo;

    RCSTAbits.OERR = 0;
    RCSTAbits.CREN = 0;
    NOP();
    RCSTAbits.CREN = 1;

    tempo = 0xFFF; //*************** mudar   ***********

    do {
        --tempo;
    } while (!RCIF && tempo > 0); // se recebeu algum byte, RCIF == 1

    if (tempo > 0) { // recebeu   dado
        return(0);
    }

    return(1);
}

uint8_t uart_receive_byte(void)
{
    uint8_t data = RCREG;
    return data;
}

void uart_send_byte(uint8_t byte)
{

    while (TXIF == 0); // verifica se pode enviar
    TXREG = byte; // passa o dado para o registrador, realizando o envio
    while (!TRMT); // enquanto estiver mandando (TST full))
}

void mk_msg(dados_t *data, uint8_t count, char *string)
{
    data->count = count;

    for (uint8_t i = 0; i < count; i++) {
        data->buff[i] = string[i];
    }
}

void write_cmd(dados_t *data, uint8_t addr_to)
{
    char aux[DATA_LEN] = {0};

    aux[0] = STX; // cabecalho
    aux[1] = addr_to; // destino da mensagem
    aux[2] = NET_ADDRESS; // origem = sempre sai de mim
    aux[3] = data->command; // comando efetuado
    aux[4] = data->count; // tamanho do buffer

    uint8_t i = 0;
    while (i < data->count) {
        aux[i + 5] = data->buff[i++]; // passa a mensagem do buffer para o aux
    }

    aux[i + 5] = calc_bcc(data);

    for (uint8_t t = 0; t < i + 6; t++) {
        uart_send_byte(aux[t]);
    }
}

uint8_t calc_bcc(uint8_t *data)
{
    uint8_t bcc = data[0]; // recebe o primeiro byte
    //uart_send_byte(data->buff[0]);
    // do segundo byte até o ultimo
    for (uint8_t i = 1; i < DATA_LEN; i++) {
        bcc = (uint8_t) (bcc ^ data[i]);
    }

    return(bcc);
}

en_comunicacao_t check_data(dados_t *data)
{
    if (data->buff[0] != STX) {
        return ERR_STX; // stx invalido
    }

    if (data->buff[1] != NET_ADDRESS) {
        return ERR_IGNORE_MSG; // mensagem nao eh pra mim       
    }

    if (calc_bcc(data->buff) != 0) {
        return ERR_BCC; // se o byte de verificacao dos dados estiver errado
    }

    data->addr_from = data->buff[2]; // salva o end de origem da mensagem
    data->command = data->buff[3]; // salva o comando recebido
    data->count = data->buff[4]; // quantos bytes de dados ha na mensagem

    if (data->count == 1) { // comandos com 1 byte
        if (data->buff[3] == WR_LED1) { // se for acionar o led1
            return(data->buff[5] & 0x01 == 1 ? LIGA_LED1 : DESLIGA_LED1);
        } else if (data->buff[3] == WR_LED2) { // se for para acionar o led2
            return(data->buff[5] & 0x01 == 1 ? LIGA_LED2 : DESLIGA_LED2);
        } else {
            return ERR_NAK; // comando nao reconhecido
        }
    }
}

void write_zero(dados_t *dados)
{
    for (uint8_t i = 0; i < DATA_LEN; i++) {
        dados->buff[i] = 0x00;
    }

    dados->count = 0;
    dados->command = 0;
    dados->data_flag = 0;
    dados->addr_from = 0;
}
