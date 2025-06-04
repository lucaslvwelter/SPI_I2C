#ifndef INC_LCD5110_HAL_H_
#define INC_LCD5110_HAL_H_

#include "stm32f4xx_hal.h"

// SPI handle
extern SPI_HandleTypeDef hspi2;

// Define os pinos do display
#define LCD_DC_GPIO_Port GPIOA
#define LCD_DC_Pin       GPIO_PIN_15

#define LCD_RST_GPIO_Port GPIOA
#define LCD_RST_Pin       GPIO_PIN_12

#define LCD_CE_GPIO_Port GPIOC
#define LCD_CE_Pin       GPIO_PIN_11

// Funções principais
void LCD5110_Init(void);
void LCD5110_SendCommand(uint8_t cmd);
void LCD5110_SendData(uint8_t data);
void LCD5110_Clear(void);
void LCD5110_SetXY(uint8_t x, uint8_t y);
void LCD5110_WriteChar(char c);
void LCD5110_WriteString(char *s);
void LCD5110_WriteStringCentered(char *str, uint8_t y);

#endif /* INC_LCD5110_HAL_H_ */
