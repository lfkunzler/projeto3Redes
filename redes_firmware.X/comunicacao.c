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

void send_string_data(char *string, dados_t *dados)
{
    uint8_t aux[DATA_LEN] = {0};

    aux[0] = STX; // cabecalho
    aux[1] = dados->addr_from; // endereco destino
    aux[2] = NET_ADDRESS; // meu endereco
    aux[3] = WRT_MSG; // sinaliza o envio da mensagem
    //aux[4] = count;
    uint8_t count;

    for (count = 0; count < string[count] != 0 && count+5 < DATA_LEN; count++) {
        // passa a mensagem para o campo de dados
        //uart_send_byte(string[count]);
        aux[count + 5] = string[count]; 
    }
    aux[4] = count; // indica quantos bytes ha
    aux[5 + count] = (uint8_t) 0; // garante um 0 na posicao do bcc
    aux[5 + count] = (uint8_t) calc_bcc(dados); // recebe o calculo do bcc   
    
    for (uint8_t i = 0; i < count+5; i++) {
        uart_send_byte(aux[i]);
    }
}

uint8_t calc_bcc(dados_t *data)
{
    uint8_t bcc = data->buff[0]; // recebe o primeiro byte
    //uart_send_byte(data->buff[0]);
    // do segundo byte até o ultimo
    for (uint8_t i = 1; i < data->count; i++) {
        bcc = (uint8_t) (bcc ^ data->buff[i]);
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
    if (calc_bcc(data) != 0) {
        return ERR_BCC;
    }
    
    data->addr_from = data->buff[2]; // salva o end de origem da mensagem
    
    if (data->buff[3] == WR_LED1) { // se for acionar o led1
        return(data->buff[4] & 0x01 == 1 ? LIGA_LED1 : DESLIGA_LED1);
    } else if (data->buff[3] == WR_LED2) { // se for para acionar o led2
        return(data->buff[4] & 0x01 == 1 ? LIGA_LED2 : DESLIGA_LED2);
    } else {
        return ERR_NAK;
    }


#if 0
    // se ha erro no inicio da mensagem

    if (data[0] != STX) {
        return(ERR_STX);
    }
    // se a mensagem nao for destinada para mim
    if (data[1] != ENDERECO_REDE) {
        return(ERR_IGNORE_MSG);
    }
    uint8_t count = 0;
    uint8_t BCC = 0;

    while (data[count + 1] != 0) {

    }
#endif
//    return ERR_UNDETECTED;
}