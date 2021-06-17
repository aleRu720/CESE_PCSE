/*
 * wifi.h
 *
 *  Created on: Jun 1, 2021
 *      Author: aleja
 */

#ifndef INC_WIFI_H_
#define INC_WIFI_H_

#include "stdint.h"

#define FALSE 			0
#define TRUE			1
#define NORMALMODE		0
#define PASTHRUEMODE	1

// ESTADOS de la MEF de comunicación WIFI con el ESP
typedef enum{
			AT,
			OK,
			ATE0,
			CWMODE_DEF,
			CWDHCP,
			CWJAP_DEF,
			CIFSR,
			CIPSTART,
			CIPMODE,
			CIPSEND,
			TRANSPARENT,
			FAIL,
			IDLE,
			USBCOM,
			USARTCOM,
			AWAITINGRESPONSE,
			INCOMMINGRESPONSE,
			AUTOMATIC
} _eEstadoESP;

_eEstadoESP espState;


typedef enum{
	UDP,
	TCP
}_eComType;

_eComType comTypeEsp;

//Estructura de configuración para la conectar el ESP, se carga vía USB

typedef struct{
		uint8_t cwMode; 		//!< 1,2,3 - Ap, softStation - Ambos modos
		uint8_t cipMode;		//!< 0,1	-modo normal	- 	modo transparente(Wifi pasThrue)
		uint8_t cwAutocon;		//!< 0,1 	- no se conecta automaticamente en power-up		-		se conecta automaticamente en power-up
		uint8_t cwDHCP[2];			//!< dig1|dig2		-
		_eComType conectionType;//!<	UDP	-	TCP	-	SERIAL
		uint8_t ssid[50];		//!<	SSID  de la red a conectarse
		uint8_t pass[50];		//!<	PASS de la red a conectarse
		uint8_t ipremota[15];	//!<	IP del servidor remoto
		uint16_t localport;		//!<	Puerto local para UDP
		uint16_t remoteport;	//!<	Puerto remoto para UDP
		uint8_t confState; 		//!<	0 no configurado - 1 listo para conectar
} _sEsp;

_sEsp espConfig;


//Estructura para el manejo de los datos del ESP Envío de comandos, recepción de respuestas, envio y recepción de datos una vez conectado
typedef struct{
	uint8_t estado;
	uint8_t comando;
	uint8_t	respuesta;
	uint8_t comandoAT[256];
	uint8_t respuestaAt[256];
	uint8_t outBuff[256];
	uint8_t inBuff[256];
	uint8_t txRead;
	uint8_t txWrite;
	uint8_t rxRead;
	uint8_t rxWrite;
}_sEspData;

// Funciones Globales
uint8_t wifiInit( _sEsp *espConfig , uint8_t modo);
uint8_t wifiSend(uint8_t *, uint8_t );
void wifiRecibeConfig();
void wifiRecibe();
void iniciarMefWifi();




#endif /* INC_WIFI_H_ */
