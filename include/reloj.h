#ifndef RELOJ_H_
#define RELOJ_H_

#include <avr/io.h>
#include <avr/interrupt.h>

// extern avisa que las variables existen y las definimos en el .c
extern volatile uint8_t flag_timer_10ms;
extern volatile uint8_t flag_timer_1seg;

void Setup_Timer(void);

#endif