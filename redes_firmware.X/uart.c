/*
 * File:   uart.c
 * Author: Luis Felipe Kunzler
 *
 * Created on 4 de Junho de 2018, 00:37
 */
#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

#include "uart.h"

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