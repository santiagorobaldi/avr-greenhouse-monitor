# AVR Greenhouse Monitor

Sistema embebido bare-metal desarrollado en C para microcontroladores AVR que monitorea las condiciones ambientales de un invernadero mediante un sensor DHT11 y un RTC DS3232. El sistema envía telemetría periódica por UART, permite la configuración remota mediante comandos y genera alarmas cuando las variables ambientales salen de los rangos establecidos.

## Características

* Arquitectura **Background / Foreground** basada en interrupciones.
* Comunicación serie **UART 9600 bps (8N1)**.
* Recepción de comandos mediante interrupciones UART RX.
* Transmisión no bloqueante mediante interrupciones UART TX.
* Comunicación **I²C** con RTC DS3232.
* Lectura de temperatura y humedad mediante sensor **DHT11**.
* Temporización mediante interrupciones periódicas de Timer.
* Configuración remota de la hora del sistema.
* Configuración dinámica de la tasa de muestreo.
* Detección de condiciones ambientales fuera de rango.
* Generación de mensajes de alarma.

---

## Hardware Utilizado

### Microcontrolador

* AVR ATmega328P (o compatible)

### Sensores y Periféricos

* RTC DS3232
* Sensor DHT11
* Terminal Serie Virtual (Proteus)

### Interfaces

* UART
* I²C

---

## Esquema General

```text
                 +-------------+
                 |   DS3232    |
                 +------+------+
                        |
                       I²C
                        |
+--------+      +-------+-------+      +---------------+
| DHT11  |----->|      AVR      |----->| Virtual UART  |
+--------+      +-------+-------+      +---------------+
                        |
                     Timer
```

---

## Arquitectura de Software

El sistema implementa una arquitectura **Background / Foreground**.

### Foreground (Interrupciones)

* UART RX
* UART TX
* Timer periódico

Las ISR se encargan únicamente de capturar eventos y actualizar flags o buffers.

### Background

El bucle principal ejecuta:

* Procesamiento de comandos.
* Lectura del RTC.
* Lectura del DHT11.
* Evaluación de rangos ambientales.
* Generación de telemetría.
* Gestión de alarmas.

---

## Rangos de Operación

| Ventana Horaria       | Temperatura Óptima | Humedad Óptima |
| --------------------- | ------------------ | -------------- |
| Día (07:00 - 18:59)   | 20°C - 30°C        | 50% - 70%      |
| Noche (19:00 - 06:59) | 15°C - 22°C        | 60% - 80%      |

---

## Comandos Soportados

### Configurar Hora

```text
SET_TIME=HH:MM:SS
```

Ejemplo:

```text
SET_TIME=14:35:00
```

Actualiza la hora almacenada en el RTC.

---

### Configurar Tasa de Muestreo

```text
SET_TM=SS
```

Ejemplo:

```text
SET_TM=10
```

Configura el intervalo de reporte entre 2 y 60 segundos.

---

## Ejemplo de Telemetría

```text
[14:35:20] T:24°C | H:65% | Estado:NORMAL
```

---

## Ejemplo de Alarma

```text
[ALERTA] [14:35:20] Temperatura fuera de rango diurno! Valor: 34°C
```

---

## Tecnologías Utilizadas

* C
* AVR-GCC
* AVR Libc
* UART
* I²C (TWI)
* Timers
* Interrupciones
* Proteus

---

## Estructura del Proyecto

```text
.
├── src/
│   ├── main.c
│   ├── uart.c
│   ├── i2c.c
│   ├── reloj.c
│   └── dht11.c
│
├── include/
│   ├── uart.h
│   ├── i2c.h
│   ├── reloj.h
│   └── dht11.h
│
├── proteus/
│   └── simulacion.pdsprj
│
└── README.md
```

---

## Objetivos Académicos

Este proyecto fue desarrollado como trabajo práctico de Sistemas con Microprocesadores y tiene como objetivo integrar conceptos fundamentales de sistemas embebidos:

* Arquitectura Background / Foreground.
* Programación bare-metal.
* Comunicación UART.
* Comunicación I²C.
* Manejo de interrupciones.
* Sensores digitales.
* Temporización por hardware.
* Diseño modular de firmware.

---

## Autor

**Santiago Robaldi y Lucila Juri**

Estudiantes de Ingeniería en Computación.
