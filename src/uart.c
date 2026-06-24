#include "uart.h"
#include <avr/interrupt.h>

#define UART_BUFFER_SIZE 128 // el mensaje mas largo es de 65 caracteres. con 128 nos sobra espacio para este buffer.

// Buffer Circular de Transmisión (TX)
static volatile uint8_t buffer_tx[UART_BUFFER_SIZE];
static volatile uint8_t tx_head = 0; // Índice de escritura
static volatile uint8_t tx_tail = 0; // Índice de lectura

// Buffer Circular de Recepción (RX)
static volatile uint8_t buffer_rx[UART_BUFFER_SIZE];
static volatile uint8_t rx_head = 0; // Índice de escritura
static volatile uint8_t rx_tail = 0; // Índice de lectura

void UART_Init(void) {

	UBRR0H = 0;
	UBRR0L = 103; // Para 16MHz y 9600 baudios
	
	// CONFIGURACIÓN DE UCSR0B:
	// RXCIE0: 1 -> Habilitamos interrupción de recepción completa.
	// RXEN0: 1 -> Habilitamos el receptor.
	// TXEN0: 1 -> Habilitamos el transmisor.
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1, UPM01:00 en 0 y USBS0 en 0 (1 bit)
}

void UART_WriteByte(uint8_t dato) {
	uint8_t siguiente = (tx_head + 1) % UART_BUFFER_SIZE;
	
	// Si el casillero que sigue es el que está leyendo la UART, esperamos.
	// esto genera una relacion de compromiso: Usamos un 'while' bloqueante para priorizar la integridad de los datos sobre el tiempo de ejecución.
	// Si el buffer se llena, el Background se frena unos milisegundos para garantizar que la telemetría no se corrompa,
	// delegando el destrabe a la ISR (Foreground) a medida que vacía la memoria.
	
	// en este problema esto no sucedera porque el buffer size es de 128 y los mensajes que mandamos son de maximo 65 caracteres. por lo que la condicion no deberia cumplirse nunca.
	// la pusimos por si hay alguna falla o si en un futuro se quieren mandar mensajes mas largos.
	while (siguiente == tx_tail);
	
	buffer_tx[tx_head] = dato;
	tx_head = siguiente;
	
	// Activamos la interrupción de registro de transmisión vacío para arrancar el despacho
	UCSR0B |= (1 << UDRIE0);
}

void UART_WriteBuffer(const uint8_t* buffer, uint16_t tamano) {
	for (uint16_t i = 0; i < tamano; i++) {
		UART_WriteByte(buffer[i]);
	}
}

uint8_t UART_Available(void) {
	// Calcula la cantidad de bytes que faltan leer del buffer de RX
	return (UART_BUFFER_SIZE + rx_head - rx_tail) % UART_BUFFER_SIZE;
}

uint8_t UART_ReadByte(void) {
	// Si no hay datos (head == tail), devuelve un byte nulo
	if (rx_head == rx_tail) {
		return 0;
	}
	uint8_t dato = buffer_rx[rx_tail];
	rx_tail = (rx_tail + 1) % UART_BUFFER_SIZE;
	return dato;
}

// ISR de Recepción Completa: El hardware nos entrega el byte y el driver lo almacena de inmediato
ISR(USART_RX_vect) {
	uint8_t dato_entrante = UDR0;
	uint8_t siguiente = (rx_head + 1) % UART_BUFFER_SIZE;
	
	// Si el buffer circular de entrada se llena, descartamos el byte viejo para proteger la memoria
	if (siguiente != rx_tail) {
		buffer_rx[rx_head] = dato_entrante;
		rx_head = siguiente;
	}
}

// ISR de Transmisión (Data Register Empty): Despacha bytes en segundo plano
ISR(USART_UDRE_vect) {
	if (tx_head != tx_tail) {
		UDR0 = buffer_tx[tx_tail];
		tx_tail = (tx_tail + 1) % UART_BUFFER_SIZE;
	} else {
		// Buffer vacío: apagamos la interrupción
		UCSR0B &= ~(1 << UDRIE0);
	}
}