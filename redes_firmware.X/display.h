#ifndef DISPLAY_H
#define	DISPLAY_H

// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

typedef struct {
    uint8_t NeedsRedraw;
    uint8_t Memory[2][16];
} lcd_t;

void lcd_init(lcd_t *lcd); // Inicializa o LCD
void lcd_mem_clean(lcd_t *lcd); // insere espacos na memoria
int lcd_write(lcd_t *lcd, uint8_t row, uint8_t col, char *string); // escreve na area de memoria
void lcd_runtime(lcd_t *lcd); // atualiza LCD
void lcd_clean(void); // Limpa a tela do display
void lcd_clean_all(lcd_t *lcd); // limpa memoria e lcd

void DelayFor18TCY(void);
void DelayPORXLCD(void);
void DelayXLCD(void);

extern void delay_ms(uint16_t milis);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* DISPLAY_H */