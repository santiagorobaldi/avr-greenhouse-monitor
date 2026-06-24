#include "i2c.h"

void Setup_I2C(void) {
    // Configuramos la velocidad del bus a 100 kHz que es el estandar de la industria
    // F_SCL = F_CPU / (16 + 2 * TWBR * Prescaler)
    // Despejando TWBR con Prescaler = 1:
    // TWBR = (16.000.000 / 100.000 - 16) / 2 = 72
    TWBR = 72; 
    TWSR = 0; // Pre-escalador en 1 
}

void I2C_Start(void) {
    // Ponemos un 1 en TWINT (para limpiar la bandera y arrancar)
    // Ponemos un 1 en TWSTA (para generar la condición de START)
    // Ponemos un 1 en TWEN (para mantener el módulo encendido)
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
   
    // Nos quedamos esperando confirmacion 
    while ((TWCR & (1 << TWINT)) == 0); 
}

void I2C_Stop(void) {
    // Ponemos un 1 en TWINT (arrancar), TWSTO (generar STOP) y TWEN (mantener encendido)
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void I2C_Write(uint8_t dato) {
    TWDR = dato; // Cargamos el byte que queremos enviar en el registro de datos
    
    // Le damos la orden de enviar limpiando el TWINT y mantemos prendido el TWEN por seguridad
    TWCR = (1 << TWINT) | (1 << TWEN); 
    
    // Esperamos a que el hardware termine de mandar los 8 bits y lea el ACK 
    while (!(TWCR & (1 << TWINT))); 
}

uint8_t I2C_Read_Nack(void) {
    // Le decimos que lea un byte, pero NO activamos TWEA. 
    // Esto genera un NACK, diciéndole al esclavo (ej. RTC): "Este es el último byte, no mandes más".
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    // Esperamos que termine de llegar el byte
    while (!(TWCR & (1 << TWINT))); 
    return TWDR; // Devolvemos lo que llegó
}

uint8_t I2C_Read_Ack(void) {
    // Le decimos que lea, y SÍ activamos TWEA.
    // Esto genera un ACK, diciéndole al esclavo: "Llegó bien, preparame el siguiente registro".
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    
    // Esperamos que termine de llegar el byte
    while (!(TWCR & (1 << TWINT))); 
    return TWDR; // Devolvemos lo que llegó
}