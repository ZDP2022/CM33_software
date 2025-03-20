#include "io_spi.h"

void SPI_GPIO_Init()
{
	//设置SCLK
	STAR_GPIO0->OUTENSET |= ( 1 << SPI_SCLK );//设置SCLK为输出
	STAR_GPIO0->ALTFUNCCLR  = ( 1 << SPI_SCLK );//关闭复用
	STAR_GPIO0->INTENCLR = ( 1 << SPI_SCLK );//关闭中断
	//设置CS
	STAR_GPIO0->OUTENSET |= ( 1 << SPI_CS );//设置CS为输出
	STAR_GPIO0->ALTFUNCCLR  = ( 1 << SPI_CS );//关闭复用
	STAR_GPIO0->INTENCLR = ( 1 << SPI_CS );//关闭中断
	//设置MOSI
	STAR_GPIO0->OUTENSET |= ( 1 << SPI_MOSI );//设置MOSI为输出
	STAR_GPIO0->ALTFUNCCLR  = ( 1 << SPI_MOSI );//关闭复用
	STAR_GPIO0->INTENCLR = ( 1 << SPI_MOSI );//关闭中断
	//设置MISO
	STAR_GPIO0->OUTENCLR = (1 << SPI_MISO);//设置MISO为输入
	STAR_GPIO0->ALTFUNCCLR  = ( 1 << SPI_MISO );//关闭复用
	STAR_GPIO0->INTENCLR = ( 1 << SPI_MISO );//关闭中断
}
//设置SPI的GPIO口的电平
void SPI_GPIO_WriteBit(uint16_t GPIO_Pin_x, uint8_t BitVal)
{
	 if (BitVal != Bit_RESET)
	{
			STAR_GPIO0->DATA |= ( Bit_SET << GPIO_Pin_x );
	}
	else
	{
			STAR_GPIO0->DATA &= (~( Bit_SET << GPIO_Pin_x ));
	}
}
//读取SPI IO口的电平
#define SPI_IO_MISO_Mask (1 << SPI_MISO)
uint8_t SPI_GPIO_RdData(uint16_t GPIO_Pin_x)
{
	uint32_t readValue = 0x00000000;
	readValue = STAR_GPIO0->DATA;
	
	if(readValue & SPI_IO_MISO_Mask)
		return 1;
	else
		return 0;
}
//SPI读取一个字节的数据
unsigned char GPIO_SPI_Read_Byte(void)
{
	unsigned char i, rByte = 0;
 //SPI_SCLK=0;
	SPI_GPIO_WriteBit(SPI_SCLK,0);
	for(i = 0;i < 8;i++){
		//SPI_SCLK=1;
		SPI_GPIO_WriteBit(SPI_SCLK,1);
		rByte <<= 1;
		//rByte|=SPI_MISO;
		rByte |= SPI_GPIO_RdData(SPI_MISO);
		//SPI_SCLK=0;
		SPI_GPIO_WriteBit(SPI_SCLK,0);
	}
		return rByte;
}
//SPI发送一个字节的数据
void GPIO_SPI_Write_Byte(uint8_t dt)
{
	uint8_t i;
	for(i = 0;i < 8;i++){
		SPI_GPIO_WriteBit(SPI_SCLK,0);
		if((dt << i) & 0x80)
			SPI_GPIO_WriteBit(SPI_MOSI,1);
		else
			SPI_GPIO_WriteBit(SPI_MOSI,0);
		SPI_GPIO_WriteBit(SPI_SCLK,1);
	}
	SPI_GPIO_WriteBit(SPI_SCLK,0);
}