/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd5110_hal.h"
#include "lcd5110_graphics.h"
#include "mpu6050_func.h"
#include <stdlib.h>
#include <time.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
  DIR_CIMA,
  DIR_BAIXO,
  DIR_ESQUERDA,
  DIR_DIREITA,
  DIR_NEUTRO
} Direcao;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t dir;
float THRESHOLD = 0.5;
leituraAcel leituraA;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
Direcao detectarMovimento(float ax, float ay, float threshold);
void desenharSeta(Direcao dir);
/* USER CODE BEGIN PFP */

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
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  MX_I2C1_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */
  mpu6050Init(); // Inicializa o sensor
  LCD5110_Init();
  LCD5110_clrScr();
  HAL_TIM_Base_Start_IT(&htim10);

  srand(HAL_GetTick());
  Direcao direcaoAtual = DIR_NEUTRO;
  uint32_t tempoSeta = 0;
  uint8_t aguardandoMovimento = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  mpu6050ReadAccel(&leituraA);
	  float ax = leituraA.accelX / 16384.0;
	  float ay = leituraA.accelY / 16384.0;

	  char buffer[100];
	  snprintf(buffer, sizeof(buffer), "AX: %.2f\tAY: %.2f\r\n", ax, ay);
	  HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

	  if (!aguardandoMovimento) {
		  static Direcao direcaoAnterior = DIR_NEUTRO;
		  do {
		  	direcaoAtual = (Direcao)(rand() % 4);  // Gera de 0 a 3
		  } while (direcaoAtual == direcaoAnterior);
		  direcaoAnterior = direcaoAtual;
		  desenharSeta(direcaoAtual);
		  tempoSeta = HAL_GetTick();
		  aguardandoMovimento = 1;
	  } else {
		  if (HAL_GetTick() - tempoSeta > 1000) {  // Espera 500ms para movimento
			  Direcao mov = detectarMovimento(ax, ay, THRESHOLD);

			  if (mov == direcaoAtual) {
				  const char *msg = "Acertou!\r\n";
				  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
			  } else {
				  const char *msg = "Errou!\r\n";
				  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
			  }

			  aguardandoMovimento = 0;
			  HAL_Delay(1000);  // Espera antes da próxima seta
		  }
	  }

	  HAL_Delay(50);  // Suaviza leitura
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void desenharSeta(Direcao dir) {
	LCD5110_clrScr();

    switch (dir) {
        case DIR_CIMA:
        	LCD5110_drawLine(42, 30, 42, 10);
			LCD5110_drawLine(42, 10, 36, 16);
			LCD5110_drawLine(42, 10, 48, 16);
			break;
        case DIR_BAIXO:
			LCD5110_drawLine(42, 10, 42, 30);
			LCD5110_drawLine(42, 30, 36, 24);
			LCD5110_drawLine(42, 30, 48, 24);
			break;
        case DIR_ESQUERDA:
			LCD5110_drawLine(50, 24, 30, 24);
			LCD5110_drawLine(30, 24, 36, 18);
			LCD5110_drawLine(30, 24, 36, 30);
			break;
        case DIR_DIREITA:
			LCD5110_drawLine(30, 24, 50, 24);
			LCD5110_drawLine(50, 24, 44, 18);
			LCD5110_drawLine(50, 24, 44, 30);
			break;
        default:
            break;
    }
}

Direcao detectarMovimento(float ax, float ay, float threshold)
{
	if (ay < -threshold) return DIR_ESQUERDA;
	if (ay > threshold) return DIR_DIREITA;
	if (ax > threshold) return DIR_BAIXO;
	if (ax < -threshold) return DIR_CIMA;
	return DIR_NEUTRO;
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



