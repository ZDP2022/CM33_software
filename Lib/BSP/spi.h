#ifndef __SPI_H
#define __SPI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STAR.h"
	
#define FLASH_ID 0xEF3017

void SSP_PinRemap(SSP_TypeDef* spix);
void SPI_Read_Flash_ID(SSP_TypeDef* spix);
void SSP_Loop(SSP_TypeDef* spix);
void SSP_Loop_1(SSP_TypeDef* spix);
void SPI_test(void);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_H */

