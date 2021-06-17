/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "myDelay.h"
#include "wifi.h"
#include "comSerial.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum{
	STANDBY,
	CONFIGURARWIFI,
	READY
}eMefPpal;

static eMefPpal estadoPrograma;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart4;

delay_t alive, conectime;
/* USER CODE BEGIN PV */

//uint8_t rxData[2], buffer[256], bufferTx[256], indiceRx, indiceTx;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART4_UART_Init(void);

/* USER CODE BEGIN PFP */

static void inicializaMef(void);

static void actualizaMef(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t rxData[1];
	delay_t hearbeat, timeToMef;
	//uint8_t numDatos;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART4_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  __HAL_UART_ENABLE_IT(&huart4,UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(&huart4,UART_IT_TC);
  HAL_UART_Receive_IT(&huart4, rxData, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  delayConfig(&hearbeat, 1000);
  delayConfig(&timeToMef, 500);
  delayConfig(&alive, 1000);
  delayConfig(&conectime, 1000);
  inicializaMef();

  while (1)
  {
    //HeartBeat para saber que el micro funciona
	 if(delayRead(&hearbeat))
		 HAL_GPIO_TogglePin(LEDAZUL_GPIO_Port, LEDAZUL_Pin);
    //Reviso la MEF del programa principal cada cierto tiempo
	 if(delayRead(&timeToMef))
		 actualizaMef();


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART4_UART_Init(void)
{

  /* USER CODE BEGIN USART4_Init 0 */

  /* USER CODE END USART4_Init 0 */

  /* USER CODE BEGIN USART4_Init 1 */

  /* USER CODE END USART4_Init 1 */
  huart4.Instance = USART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART4_Init 2 */

  /* USER CODE END USART4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LEDROJO_Pin|LEDAZUL_Pin|LEDNARANJA_Pin|LEDVERDE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USERBUTTON_Pin */
  GPIO_InitStruct.Pin = USERBUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USERBUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LEDROJO_Pin LEDAZUL_Pin LEDNARANJA_Pin LEDVERDE_Pin */
  GPIO_InitStruct.Pin = LEDROJO_Pin|LEDAZUL_Pin|LEDNARANJA_Pin|LEDVERDE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

static void inicializaMef(void){
	estadoPrograma=STANDBY;
}


static void actualizaMef(void){
  uint8_t mensaje[100], nbytes;
  static cuenta=0;
	switch (estadoPrograma){
		case STANDBY:
    	//REVISAR COMUNICACIONES y pasar a CONFIGURAWIFI si se recibio info desde la PC
			comSerialRecibe();
			if (espConfig.confState)
				  estadoPrograma=CONFIGURARWIFI;
		
		break;
		case CONFIGURARWIFI:
			// configurar y conectar el WIFI
			if(wifiInit(&espConfig, PASTHRUEMODE))
				estadoPrograma=READY;
			wifiRecibeConfig();
			comSerialRecibe();
      cuenta=0;
		break;
		case READY:
			//LEER COMUNICACIONES
      cuenta++;
			wifiRecibe();
			comSerialRecibe();
      //Solo implementado para Testing del funcionamiento
			if(delayRead(&alive)){
        if(espConfig.conectionType==UDP){
          sprintf(mensaje, "stm32 ALIVE UDP N°=%d", cuenta);
          nbytes = strlen(mensaje);
          wifiSend(mensaje, nbytes);
        }				 
        else{
          sprintf(mensaje, "\\192.168.1.200\\mensaje.php?valor=\"stm32 ALIVE TCP N=%d\"", cuenta);
          nbytes = strlen(mensaje);
          wifiSend(mensaje, nbytes);
        }
      }
			//EJECUTAR RUTINAS
			/**
			 * 	Leer ADC
			 * 	Convertir Datos ADC en Valores de Ingeniería
			 * 	Enviar valores al Servidor
			 */
		break;
		default:
			estadoPrograma=STANDBY;
	}


}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
