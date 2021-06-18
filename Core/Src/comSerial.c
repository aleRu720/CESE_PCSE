/*
 * comSerial.c
 *
 *  Created on: 1 jun. 2021
 *      Author: aleja
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "myDelay.h"
#include "comSerial.h"
#include "stm32f0xx_hal.h"
#include "wifi.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define NOCONF	0
#define READYTOCONF	1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/


/* USER CODE BEGIN PV */

static uint8_t indexrx=0, headerrx=0, timeoutrx=0, nbytesrx=0, cksrx=0, estadoUSB;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

static void enviar_byte(_sSerial *);
static void decodeHeader(_sSerial *);
static void DecodeCommand(_sSerial *);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//extern _sEsp espConfig;

/* USER CODE END 0 */

// Recibe como parámetro un puntero a la estructura de datos, chequea los indices, arma el paquete uy lo envía 
void enviar_byte(_sSerial *data){
	uint8_t nbytes;
	uint8_t tempBuff[256];

	if(data->txRead>data->txWrite )
		nbytes=((256-data->txRead) + data->txWrite);
	else
		nbytes=(data->txWrite - data->txRead);

	if(nbytes!=0){

		for(uint8_t a=0;a<nbytes;a++)
					tempBuff[a]=data->outBuff[data->txRead+a];
		estadoUSB=CDC_Transmit_FS(tempBuff, nbytes);
	}
	data->txRead=data->txWrite;
}

// Decodifica el mensaje recibido por USB(puerto serie virtual), para ver si es util o lo descarta.
void decodeHeader(_sSerial *data){
	unsigned char msb_nbytes=0;
	while(data->rxRead!=data->rxWrite ){
		switch(headerrx){
			case 0:
			if(data->inBuff[(data->rxRead)++] == 'U'){
				headerrx = 1;
				timeoutrx = 5;
			}
			break;
			case 1:
			if(data->inBuff[data->rxRead] == 'N')
			{
				(data->rxRead)++;
				headerrx = 2;
			}
			else{
				if(data->inBuff[data->rxRead]!='U')
				headerrx = 0;

			}
			break;
			case 2:
			if(data->inBuff[data->rxRead] == 'E')
			{
				(data->rxRead)++;
				headerrx = 3;
			}
			else{
				if(data->inBuff[data->rxRead]!='U')
				headerrx = 0;
				else
				headerrx = 1;
			}
			break;
			case 3:
			if(data->inBuff[data->rxRead] == 'R'){
				(data->rxRead)++;
				headerrx = 4;
			}
			else{
				if(data->inBuff[data->rxRead]!='U')
				headerrx = 0;
				else
				headerrx = 1;
			}
			break;
			case 4:
			nbytesrx = data->inBuff[(data->rxRead)++];
			headerrx = 5;
			break;
			case 5:
			msb_nbytes = data->inBuff[(data->rxRead)++];
			headerrx = 6;
			break;
			case 6:
			if(data->inBuff[(data->rxRead)++] == ':'){
				headerrx = 7;
				cksrx = 'U' ^ 'N' ^ 'E' ^ 'R' ^ nbytesrx ^ msb_nbytes ^ ':';
				data->rxdata[0] = nbytesrx;
				indexrx = 1;
			}
			else{
				if(data->inBuff[(data->rxRead)]!='U')
				headerrx = 0;
				else
				headerrx =1;
			}
			break;
			case 7:
			if(nbytesrx > 1){
				cksrx ^= data->inBuff[(data->rxRead)];
				data->rxdata[indexrx++]= data->inBuff[(data->rxRead)++];
			}
			nbytesrx--;
			if(nbytesrx == 0){
				headerrx = 0;
				if(data->inBuff[(data->rxRead)] == cksrx)
				DecodeCommand(data);
			}
			break;
		}
	}
}


//Si el mensaje recibido esta completo, arma el paquete de respuesta y ejecuta las acciones asociadas al comando recibido

void DecodeCommand(_sSerial *data)
{
    uint8_t inicioTx=0, cheksum=0, indexEsp=0;
   uint8_t  buffer[256], tipo=0;
   

    buffer[inicioTx++]='U';
    buffer[inicioTx++]='N';
    buffer[inicioTx++]='E';
    buffer[inicioTx++]='R';
    buffer[inicioTx++]=0x00;
    buffer[inicioTx++]=0x00;
    buffer[inicioTx++]=':';

    switch(data->rxdata[1]) {
        case 0xA0: //Hacer algo
            buffer[inicioTx++]=0xA0;
            
            buffer[4]=0x02;
            break;
        case 0xA1://Hacer algo
            buffer[inicioTx++]=0xA1;

            buffer[4]=0x04;
            break;
        case 0xA2: //Hacer algo
			buffer[inicioTx++]=0xA2;

			buffer[4]=0x04;
            break;
        case 0xA3: //Valor del ADC CH_2
			buffer[inicioTx++]=0xA3;

			buffer[4]=0x04;
        case 0xA4://Hacer algo
        	buffer[inicioTx++]=0xA4;

			buffer[4]=0x02;
			break;
        case 0xA5://Hacer algo
			buffer[inicioTx++]=0xA5;

			buffer[4]=0x04;
			break;
        case 0xA6://Hacer algo
			buffer[inicioTx++]=0xA6;

			buffer[4]=0x02;
            break;
        case 0xA7://Hacer algo
        	buffer[inicioTx++]=0xA7;

        	buffer[4]=0x02;
            break;
        case 0xA8://Hacer algo
        	buffer[inicioTx++]=0xA8;

        	buffer[4]=0x02;
            break;

        case 0xEE://Hacer algo
              	buffer[inicioTx++]=0xEE;

              	buffer[4]=0x02;
                  break;
        case 0xEF://Configuración de los parámetros de conexión vía USB
			buffer[inicioTx++]=0xEF;
			buffer[inicioTx++]=0x0D;
			espConfig.cipMode=data->rxdata[2];
			espConfig.cwMode=data->rxdata[3];
			espConfig.cwDHCP[0]=data->rxdata[4];
			espConfig.cwDHCP[1]=data->rxdata[5];
			myWord.ui8[0]=data->rxdata[6];
			myWord.ui8[1]=data->rxdata[7];
			myWord.ui8[2]=data->rxdata[8];
			myWord.ui8[3]=data->rxdata[9];
			espConfig.remoteport=myWord.ui16[0];
			espConfig.localport=myWord.ui16[1];
			espConfig.conectionType=data->rxdata[10];
			for (uint8_t index=11; index<(data->rxdata[0]);index++){
				switch(tipo){
				case 0:
					if(data->rxdata[index]!=',')
						espConfig.ssid[indexEsp++]=data->rxdata[index];
					else{
						tipo=1;
						indexEsp=0;
					}
				break;
				case 1:
					if(data->rxdata[index]!=',')
						espConfig.pass[indexEsp++]=data->rxdata[index];
					else{
						tipo=2;
						indexEsp=0;
					}
				break;
				case 2:
					if(data->rxdata[index]!=',')
						espConfig.ipremota[indexEsp++]=data->rxdata[index];
					else{
						tipo=3;
					}
				break;
				default:
					;
				}
			}
			espConfig.confState=READYTOCONF;
			buffer[4]=0x03;
			break;
       case 0xFA://Hacer algo
			buffer[inicioTx++]=0xFA;

			buffer[4]=0x02;
			break;
        case 0xF0://ACK
            buffer[inicioTx++]=0xF0;
            buffer[inicioTx++]=0x0D;
            buffer[4]=0x03;
            break;
        case 0xF1://Version de Firmware
        	buffer[inicioTx++]=0xF0;
			buffer[4]=0x02;
			break;
		default:

			;
    }
 		for(int a=0; a<inicioTx; a++) {
		   cheksum^=buffer[a];
		   data->outBuff[data->txWrite++] =buffer[a];
 		}
		data->outBuff[data->txWrite++]=cheksum;
}

//**********************************************************************************************************************************
//******************************************************* FINALIZA RUTINAS DE DECODIFICACIÓN ***************************************
//**********************************************************************************************************************************

/******************* Funciones Publicas *******************************************/

void comSerialRecibe(){
	if(usbCom.rxRead!=usbCom.rxWrite){
		decodeHeader(&usbCom);
	}
	if(usbCom.txRead!=usbCom.txWrite)
		enviar_byte(&usbCom);

}

uint8_t comSerialSend(_sSerial *usbCom){
	estadoUSB=2;
	if(usbCom->txRead!=usbCom->txWrite)
		enviar_byte(usbCom);
	return estadoUSB;
}
