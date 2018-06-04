// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef COMUNICACAO_H
#define	COMUNICACAO_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h> // include standart intenger types

#include "main.h"

#define DATA_LEN 64

#define DE_PIN TRISCbits.RC2

/* PROTOCOLO UCSBUS */
#define STX 0x02 // inicio do protocolo
#define NET_ADDRESS 0x09 // o endereco no qual eu respondo

/* COMANDOS DO PROTOCOLO UCSBUS */
#define RD_BUT1 0x01 // le do status botao 1, 0 quando nao estiver acionado
#define RD_BUT2 0x02 // le o status botao 2, 0 quando nao estiver acionado
#define WR_LED1 0x21 // configura o led1, 1 para ligar e 0 para desligar
#define WR_LED2 0x22 // configura o led2, 1 para ligar
#define BLINK_LED1 0x41 // pisca led1, 2 bytes dados: [qtde:tempo]
#define BLINK_LED2 0x42 // pisca o led2
#define WRT_MSG 0x70 // escreve msg no display, 1o byte eh o endereco, 1o addr = 0x80
#define ANS_ACK 0x06 // byte de resposta para comando compreendido
#define ANS_NAK 0x15 // byte de resposta para comando nao compreendido
/* FIM DA DEFINICAO DE COMANDOS */

/* RETORNOS PARA VERIFICACAO DE DADOS */
typedef enum {
    ERR_STX,
    ERR_IGNORE_MSG,
    ERR_BCC,
    ERR_NAK,
    LIGA_LED1,
    LIGA_LED2,
    DESLIGA_LED1,
    DESLIGA_LED2,
    ERR_UNDETECTED
} en_comunicacao_t;
/* FIM DA DEFINICAO DE RETORNOS */

typedef struct {
    uint8_t count; // qtde de dados
    uint8_t buff[DATA_LEN]; // buffer
    short data_flag; // flag de sinalizacao
    uint8_t addr_from; // de quem veio a mensagem
    uint8_t command; // qual foi o comando enviado
} dados_t;

void uart_init(void);
short uart_check_rx(void);
uint8_t uart_receive_byte(void);
void uart_send_byte(uint8_t dbyte);

void send_string_data(char *string, dados_t *dados);
en_comunicacao_t check_data(dados_t *data); // analisa o tipo de dado
uint8_t calc_bcc(uint8_t *data); // calcula o bcc
void write_zero(dados_t *dados); // write zeros on buffer and count
// salva a string no campo de buff do data e atualiza o contador
void mk_msg(dados_t *data, uint8_t count, char *string);
// coloca a mensagem no protocolo e a envia via serial
void write_cmd(dados_t *data, uint8_t addr_to);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* COMUNICACAO_H */