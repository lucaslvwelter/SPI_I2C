// lcd5110_graphics.h
#ifndef INC_LCD5110_GRAPHICS_H_
#define INC_LCD5110_GRAPHICS_H_

#include <stdint.h>
#include "direcao.h"

void LCD5110_update(void);
void LCD5110_clrScr(void);
void LCD5110_fillScr(void);
void LCD5110_setPixel(uint16_t x, uint16_t y);
void LCD5110_clrPixel(uint16_t x, uint16_t y);
void LCD5110_invPixel(uint16_t x, uint16_t y);
void LCD5110_drawHLine(int x, int y, int l);
void LCD5110_clrHLine(int x, int y, int l);
void LCD5110_drawVLine(int x, int y, int l);
void LCD5110_clrVLine(int x, int y, int l);
void LCD5110_drawLine(int x1, int y1, int x2, int y2);
void LCD5110_drawRect(int x1, int y1, int x2, int y2);
void LCD5110_drawCircle(int x, int y, int radius);
void desenharSeta(Direcao dir);
void mostrarResultado(uint8_t acertou);
void mostrarTelaInicial(void);

#endif /* INC_LCD5110_GRAPHICS_H_ */
