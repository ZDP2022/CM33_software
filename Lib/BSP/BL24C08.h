#ifndef __BL24C08_H
#define __BL24C08_H	 
#include "STAR.h"
								  
void I2C_PageWrite_16Bytes (uint8_t slave_addr,uint8_t addr,uint8_t* Buffer);
void I2C_ByteWrite_nBytes (uint8_t slave_addr,uint8_t word_addr,uint8_t Num,uint8_t* Buffer);
void I2C_EEPROM_RandomRead(uint8_t slave_addr,uint8_t word_addr,uint16_t rx_data_num,uint8_t* Buffer);

void EEPROM_ReadWrite_test(void);

#endif
