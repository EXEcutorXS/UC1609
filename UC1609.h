#include "main.h"

#ifndef UC1609
#define UC1609


// Display  Size
#define UC1609_WIDTH 192
#define UC1609_HEIGHT 64

#define UC1609_BUFFER_SIZE (UC1609_WIDTH*UC1609_HEIGHT/8)
#define UC1609_CMD_INVERION

#define UC1609_CMD_SCROLL 0b01000000
#define UC1609_CMD_INVERSION 0b10100110
#define UC1609_CMD_ENABLE  0b10101111
#define UC1609_CMD_MAPPING 0b11000000

	#define MIRROR_X 0b100
	#define MIRROR_Y 0b010

#define UC1609_CMD_SET_BIAS_RATIO  0b11101000
	#define DEFAULT_BIAS_RATIO 0b11
#define UC_1609_CMD_SET_BIAS_POTENTIOMETER 0b10000001
#define IMHO_BEST_LOOKING_BIAS_RATIO 0x83



void UC1609_Init (SPI_HandleTypeDef* spiHandler,
									GPIO_TypeDef* csGpioPort,
									uint16_t csGpioPin,
									GPIO_TypeDef* cdGpioPort,
									uint16_t cdGpioPin,
									GPIO_TypeDef* rstGpioPort,
									uint16_t rstGpioPin);
void UC1609_SetPos (uint8_t x, uint8_t y);
void UC1609_UpdateScreen ();
void UC1609_Clean ();
void UC1609_PutString (char *c);
void UC1609_SetXY (uint8_t x, uint8_t y);
void UC1609_HardwareReset();
void UC1609_SoftwareReset();
void UC1609_Scroll (uint8_t lines);
void UC1609_SetInversion (uint8_t invert);
void UC1609_DrawBitmap(unsigned char* buf);
void UC1609_PutPixel(uint8_t x, uint8_t y);
void UC1609_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void UC1609_DrawDottedLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void UC1609_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void UC1609_DrawFilledRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
#endif
