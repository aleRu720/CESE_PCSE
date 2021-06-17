/*
 * comSerial.h
 *
 *  Created on: 1 jun. 2021
 *      Author: aleja
 */

#ifndef INC_COMSERIAL_H_
#define INC_COMSERIAL_H_



#include "stdint.h"
#include <myDelay.h>

// Unión para el ensamblar o separa valores 

typedef union {
	float f32;
	uint32_t i32;
	int16_t i16[2];
	uint16_t ui16[2];
	int8_t ui8[4];
} _uword;

_uword myWord;

//EStructura que contiene los buffer de recepción y trasnmisión así como también los indices de lectura y escritura de esos buffers

typedef struct{
	uint8_t outBuff[256];
	uint8_t inBuff[256];
	uint8_t txRead;
	uint8_t txWrite;
	uint8_t rxRead;
	uint8_t rxWrite;
	uint8_t rxdata[50];
	uint8_t dir;
}_sSerial;

_sSerial usbCom;

//Funciones Globales para recibir y enviar datos
void comSerialRecibe();
uint8_t comSerialSend(_sSerial *usbCom);




#endif /* INC_COMSERIAL_H_ */
