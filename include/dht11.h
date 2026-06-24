#ifndef DHT11_H_
#define DHT11_H_

#include <avr/io.h>

// Variables globales para almacenar la última lectura válida
extern uint8_t dht11_temp;
extern uint8_t dht11_hum;

// Prototipo de la función principal del sensor
// Devuelve 1 si leyó bien los datos y el checksum es correcto.
// Devuelve 0 si el sensor está desconectado, roto o hubo error de lectura.
uint8_t Leer_DHT11(void);

#endif /* DHT11_H_ */