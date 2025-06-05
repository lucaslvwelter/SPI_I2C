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
#include "funcGerais.h"
#include "direcao.h"
#include <stdlib.h>
#include <time.h>
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
uint8_t dir; // armazena a direção da seta
float THRESHOLD = 0.5; // sensibilidade do movimento
leituraAcel leituraA; // variavel que armazena a aceleração
int acertosSequencia = 0; // armazena a sequencia de acertos
int start = 0; // botao de iniciar
int stop = 1; // botao de parar
Direcao direcaoAtual = DIR_NEUTRO; // estado inicial da placa
Direcao direcaoAnterior = DIR_NEUTRO; // estado inicial da placa
uint32_t tempoSeta = 0; // tempo que aparece a seta (calculo com HAL_GetTick)
uint8_t aguardandoMovimento = 0; //
int mestre_mandou = 0; // estado do LED e ordem do "mestre mandou"
uint8_t tela_inicial_mostrada = 0; // exibe a tela inicial do jogo
/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
	LCD5110_Init(); // Inicializa o display
	LCD5110_clrScr(); // Limpa o display
	srand(HAL_GetTick()); // Aleatoriza a seed
	mostrarTelaInicial(); // Exibe a tela inicial do jogo
	/* USER CODE END 2 */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		lerBotoes(); // Lê o estado dos botoões de start e stop
		if (stop && !tela_inicial_mostrada) // Exibe a tela inicial se stop for clicado
		{
			mostrarTelaInicial();
			tela_inicial_mostrada = 1;
		}
		if (start && !stop) // Se start for clicado, começa o jogo
		{
			if (!aguardandoMovimento) // exibe as setas para após poder observar o movimento
			{
				mestre_mandou = rand() % 2; // Lógica do Mestre Mandou
				if (mestre_mandou == 0) // caso o número aleatório seja 0
				{
					// NÃO mandei
					HAL_GPIO_WritePin(MESTRE_MANDOU_GPIO_Port, MESTRE_MANDOU_Pin, GPIO_PIN_RESET); // LED FICA APAGADO
					// Gera nova direção
					do
					{
						direcaoAtual = (Direcao)(rand() % 4); // aleatoriza a seta para ser diferente da anterior
					} while (direcaoAtual == direcaoAnterior); // caso seja a mesma seta da rodada anterior, faz ela alterar
					direcaoAnterior = direcaoAtual;
					desenharSeta(direcaoAtual); // Exibe seta
					uint32_t tempoInicial = HAL_GetTick();
					while (HAL_GetTick() - tempoInicial < 1000) // tempo entre setas
					{
						mpu6050ReadAccel(&leituraA); // faz a leitura do sensor para observar se o usuário se moveu
						float ax = leituraA.accelX / 16384.0;
						float ay = leituraA.accelY / 16384.0;
						Direcao mov = detectarMovimento(ax, ay, THRESHOLD); // verifica se o usuário fez um movimento brusco
						if (mov != DIR_NEUTRO)
						{
							// Movimento foi detectado com LED apagado => erro!
							mostrarResultado(0);
							acertosSequencia = 0;
							enviarAcertosSequencia();
							start = 0;
							stop = 1;
							tela_inicial_mostrada = 0;
							break; // sai do while
						}
						HAL_Delay(50); // pequena pausa entre leituras
					}
					HAL_Delay(500); // pequena espera antes da próxima rodada (opcional)
				}
				else // CASO MESTRE MANDOU = 1
				{
					HAL_GPIO_WritePin(MESTRE_MANDOU_GPIO_Port, MESTRE_MANDOU_Pin, GPIO_PIN_SET); // LED FICA ACESO
					// Gera nova direção
					do
					{
						direcaoAtual = (Direcao)(rand() % 4); // aleatoriza uma seta
					} while (direcaoAtual == direcaoAnterior); // faz a seta ser diferente da anterior
					direcaoAnterior = direcaoAtual;
					desenharSeta(direcaoAtual); // desenha a seta
					tempoSeta = HAL_GetTick();
					aguardandoMovimento = 1; // espera o movimento do usuário
				}
			}
			else // se o usuário não se mecher, o usuário perde
			{
				// Aguarda movimento por no máximo 2 segundos
				if (HAL_GetTick() - tempoSeta > 2000)
				{
					// Timeout
					mostrarResultado(0);
					acertosSequencia = 0;
					start = 0;
					stop = 1;
					tela_inicial_mostrada = 0;
					aguardandoMovimento = 0;
				}
				else if (HAL_GetTick() - tempoSeta > 1000) // só começa a verificar após 1s
				{
					mpu6050ReadAccel(&leituraA); // faz a leitura do movimento
					float ax = leituraA.accelX / 16384.0;
					float ay = leituraA.accelY / 16384.0;
					Direcao mov = detectarMovimento(ax, ay, THRESHOLD); // verifica se o movimento é válido
					if (mov != DIR_NEUTRO)
					{
						if (mov == direcaoAtual) // se movimento igual à seta
						{
							mostrarResultado(1); // exibe acertou no display
							acertosSequencia++; // aumenta a quantidade de acertos
						}
						else // se o usuário errou
						{
							mostrarResultado(0); // exibe errou no display
							acertosSequencia = 0; // zera os acertos em sequência
							start = 0;
							stop = 1; // volta a tela inicial do jogo
							tela_inicial_mostrada = 0;
						}
						enviarAcertosSequencia(); // envia a quantidade de acertos
						aguardandoMovimento = 0;
						HAL_Delay(500); // Pausa após resposta
					}
					else // se movimento for igual a neutro, ele zera os acertos
					{
						acertosSequencia = 0; // zera os acertos em sequência
						start = 0;
						stop = 1; // volta a tela inicial do jogo
						tela_inicial_mostrada = 0;
						enviarAcertosSequencia(); // envia a quantidade de acertos
						aguardandoMovimento = 0;
						HAL_Delay(500); // Pausa após resposta
					}
				}
			}
			HAL_Delay(50); // Suavização
		}
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


