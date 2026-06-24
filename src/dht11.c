#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "dht11.h"
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t dht11_temp = 0;
uint8_t dht11_hum = 0;

uint8_t Leer_DHT11(void) {
	uint8_t bits[5] = {0, 0, 0, 0, 0};
	uint8_t i, j;
	uint16_t timeout;

	// 1. EL "DESPERTADOR": Enviar pulso de START (Sección 5 del Datasheet)
	DDRB |= (1 << PB4);   // Configuramos PB4 como salida
	PORTB &= ~(1 << PB4); // Tiramos la línea a 0V
	_delay_ms(18);        // Mantenemos 0V por al menos 18ms
	PORTB |= (1 << PB4);  // Soltamos la línea a 5V
	DDRB &= ~(1 << PB4);  // Configuramos PB4 como entrada para escuchar

	// SECCIÓN CRÍTICA: Apagamos interrupciones para no perder la ventana de microsegundos
	cli();

	// 2. ESPERAR RESPUESTA (Acknowledge de 80us)
	timeout = 10000;
	while (PINB & (1 << PB4)) {
		if (--timeout == 0) { sei(); return 0; } // Timeout: Sensor desconectado
	}

	timeout = 10000;
	while (!(PINB & (1 << PB4))) {
		if (--timeout == 0) { sei(); return 0; }
	}

	timeout = 10000;
	while (PINB & (1 << PB4)) {
		if (--timeout == 0) { sei(); return 0; }
	}

	// 3. LEER LOS 40 BITS DE DATOS
	for (j = 0; j < 5; j++) {
		for (i = 0; i < 8; i++) {
			
			// Esperar a que el pulso suba de 0 a 1 (Fin de los 50us en bajo)
			timeout = 10000;
			while (!(PINB & (1 << PB4))) {
				if (--timeout == 0) { sei(); return 0; }
			}

			// Ventana de decisión empírica para distinguir '0' (28us) de '1' (70us)
			_delay_us(30);

			if (PINB & (1 << PB4)) {
				// Si pasados 30us sigue en 1, el pulso es largo. Es un '1' lógico.
				bits[j] |= (1 << (7 - i));
				
				// Ahora debemos esperar a que baje a 0 para buscar el siguiente bit
				timeout = 10000;
				while (PINB & (1 << PB4)) {
					if (--timeout == 0) { sei(); return 0; }
				}
			}
			// Si pasados 30us ya estaba en 0, el pulso era corto. Es un '0' lógico.
			// Como el arreglo se inicializó en ceros, no hace falta hacer nada.
		}
	}

	// FIN DE SECCIÓN CRÍTICA: Volvemos a encender las interrupciones
	sei();

	// 4. VALIDACIÓN DE INTEGRIDAD (Checksum)
	// Sumamos los primeros 4 bytes y lo comparamos con el 5to byte recibido
	if ((uint8_t)(bits[0] + bits[1] + bits[2] + bits[3]) == bits[4]) {
		dht11_hum = bits[0];  // Byte 1: Humedad Entera
		dht11_temp = bits[2]; // Byte 3: Temperatura Entera
		return 1; // Lectura exitosa
	}

	return 0; // Falló el Checksum (Hubo ruido eléctrico)
}