#include "control.h"
#include "i2c.h"
#include "dht11.h"
#include "consola.h"
#include <stdio.h>

// Funciones BCD (las sacamos del main para que no molesten)
static uint8_t bcd_a_decimal(uint8_t bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void Control_MonitorearInvernadero(void) {
	char mensaje_consola[100];
	uint8_t hora_bcd, minuto_bcd, seg_bcd, hora_dec, min_dec, seg_dec;
	static uint8_t tramas_en_alarma = 0; // Se mantiene entre llamadas

	// LECTURA DEL RTC (Módulo I2C)
	I2C_Start();
	I2C_Write(0xD0);
	I2C_Write(0x00);
	I2C_Start();
	I2C_Write(0xD1);
	seg_bcd = I2C_Read_Ack();
	minuto_bcd = I2C_Read_Ack();
	hora_bcd = I2C_Read_Nack();
	I2C_Stop();

	seg_dec = bcd_a_decimal(seg_bcd);
	min_dec = bcd_a_decimal(minuto_bcd);
	hora_dec = bcd_a_decimal(hora_bcd);

	// LECTURA DEL DHT11
	if (Leer_DHT11() == 1) {
		uint8_t temp_min, temp_max, hum_min, hum_max;

		// Rangos dinámicos dependiendo de si es de día o de noche
		if (hora_dec >= 7 && hora_dec < 19) {
			temp_min = 20; temp_max = 30; hum_min = 50; hum_max = 70;
			} else {
			temp_min = 15; temp_max = 22; hum_min = 60; hum_max = 80;
		}

		// Evaluación de anomalías
		uint8_t hay_alarma = 0;
		if (dht11_temp < temp_min || dht11_temp > temp_max || dht11_hum < hum_min || dht11_hum > hum_max) {
			hay_alarma = 1;
		}
		
		sprintf(mensaje_consola, "[%02d:%02d:%02d] T: %d°C | H: %d%% | Estado: %s\r\n",
		hora_dec, min_dec, seg_dec, dht11_temp, dht11_hum,
		hay_alarma ? "ALERTA" : "NORMAL");
		Consola_Print(mensaje_consola);

		if (hay_alarma == 1) {
			tramas_en_alarma++;
			if (tramas_en_alarma >= 2) {
				Consola_Print("    *** EMERGENCIA: Variable(s) fuera de rango optimo ***\r\n");
				tramas_en_alarma = 0; // Se reinicia para esperar otras 2 tramas
			}
			} else {
			// Si todo volvió a la normalidad, limpiamos el contador
			tramas_en_alarma = 0;
		}

		} else {
		// Alerta por si el cable del sensor se desconecta en vivo
		sprintf(mensaje_consola, "[%02d:%02d:%02d] ERROR CRITICO: Sensor DHT11 no responde.\r\n", hora_dec, min_dec, seg_dec);
		Consola_Print(mensaje_consola);
	}
}