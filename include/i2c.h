#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>

void Setup_I2C(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(uint8_t dato);
uint8_t I2C_Read_Nack(void);
uint8_t I2C_Read_Ack(void);

#endif