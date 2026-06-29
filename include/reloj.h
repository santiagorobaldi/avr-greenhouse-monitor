#ifndef RELOJ_H_
#define RELOJ_H_

#include <stdint.h>

// Única bandera necesaria para el main
extern volatile uint8_t flag_timer_1seg;

// Configura el Timer1 para interrumpir a 1Hz exacto
void Setup_Timer(void);

#endif /* RELOJ_H_ */