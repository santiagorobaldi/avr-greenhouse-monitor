#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "reloj.h"
#include "consola.h"
#include "i2c.h"
#include "control.h"

uint8_t T_muestreo = 5;
uint8_t contador_T = 0;

int main(void) {
	// Inicialización 
	Setup_Timer();
	Consola_Init();
	Setup_I2C();

	sei(); // Encendemos las interrupciones globales

	while (1) {
		// Levanta los bytes de hardware de la UART y alimenta el buffer de la consola
		Consola_ProcesarEntrada();

		// PROCESAMIENTO DE COMANDOS
		if (FLAG_linea_recibida == 1) {
			//  procesamiento de strings y comandos al módulo de la consola
			Consola_ProcesarComandos();
			
			// avisamos que recibimos bien el mensaje y habiltamos que se pueda escribir devuelta
			FLAG_linea_recibida = 0;
		}

		// TELEMETRÍA 
		if (flag_timer_1seg == 1) {
			flag_timer_1seg = 0;
			contador_T++;
			
			if (contador_T >= T_muestreo) {
				contador_T = 0;
				
				// lectura de sensores y lógica de alertas al módulo de ambiente
				Control_MonitorearInvernadero();
			}
		}
	}
	return 0;
}