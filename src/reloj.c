#include "reloj.h"

// variables para el main
volatile uint8_t flag_timer_10ms = 0; // pasaron 10ms - lo dejamos porque a futuro es mas escalable tener un flag de 10ms y poder usarlo que tener uno de 1seg.
volatile uint8_t flag_timer_1seg = 0; // paso un segundo

void Setup_Timer(void) {
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC, Prescaler 64
	OCR1A = 2499; // 10ms a 16MHz
	TIMSK1 = (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
	flag_timer_10ms = 1; 
	static uint16_t contador_ticks = 0;
	contador_ticks++;
	if (contador_ticks >= 100) {
		contador_ticks = 0;
		flag_timer_1seg = 1;
	}
}