#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdint.h>

// Configura la velocidad a 9600 para el reloj de 16MHz
void UART_Init(void);

// Manda un byte suelto cargándolo en el buffer circular
void UART_WriteByte(uint8_t dato);

// Manda un choclo de bytes seguidos
void UART_WriteBuffer(const uint8_t* buffer, uint16_t tamano);

// Te dice cuántas letras llegaron y todavía no leímos
uint8_t UART_Available(void);

// Saca el byte más viejo que está esperando en el buffer
uint8_t UART_ReadByte(void);

#endif /* UART_H_ */