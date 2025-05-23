#include "lcd5110_hal.h"
#include "font6_8.h" // fonte 6x8

static void LCD5110_Delay(uint32_t ms) {
    HAL_Delay(ms);
}

static void LCD5110_DC(uint8_t val) {
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void LCD5110_RST(uint8_t val) {
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void LCD5110_CE(uint8_t val) {
    HAL_GPIO_WritePin(LCD_CE_GPIO_Port, LCD_CE_Pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void LCD5110_Send(uint8_t data) {
    HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
}

void LCD5110_SendCommand(uint8_t cmd) {
    LCD5110_DC(0);
    LCD5110_CE(0);
    LCD5110_Send(cmd);
    LCD5110_CE(1);
}

void LCD5110_SendData(uint8_t data) {
    LCD5110_DC(1);
    LCD5110_CE(0);
    LCD5110_Send(data);
    LCD5110_CE(1);
}

void LCD5110_Init(void) {
    LCD5110_RST(0);
    LCD5110_Delay(10);
    LCD5110_RST(1);

    LCD5110_SendCommand(0x21); // Extended commands
    LCD5110_SendCommand(0xC6); // Vop (contrast)
    LCD5110_SendCommand(0x06); // Temp coef
    LCD5110_SendCommand(0x13); // Bias
    LCD5110_SendCommand(0x20); // Basic commands
    LCD5110_SendCommand(0x0C); // Normal display

    LCD5110_Clear();
}

void LCD5110_Clear(void) {
    for (int i = 0; i < 504; i++)
        LCD5110_SendData(0x00);
    LCD5110_SetXY(0, 0);
}

void LCD5110_SetXY(uint8_t x, uint8_t y) {
    LCD5110_SendCommand(0x80 | (x * 6));
    LCD5110_SendCommand(0x40 | y);
}

void LCD5110_WriteChar(char c) {
    if (c < 32 || c > 127) c = '?';
    for (int i = 0; i < 6; i++)
        LCD5110_SendData(font6_8[c - 32][i]);
}

void LCD5110_WriteString(char *s) {
    while (*s) {
        LCD5110_WriteChar(*s++);
    }
}

