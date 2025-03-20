#include "STAR_rcc.h"
#include "STAR_gpio.h"
#include "STAR.h"
#include "STAR_i2c.h"
#include "BL24C08.h"
#include <stdio.h>

//STAR I2C OWN_ADDR
#define I2C_OWN_ADDR 	0x000B

//EEPROM I2C设备地址
#define EEPROM_DEV_ADDR 0xA0


//最多往EEPROM连续写入n个字节数据
//slave_addr:需要写入数据的EEPROM 的设备地址
//addr:开始写入数据的存储地址
//Num:需要写入数据的个数
//Buffer:要写入的数据buffer
void I2C_ByteWrite_nBytes (uint8_t slave_addr,uint8_t word_addr,uint8_t Num,uint8_t* Buffer)
{	
	uint16_t addr_wtite = 0x0000;
	
	I2C_DeInit(STAR_I2C);
	//将对应的GPIO复用为I2C的管脚
	GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_I2C0SCL, ENABLE); //GPIO[15]
	GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_I2C0SDA, ENABLE); //GPIO[16]
	//I2C配置: I2C主模式，标准I2C速率	
	I2C_Init(STAR_I2C, I2C_Mode_Master, I2C_Speed_Standard, I2C_OWN_ADDR);
	//使能发送
	I2C_TxEnable( STAR_I2C, ENABLE);
	//设置设备地址
	addr_wtite = 0x0000 | slave_addr;
	I2C_SlvaddrConfig( STAR_I2C, addr_wtite);
	//开始发送设备地址和数据
	I2C_GenerateSTART( STAR_I2C,ENABLE);
	
	I2C_TxRxDataNumConfig(STAR_I2C,2);
	
	I2C_SendData(STAR_I2C, word_addr>>8); 	//24C64 wordAddr 是16bit 分两次发送
	while(I2C_CheckACKIsFail(STAR_I2C));	//等待ACK
	I2C_SendData(STAR_I2C, word_addr&0xff);
	while(I2C_CheckACKIsFail(STAR_I2C));	//等待ACK
	
	while(Num)
	{
		I2C_TxRxDataNumConfig(STAR_I2C,1);
		I2C_WaitTxFIFOIsNotFull(STAR_I2C);//等待发送FIFO有空位
		I2C_SendData(STAR_I2C, *Buffer);
		while(I2C_CheckACKIsFail(STAR_I2C));//等待ACK
		Buffer ++;
		Num--;
	}
	I2C_WaitSendDataFinish(STAR_I2C);	
	I2C_GenerateSTOP(STAR_I2C,ENABLE );
}

//EEPROM任意地址读取
//slave_addr:需要读取数据的EEPROM的设备地址
//word_addr:需要开始读取数据的存储地址
//rx_data_num:读取数据的字节数
//Buffer:存储读取到数据的buffer
void I2C_EEPROM_RandomRead(uint8_t slave_addr,uint8_t word_addr,uint16_t rx_data_num,uint8_t* Buffer)
{		
	uint16_t addr_read  = 0x8000;	//读写操作由SLV_ADDR的bit[15]确定，该位为0表示写操作，为1表示读操作
	uint16_t addr_wtite = 0x0000;
	
	I2C_DeInit(STAR_I2C);
	//将对应的GPIO复用为I2C的管脚
	GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_I2C0SCL, ENABLE); //GPIO[15]
	GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_I2C0SDA, ENABLE); //GPIO[16]
	//I2C配置: I2C主模式，标准I2C速率
	I2C_Init(STAR_I2C, I2C_Mode_Master, I2C_Speed_Standard, I2C_OWN_ADDR);//0x001a
	//使能发送
	I2C_TxEnable( STAR_I2C, ENABLE); //TX ENABLE
	//设置设备地址
	addr_wtite |= slave_addr;
	
	I2C_SlvaddrConfig(STAR_I2C, addr_wtite);//
	I2C_GenerateSTART(STAR_I2C,ENABLE);
	
	I2C_TxRxDataNumConfig(STAR_I2C,2);
	I2C_SendData(STAR_I2C, word_addr>>8); //24C64 wordAddr 是16bit 分两次发送
	while(I2C_CheckACKIsFail(STAR_I2C));//while(I2C_Get_ACK_Fail_Flag());						//等待ACK
	I2C_SendData(STAR_I2C, word_addr&0xff);//08  address
	while(I2C_CheckACKIsFail(STAR_I2C));//while(I2C_Get_ACK_Fail_Flag());						//等待ACK
	I2C_WaitSendDataFinish(STAR_I2C);
	
	
	I2C_GenerateSTOP( STAR_I2C,ENABLE );
	I2C_TxEnable(STAR_I2C,DISABLE);//STAR_I2C->CONTROL |= (0UL << CM3DS_MPS2_I2C_TX_ENA_Pos);
	
	I2C_Init(STAR_I2C, I2C_Mode_Master, I2C_Speed_Standard, I2C_OWN_ADDR);//0x001a
	addr_read |= slave_addr;
	I2C_SlvaddrConfig( STAR_I2C, addr_read);//read 
	//使能接收
	I2C_RxEnable( STAR_I2C,ENABLE );
	//开始接收读取到的数据
	I2C_GenerateSTART( STAR_I2C,ENABLE);
	while(rx_data_num)
	{
		I2C_WaitReceiveReady(STAR_I2C);
		*Buffer =  I2C_ReceiveData(STAR_I2C);
		Buffer++;
		rx_data_num--; 
	}
	I2C_GenerateSTOP( STAR_I2C,ENABLE );
}

uint8_t Tx_Buffer[256];				  
uint8_t Rx_Buffer[256];


void EEPROM_ReadWrite_test(void)
{
	int i=0;
	uint16_t rw_num=32;
	uint32_t s1,s2;
	uint16_t rw_addr=0;
	
	//向EEPROM的rw_addr地址开始写入rw_num字节数据
	printf("I2C write data to EEPROM...\n");
	memset(Tx_Buffer,0x0,sizeof(Tx_Buffer));
	for(i=0;i<rw_num;i++)
	{
		Tx_Buffer[i] =i;
	}
	I2C_ByteWrite_nBytes(EEPROM_DEV_ADDR,rw_addr,rw_num,Tx_Buffer);
	printf("write data:");
	for(i=0;i<rw_num;i++)
	{	
		printf("0x%x ", Tx_Buffer[i]);		
	}
	printf("\n");
	
	//从EEPROM的0地址连续读取rw_num字节数据，验证之前写入的数据
	printf("I2C read data from EEPROM...\n");
	memset(Rx_Buffer,0x0,sizeof(Rx_Buffer));
	I2C_EEPROM_RandomRead(EEPROM_DEV_ADDR,rw_addr,rw_num,Rx_Buffer);
	
	printf("read data:");
	for(i=0;i<rw_num;i++)
	{		
		printf("0x%x ", Rx_Buffer[i]);		
	}
	printf("\n");
	
}
