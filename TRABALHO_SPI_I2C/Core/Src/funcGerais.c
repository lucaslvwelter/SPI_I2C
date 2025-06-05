/*
 * funcGerais.c
 *
 *  Created on: Jun 4, 2025
 *      Author: Usuário
 */

#include "variaveis.h"
#include "main.h"
#include "usart.h"

void lerBotoes(void) // função que lê os pinos do botão
{
	if (HAL_GPIO_ReadPin(BOTAO_START_GPIO_Port, BOTAO_START_Pin) == GPIO_PIN_SET) // verifica se o botão de START foi pressionado
	{
	 start = 1;
	 stop = 0;
	 tela_inicial_mostrada = 0;
	 HAL_Delay(200); // debounce
	}
	if (HAL_GPIO_ReadPin(BOTAO_STOP_GPIO_Port, BOTAO_STOP_Pin) == GPIO_PIN_SET) // verifica se o botão de STOP foi pressionado
	{
	 stop = 1;
	 start = 0;
	 acertosSequencia = 0;
	 aguardandoMovimento = 0;
	 tela_inicial_mostrada = 0;
	 HAL_Delay(200); // debounce
	}
}

void enviarAcertosSequencia(void)
{
	char buffer[64]; // armazenador da mensagem
	int len = snprintf(buffer, sizeof(buffer), "ACERTOS:%d\r\n", acertosSequencia); // salva para poder enviar para o monitor
	HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY); // envia os acertos para o monitor CUBE
}
