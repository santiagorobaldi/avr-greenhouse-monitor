#include "reloj.h"
#include <avr/interrupt.h>

volatile uint8_t flag_timer_1seg = 0; // Paso un segundo

void Setup_Timer(void) {
	TCCR1A = 0;
	
	// Configuración: Modo CTC (WGM12) y Prescaler de 1024 (CS12 y CS10)
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
	
	// Cuenta exacta calculada para 1 segundo a 16MHz
	OCR1A = 15624; 
	
	// Habilitamos la interrupción por comparación (Match A)
	TIMSK1 = (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
	// El hardware entra acá exactamente 1 vez por segundo. Cero overhead de software.
	flag_timer_1seg = 1;
}