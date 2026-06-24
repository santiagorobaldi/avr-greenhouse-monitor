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
	char mensaje_consola[120];
	uint8_t hora_bcd, minuto_bcd, seg_bcd, hora_dec, min_dec, seg_dec;
	
	// CONTADORES INDEPENDIENTES: Lógica de hilado fino para rastrear fallas consecutivas por variable
	static uint8_t contador_falla_temp = 0;
	static uint8_t contador_falla_hum = 0;

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
		uint8_t es_diurno = 0;

		// Rangos según horario comercial diurno (7 a 19) o nocturno
		if (hora_dec >= 7 && hora_dec < 19) {
			temp_min = 20; temp_max = 30; hum_min = 50; hum_max = 70;
			es_diurno = 1;
			} else {
			temp_min = 15; temp_max = 22; hum_min = 60; hum_max = 80;
			es_diurno = 0;
		}

		// Chequeo independiente para ver qué variable falló
		uint8_t alarma_temp = 0;
		uint8_t alarma_hum = 0;
		uint8_t estado_global = 0; // 0 = NORMAL, 1 = ALERTA

		if (dht11_temp < temp_min || dht11_temp > temp_max) {
			alarma_temp = 1;
			estado_global = 1;
		}
		if (dht11_hum < hum_min || dht11_hum > hum_max) {
			alarma_hum = 1;
			estado_global = 1;
		}

		// 1. REPORTE DE TELEMETRÍA PRINCIPAL (Punto 1 del enunciado)
		sprintf(mensaje_consola, "[%02d:%02d:%02d] T: %02d°C | H: %02d%% | Estado: %s\r\n",
		hora_dec, min_dec, seg_dec,
		dht11_temp, dht11_hum,
		(estado_global == 1) ? "ALERTA" : "NORMAL");
		Consola_Print(mensaje_consola);

		// 2. PROCESAMIENTO EVALUACIÓN DE TEMPERATURA (Independiente)
		if (alarma_temp == 1) {
			contador_falla_temp++;
			if (contador_falla_temp >= 2) {
				// Mandamos el mensaje exacto para la emergencia de temperatura
				sprintf(mensaje_consola, "[ALERTA] [%02d:%02d:%02d] Temperatura fuera de rango %s! Valor: %d°C\r\n",
				hora_dec, min_dec, seg_dec,
				es_diurno ? "diurno" : "nocturno",
				dht11_temp);
				Consola_Print(mensaje_consola);
				contador_falla_temp = 0; // Reinicia para esperar los próximos dos estados de alerta de temperatura
			}
			} else {
			// Si volvió a la normalidad, se limpia este contador específico para evitar acumulados viejos
			contador_falla_temp = 0;
		}

		// 3. PROCESAMIENTO EVALUACIÓN DE HUMEDAD (Independiente)
		if (alarma_hum == 1) {
			contador_falla_hum++;
			if (contador_falla_hum >= 2) {
				// Mandamos el mensaje exacto para la emergencia de humedad
				sprintf(mensaje_consola, "[ALERTA] [%02d:%02d:%02d] Humedad fuera de rango %s! Valor: %d%%\r\n",
				hora_dec, min_dec, seg_dec,
				es_diurno ? "diurno" : "nocturno",
				dht11_hum);
				Consola_Print(mensaje_consola);
				contador_falla_hum = 0; // Reinicia para esperar los próximos dos estados de alerta de humedad
			}
			} else {
			// Si volvió a la normalidad, se limpia este contador específico
			contador_falla_hum = 0;
		}

		} else {
		// Alerta de seguridad por falla física del DHT11
		sprintf(mensaje_consola, "[%02d:%02d:%02d] ERROR CRITICO: Sensor DHT11 no responde.\r\n", hora_dec, min_dec, seg_dec);
		Consola_Print(mensaje_consola);
	}
}