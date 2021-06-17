/*
 * wifi.c
 *
 *  Created on: Jun 1, 2021
 *      Author: aleja
 */
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "wifi.h"
#include "stm32f0xx_hal.h"
#include "myDelay.h"
#include <string.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static  _sEspData esp8266Data;

static uint8_t nveces, comfail, stateInit, stateConection, stateSendData, numBytesAt;

static uint8_t rxDatas[2];

static delay_t timeoutesp;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

static void inicializaMEF();

static void esp8266MEF(_sEsp *espConfig, uint8_t modo);

static uint8_t respuesta_esp8266(uint8_t *rawdata);

static void sendCommand_esp8266(uint8_t *tempBuff, uint8_t nbytes);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/

extern UART_HandleTypeDef huart4;

/*************************************************************************************************************************/
/***************************************************   INICIA RUTINAS DE TRANSMISION USART  ******************************/
/*************************************************************************************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if(huart->Instance==USART4) {
			HAL_UART_Receive_IT(&huart4, rxDatas, 1);
		esp8266Data.inBuff[esp8266Data.rxWrite++]=rxDatas[0];
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance==USART4) {
		stateSendData=TRUE;
	}
}

void sendCommand_esp8266(uint8_t *tempBuff, uint8_t nbytes){
	HAL_UART_Transmit_IT(&huart4, tempBuff, nbytes);
}



/*************************** Funciones privadas********************************/

static void inicializaMEF(){
	esp8266Data.comando=ATE0;
	esp8266Data.respuesta=IDLE;
	delayConfig(&timeoutesp, 0);
	nveces=0;
	comfail=FALSE;
}


static void esp8266MEF(_sEsp *esp8266, uint8_t modo)
{
	numBytesAt=0;
	stateInit=FALSE;
	comfail=FALSE;
	if (esp8266Data.respuesta==INCOMMINGRESPONSE){
		if(delayRead(&timeoutesp)){
			comfail=TRUE;
			nveces++;
		}
	}
	else{

		switch(esp8266Data.comando )
		{
			case ATE0:
				if (esp8266Data.respuesta==OK){
					esp8266Data.comando = CWMODE_DEF;
					esp8266Data.respuesta=IDLE;
					nveces=0;
				}
				else{
					if (esp8266Data.respuesta==IDLE){
						esp8266Data.respuesta=INCOMMINGRESPONSE;
						sprintf(esp8266Data.comandoAT,"ATE0\r\n");
						numBytesAt=strlen(esp8266Data.comandoAT);
						delayWrite(&timeoutesp, 1000);
					}

				}
			break;
			case CWMODE_DEF:
				if (esp8266Data.respuesta==OK){
					esp8266Data.comando = CWDHCP;
					esp8266Data.respuesta=IDLE;
					nveces=0;
				}
				else{
					if (esp8266Data.respuesta==IDLE){
						esp8266Data.respuesta=INCOMMINGRESPONSE;
						sprintf(esp8266Data.comandoAT,"AT+CWMODE_DEF=%d\r\n", esp8266->cwMode);
						numBytesAt=strlen(esp8266Data.comandoAT);
						delayWrite(&timeoutesp, 1000);
					}

				}
			break;
			case CWDHCP:
				if (esp8266Data.respuesta==OK){
					esp8266Data.comando = CWJAP_DEF;
					esp8266Data.respuesta=IDLE;
					nveces=0;
				}
				else{
					if (esp8266Data.respuesta==IDLE){
						esp8266Data.respuesta=INCOMMINGRESPONSE;
						sprintf(esp8266Data.comandoAT,"AT+CWDHCP=%d,%d\r\n", esp8266->cwDHCP[0],esp8266->cwDHCP[1] );
						numBytesAt=strlen(esp8266Data.comandoAT);
						delayWrite(&timeoutesp, 1000);
					}

				}
			break;
			case CWJAP_DEF:
				if (esp8266Data.respuesta==OK){
					esp8266Data.comando = CIPSTART;
					esp8266Data.respuesta=IDLE;
					nveces=0;
				}
				else{
					if (esp8266Data.respuesta==IDLE){
						esp8266Data.respuesta=INCOMMINGRESPONSE;
						sprintf(esp8266Data.comandoAT,"AT+CWJAP_DEF=\"%s\",\"%s\"\r\n", esp8266->ssid, esp8266->pass);
						numBytesAt=strlen(esp8266Data.comandoAT);
						delayWrite(&timeoutesp, 5000);
					}

				}
			break;
			case CIPSTART:
				if (esp8266Data.respuesta==OK){
					esp8266Data.comando = CIPMODE;
					esp8266Data.respuesta=IDLE;
					nveces=0;
				}
				else{
					if (esp8266Data.respuesta==IDLE){
						esp8266Data.respuesta=INCOMMINGRESPONSE;
						switch(modo)
						case UDP:
							sprintf(esp8266Data.comandoAT,"AT+CIPSTART=\"UDP\",\"%s\",%d,%d,0\r\n", esp8266->ipremota, esp8266->remoteport, esp8266->localport);
						break;
						case TCP:
							sprintf(esp8266Data.comandoAT,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", esp8266->ipremota, esp8266->remoteport);
						break;
						numBytesAt=strlen(esp8266Data.comandoAT);
						delayWrite(&timeoutesp, 3000);
					}

				}
			break;
			case CIPMODE:
				if (esp8266Data.respuesta==OK){
					esp8266Data.comando = CIPSEND;
					esp8266Data.respuesta=IDLE;
					nveces=0;
				}
				else{
					if (esp8266Data.respuesta==IDLE){
						esp8266Data.respuesta=INCOMMINGRESPONSE;
						sprintf(esp8266Data.comandoAT,"AT+CIPMODE=%d\r\n", esp8266->cipMode);
						numBytesAt=strlen(esp8266Data.comandoAT);
						delayWrite(&timeoutesp, 1000);
					}

				}
			break;
			case CIPSEND:
				if (esp8266Data.respuesta==TRANSPARENT){
					esp8266Data.comando = TRANSPARENT;
					stateInit=TRUE;
				}
				else{
					if (esp8266Data.respuesta==IDLE){
						esp8266Data.respuesta=INCOMMINGRESPONSE;
						sprintf(esp8266Data.comandoAT,"AT+CIPSEND\r\n");
						numBytesAt=strlen(esp8266Data.comandoAT);
						delayWrite(&timeoutesp, 1000);
					}

				}

			break;
			default:
				inicializaMEF();
				;

		}
	}
	if(numBytesAt!=0)
		sendCommand_esp8266(esp8266Data.comandoAT,numBytesAt);
	if(comfail){
		esp8266Data.respuesta=IDLE;
		if(nveces>3){
			esp8266Data.comando=ATE0;
			nveces=0;
		}
	}
}


/*************************** Funciones públicas********************************/

uint8_t wifiInit( _sEsp *espConfig, uint8_t modo){
	esp8266MEF(espConfig,modo);
	return stateInit;
}


uint8_t wifiSend(uint8_t *data, uint8_t longitud){
	stateSendData=FALSE;
	sendCommand_esp8266(data,longitud);
	return stateSendData;
}

void wifiRecibeConfig(){
	uint8_t indice=0, respuesta=INCOMMINGRESPONSE;
	if(esp8266Data.rxRead!=esp8266Data.rxWrite){
		while(esp8266Data.rxRead!=esp8266Data.rxWrite){
			esp8266Data.respuestaAt[indice++]=esp8266Data.inBuff[esp8266Data.rxRead++];
		 }
		if(strstr(esp8266Data.respuestaAt,">")!=NULL){
			respuesta=TRANSPARENT;
		}
		else{
			if(strstr(esp8266Data.respuestaAt,"OK")!=NULL){
				respuesta=OK;
			}
		}
		esp8266Data.respuesta=respuesta;
	}
}

void wifiRecibe(){
	//Para implementar más adelante.
	// se usa cuando ya está en modo transparente para leer el buffer de entrada.

}

void iniciarMefWifi(){
	inicializaMEF();
}






