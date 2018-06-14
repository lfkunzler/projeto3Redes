/*
 * File:   comunicacao.c
 * Authors: Luis Felipe Kunzler & Ruan Carlos Pinto
 *
 * Created on 26 de Maio de 2018, 15:44
 */
#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h> // include standart intenger types

#include "comunicacao.h"
#include "uart.h"

void rcv_msg(dados_t *data, char *msg)
{
    /*
     * PARA O CASO DE RECEBER UM TEXTO:
     * 
     * pos 0 == stx
     * pos 1 == end destino
     * pos 2 == end origem
     * pos 3 == comando
     * pos 4 == quantidade n de dados
     * pos 5 == posicao
     * pos 5 == primeiro caracter
     * pos (n-1)+6 == ultimo caracter
     * pos n+6 == bcc
     */

    // count-1 pois ignora o byte que era de posicao
    for (uint8_t i = 0; i < data->count - 1; i++) {
        msg[i] = data->buff[i + 6];
    }
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
    uint8_t aux[DATA_LEN] = {0};

    aux[0] = STX; // cabecalho
    aux[1] = addr_to; // destino da mensagem
    aux[2] = NET_ADDRESS; // origem = sempre sai de mim
    aux[3] = data->command; // comando efetuado
    aux[4] = data->count; // tamanho do buffer

    uint8_t i = 0;
    while (i < data->count) {
        aux[i + 5] = data->buff[i++]; // passa a mensagem do buffer para o aux
    }

    aux[i + 5] = calc_bcc(data->buff);

    TX_EN_PIN = 1; // send data  
    for (uint8_t t = 0; t < i + 6; t++) {
        uart_send_byte(aux[t]);
    }
    TX_EN_PIN = 0; // receive data
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

comunicacao_en check_data(dados_t *data)
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


    if (data->count == 0) { // comandos com nenhum byte de dados
        if (data->command == RD_BUT1) {
            return LE_BOTAO1;
        }
        if (data->command == RD_BUT2) {
            return LE_BOTAO2;
        }
        return ERR_NAK;
    } else if (data->count == 1) { // comandos com 1 byte
        if (data->command == WR_LED1) { // se for acionar o led1
            return(data->buff[5] & 0x01 == 1 ? LIGA_LED1 : DESLIGA_LED1);
        }
        if (data->command == WR_LED2) { // se for para acionar o led2
            return(data->buff[5] & 0x01 == 1 ? LIGA_LED2 : DESLIGA_LED2);
        }
        return ERR_NAK;
    } else if (data->command == WRT_MSG) { // se for para escrever mensagem
        // se estiver fora dos limites de posicionamento
        if (data->buff[5] < 0x80 && data->buff[5] > 0x9F) {
            return ERR_NAK;
        }
        return LE_MSG;
    } else if (data->count == 2) { // comandos com 3 bytes
        if (data->command == BLINK_LED1) { // se for para piscar o led1
            // TODO: ler quanto tempo para piscar e passar essa informacao
            // para o main
            return PISCA_LED1;
        }
        if (data->command == BLINK_LED2) { // se for para piscar o led2
            // TODO: ler quanto tempo para piscar e passar essa informacao
            // para o main
            return PISCA_LED2;
        }
        return ERR_NAK;
    }
    else { // nao foi possivel mapear o comando
        return ERR_NAK; // comando nao reconhecido        
    }

    return ERR_UNDETECTED;
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
