#include "consola.h"
#include "uart.h"
#include "i2c.h"
#include <stdio.h>
#include <string.h>

// Traemos las variables globales del main para poder configurarlas con los comandos
extern uint8_t T_muestreo;
extern uint8_t contador_T;

// Bandera de comunicación que maneja el semáforo lógico con el main
volatile unsigned char FLAG_linea_recibida = 0;

// Buffer privado inmune a corrupciones externas
static char BufferRX[30];

// Función auxiliar local para empaquetar los datos del RTC (Estilo cátedra)
static uint8_t decimal_a_bcd(uint8_t decimal) {
	return ((decimal / 10) << 4) | (decimal % 10);
}

void Consola_Init(void) {
	UART_Init();
}

void Consola_ProcesarEntrada(void) {
	static int IndexRX = 0; // Índice estático local (Copiado exacto del Ejemplo 5 del profe)
	
	// SEMÁFORO: Si el comando anterior no se procesó, no sacamos bytes de la UART
	if (FLAG_linea_recibida == 1) {
		return;
	}
	
	while (UART_Available() > 0) {
		char RX_Data = (char)UART_ReadByte();
		
		if (IndexRX >= 28) {
			IndexRX = 0;
		}
		
		if (RX_Data != '\r') {
			BufferRX[IndexRX++] = RX_Data;
		}
		else {
			BufferRX[IndexRX++] = '\r';
			BufferRX[IndexRX] = '\0'; // Nulo al final para cerrar el string de C
			IndexRX = 0;
			FLAG_linea_recibida = 1;  // Cerramos el semáforo, comando listo para procesar
		}
	}
}

void Consola_ProcesarComandos(void) {
	char mensaje_consola[100]; // Buffer temporal local para respuestas

	// Comando: SET_TIME=HH:MM:SS
	if (strncmp(BufferRX, "SET_TIME=", 9) == 0) {
		int h, m, s;
		if (sscanf(BufferRX + 9, "%d:%d:%d", &h, &m, &s) == 3) {
			if (h >= 0 && h <= 23 && m >= 0 && m <= 59 && s >= 0 && s <= 59) {
				I2C_Start();
				I2C_Write(0xD0);
				I2C_Write(0x00);
				I2C_Write(decimal_a_bcd((uint8_t)s));
				I2C_Write(decimal_a_bcd((uint8_t)m));
				I2C_Write(decimal_a_bcd((uint8_t)h));
				I2C_Stop();
				Consola_Print("-> COMANDO ACEPTADO: RTC Actualizado.\r\n");
				} else {
				Consola_Print("-> ERROR: Valores de tiempo fuera de rango.\r\n");
			}
			} else {
			Consola_Print("-> ERROR: Formato invalido. Use SET_TIME=HH:MM:SS\r\n");
		}
	}
	
	// Comando: SET_TM=SS
	else if (strncmp(BufferRX, "SET_TM=", 7) == 0) {
		int nuevo_t;
		if (sscanf(BufferRX + 7, "%d", &nuevo_t) == 1) {
			if (nuevo_t >= 2 && nuevo_t <= 60) {
				T_muestreo = (uint8_t)nuevo_t;
				contador_T = 0; // Reseteamos contador para arrancar la nueva tasa limpio
				sprintf(mensaje_consola, "-> COMANDO ACEPTADO: Tasa de muestreo = %d seg.\r\n", T_muestreo);
				Consola_Print(mensaje_consola);
				} else {
				Consola_Print("-> ERROR: T debe estar entre 2 y 60 segundos.\r\n");
			}
			} else {
			Consola_Print("-> ERROR: Formato invalido. Use SET_TM=SS\r\n");
		}
	}
	else {
		Consola_Print("-> ERROR: Comando desconocido.\r\n");
	}
}

void Consola_Print(const char* texto) {
	uint16_t i = 0;
	while (texto[i] != '\0') {
		UART_WriteByte((uint8_t)texto[i]);
		i++;
	}
}