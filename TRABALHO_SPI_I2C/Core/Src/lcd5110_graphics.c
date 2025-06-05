// lcd5110_graphics.c
#include "lcd5110_hal.h"
#include "lcd5110_graphics.h"
#include "direcao.h"
#include "main.h"

static uint8_t scrbuf[504];

void LCD5110_update(void) {
    LCD5110_SetXY(0, 0);
    for (int i = 0; i < 504; i++) {
        LCD5110_SendData(scrbuf[i]);
    }
}

void LCD5110_clrScr(void) { // limpa o display
    for (int i = 0; i < 504; i++) {
        scrbuf[i] = 0x00;
    }
    LCD5110_update();
}

void LCD5110_fillScr(void) {
    for (int i = 0; i < 504; i++) {
        scrbuf[i] = 0xFF;
    }
    LCD5110_update();
}

void LCD5110_setPixel(uint16_t x, uint16_t y) {
    if (x >= 84 || y >= 48) return;
    scrbuf[(y/8)*84 + x] |= (1 << (y % 8));
}

void LCD5110_clrPixel(uint16_t x, uint16_t y) {
    if (x >= 84 || y >= 48) return;
    scrbuf[(y/8)*84 + x] &= ~(1 << (y % 8));
}

void LCD5110_invPixel(uint16_t x, uint16_t y) {
    if (x >= 84 || y >= 48) return;
    scrbuf[(y/8)*84 + x] ^= (1 << (y % 8));
}

void LCD5110_drawHLine(int x, int y, int l) {
    for (int i = 0; i < l && (x+i)<84; i++) {
        LCD5110_setPixel(x+i, y);
    }
    LCD5110_update();
}

void LCD5110_clrHLine(int x, int y, int l) {
    for (int i = 0; i < l && (x+i)<84; i++) {
        LCD5110_clrPixel(x+i, y);
    }
    LCD5110_update();
}

void LCD5110_drawVLine(int x, int y, int l) {
    for (int i = 0; i < l && (y+i)<48; i++) {
        LCD5110_setPixel(x, y+i);
    }
    LCD5110_update();
}

void LCD5110_clrVLine(int x, int y, int l) {
    for (int i = 0; i < l && (y+i)<48; i++) {
        LCD5110_clrPixel(x, y+i);
    }
    LCD5110_update();
}

void LCD5110_drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        LCD5110_setPixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
    LCD5110_update();
}

void LCD5110_drawRect(int x1, int y1, int x2, int y2) {
    LCD5110_drawHLine(x1, y1, x2 - x1);
    LCD5110_drawHLine(x1, y2, x2 - x1);
    LCD5110_drawVLine(x1, y1, y2 - y1);
    LCD5110_drawVLine(x2, y1, y2 - y1);
}

void LCD5110_drawCircle(int x0, int y0, int radius) {
    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    LCD5110_setPixel(x0, y0 + radius);
    LCD5110_setPixel(x0, y0 - radius);
    LCD5110_setPixel(x0 + radius, y0);
    LCD5110_setPixel(x0 - radius, y0);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        LCD5110_setPixel(x0 + x, y0 + y);
        LCD5110_setPixel(x0 - x, y0 + y);
        LCD5110_setPixel(x0 + x, y0 - y);
        LCD5110_setPixel(x0 - x, y0 - y);
        LCD5110_setPixel(x0 + y, y0 + x);
        LCD5110_setPixel(x0 - y, y0 + x);
        LCD5110_setPixel(x0 + y, y0 - x);
        LCD5110_setPixel(x0 - y, y0 - x);
    }
    LCD5110_update();
}

void desenharSeta(Direcao dir) { // desenha a respectiva seta
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

void mostrarResultado(uint8_t acertou) { // exibe os resultados, se acertou ou errou
    LCD5110_Clear();
    LCD5110_SetXY(0, 0);

    if (acertou == 1) { // se acertou
        LCD5110_WriteStringCentered("Acertou!", 2); // exibe ACERTOU centralizado
    } else { // se errou
        LCD5110_WriteStringCentered("Errou!", 2); // exibe ERROU centralizado
    }

    HAL_Delay(1000);  // Dá tempo para ver o resultado
}

void mostrarTelaInicial(void) // mostra a tela inicial do jogo
{
	LCD5110_Clear();
	LCD5110_SetXY(0, 0);
	LCD5110_WriteStringCentered("MESTRE MANDOU", 2); // exibe MESTRE MANDOU centralizado
}
