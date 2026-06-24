#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdint.h>

// Bandera pública para que el main sepa si hubo actividad o si se cambió el tiempo de muestreo
extern volatile unsigned char FLAG_linea_recibida;

// Inicializa el sistema de la terminal
void Consola_Init(void);

// Tarea de fondo que barre los bytes de la UART y arma el string privado
void Consola_ProcesarEntrada(void);

// NUEVA FUNCIÓN: Agarra el comando que se terminó de escribir, lo interpreta y ejecuta la acción
void Consola_ProcesarComandos(void);

// Manda strings enteros a la terminal virtual
void Consola_Print(const char* texto);

#endif /* CONSOLA_H_ */