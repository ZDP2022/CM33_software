#ifndef _IO_SPI_H
#define _IO_SPI_H

#include "STAR.h"
#include "STAR_gpio.h"
#include "STAR_conf.h"

#define SPI_SCLK GPIO_Pin_11 //SP1 GPIO_Pin_11
#define SPI_CS   GPIO_Pin_12 // GPIO_Pin_12
#define SPI_MOSI GPIO_Pin_13 // GPIO_Pin_13
#define SPI_MISO GPIO_Pin_14//GPIO_Pin_14

void SPI_GPIO_Init();
void SPI_GPIO_WriteBit(uint16_t GPIO_Pin_x, uint8_t BitVal);
uint8_t SPI_GPIO_RdData(uint16_t GPIO_Pin_x);
void GPIO_SPI_Write_Byte(uint8_t dt);
unsigned char GPIO_SPI_Read_Byte(void);

#endif