#include "main.h"

extern const unsigned char myFont[165][5];
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi1;


uint8_t buffer[UC1609_BUFFER_SIZE];
uint16_t cursor;

SPI_HandleTypeDef* hspi;
GPIO_TypeDef * csPort;
uint16_t csPin;
GPIO_TypeDef * cdPort;
uint16_t cdPin;
GPIO_TypeDef * rstPort;
uint16_t rstPin;
uint8_t biasRatio=0x88;

void sendCommand (uint8_t com)
{
	HAL_GPIO_WritePin (csPort, csPin, 0);
	HAL_GPIO_WritePin (cdPort, cdPin, 0);
	HAL_SPI_Transmit (hspi, &com, 1, 10);
	HAL_GPIO_WritePin (csPort, csPin, 1);
}

void UC1609_SoftwareReset()
{
sendCommand(0b11100010);
HAL_Delay (20);
}

void UC1609_HardwareReset()
{
	HAL_GPIO_WritePin (rstPort, rstPin, 0);
	HAL_Delay (1);
	HAL_GPIO_WritePin (rstPort, rstPin, 1);
	HAL_Delay (5);
}
void UC1609_Init (SPI_HandleTypeDef* spiHandler,
									GPIO_TypeDef* csGpioPort,
									uint16_t csGpioPin,
									GPIO_TypeDef* cdGpioPort,
									uint16_t cdGpioPin,
									GPIO_TypeDef* rstGpioPort,
									uint16_t rstGpioPin)
{
	hspi = spiHandler;
	csPort=csGpioPort;
	csPin=csGpioPin;
	cdPort=cdGpioPort;
	cdPin=cdGpioPin;
	rstPort=rstGpioPort;
	rstPin=rstGpioPin;
if (rstPin==0 || rstPort==0)
	{
	UC1609_SoftwareReset();
	}
else
	{
	UC1609_HardwareReset();
	}
	sendCommand (UC1609_CMD_MAPPING | MIRROR_X);
	sendCommand (UC1609_CMD_SET_BIAS_RATIO|DEFAULT_BIAS_RATIO);
	sendCommand (UC_1609_CMD_SET_BIAS_POTENTIOMETER);
	sendCommand (IMHO_BEST_LOOKING_BIAS_RATIO);
	UC1609_Scroll(0);
}

void UC1609_SetXY (uint8_t x, uint8_t y)
{
	sendCommand (0b00000000 | (x & 0xF));
	sendCommand (0b00010000 | ((x >> 4) & 0xF));
	sendCommand (0b10110000 | y);
}

void UC1609_SetPos (uint8_t x, uint8_t y)
{
	cursor = x * 6 + UC1609_WIDTH * y;
}


void UC1609_UpdateScreen ()
{

	UC1609_SetXY (0, 0);
	HAL_GPIO_WritePin (csPort, csPin, 0);
	HAL_GPIO_WritePin (cdPort, cdPin, 1);
	HAL_SPI_Transmit (hspi, buffer, UC1609_BUFFER_SIZE, 1000);
	HAL_GPIO_WritePin (csPort, csPin, 1);
	sendCommand (UC1609_CMD_ENABLE);
}

void UC1609_Clean ()
{
	for (uint16_t i = 0; i < UC1609_BUFFER_SIZE; ++i)
		buffer[i] = 0;
}

void UC1609_PutRuC (char *c)
{

	uint8_t pos;
	if (*c == 0xD0)
		pos = *(c + 1) - 144 + 101;
	else if (*c == 0xD1)
		pos = *(c + 1) - 128 + 149;
	else
		return;
	memcpy (buffer + cursor, &myFont[pos][0], 5);
	cursor += 5;
}

void UC1609_Put (char c)
{
	memcpy (buffer + cursor, &myFont[c - 0x20][0], 5);
	cursor += 5;
}

void UC1609_PutString (char *c)
{
	while (*c)
		{
			cursor %= UC1609_BUFFER_SIZE;
			if (*c == 0xD0 || *c == 0xD1)
				{
					UC1609_PutRuC (c);
					c += 2;
				}
			else
				UC1609_Put (*c++);
			cursor++;
		}
}

void UC1609_SetInversion (uint8_t invert)
{
sendCommand(UC1609_CMD_INVERSION|invert);
}

void UC1609_Scroll (uint8_t lines)
{
lines%=64;
sendCommand(UC1609_CMD_SCROLL|lines);
}

void UC1609_DrawBitmap(uint8_t *buf) {
	memcpy(buffer, buf, sizeof(buffer));
}

void UC1609_PutPixel(uint8_t x, uint8_t y) {
	buffer[(x + (y/8)*UC1609_WIDTH)%UC1609_BUFFER_SIZE] |= 1 << (y % 8);
}

void UC1609_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	if (x2==x1 && y2==y1)
		{
		UC1609_PutPixel(x1, y1);
		return;
		}
	if(x2<x1)
		{
			uint8_t temp = x2;
			x2=x1;
			x1=temp;
		}
	if(y2<y1)
			{
				uint8_t temp = y2;
				y2=y1;
				y1=temp;
			}
	if (x2 - x1 > y2 - y1) {
		for (uint8_t i = x1; i < x2 + 1; ++i) {
			UC1609_PutPixel(i, y1 + (y2 - y1) * (i - x1) / (x2 - x1));
		}
	} else {
		for (uint8_t i = y1; i < y2 + 1; ++i) {
			UC1609_PutPixel(x1 + (x2 - x1) * (i - y1) / (y2 - y1), i);
		}
	}
}

void UC1609_DrawDottedLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	if (x2==x1 && y2==y1)
		{
		UC1609_PutPixel(x1, y1);
		return;
		}
	if (x2 - x1 > y2 - y1) {
			for (uint8_t i = x1; i < x2 + 1; i+=2) {
				UC1609_PutPixel(i, y1 + (y2 - y1) * (i - x1) / (x2 - x1));
			}
		} else {
			for (uint8_t i = y1; i < y2 + 1; i+=2) {
				UC1609_PutPixel(x1 + (x2 - x1) * (i - y1) / (y2 - y1), i);
			}
		}
}

void UC1609_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	UC1609_DrawLine(x1, y1, x1, y2);
	UC1609_DrawLine(x2, y1, x2, y2);
	UC1609_DrawLine(x1, y1, x2, y1);
	UC1609_DrawLine(x1, y2, x2, y2);
}

void UC1609_DrawFilledRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	for (uint8_t i = x1; i < x2+1; ++i)
		UC1609_DrawLine(i, y1, i, y2);
}
