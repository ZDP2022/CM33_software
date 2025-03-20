#include "io_spi.h"

void SPI_GPIO_Init()
{
	//����SCLK
	STAR_GPIO0->OUTENSET |= ( 1 << SPI_SCLK );//����SCLKΪ���
	STAR_GPIO0->ALTFUNCCLR  = ( 1 << SPI_SCLK );//�رո���
	STAR_GPIO0->INTENCLR = ( 1 << SPI_SCLK );//�ر��ж�
	//����CS
	STAR_GPIO0->OUTENSET |= ( 1 << SPI_CS );//����CSΪ���
	STAR_GPIO0->ALTFUNCCLR  = ( 1 << SPI_CS );//�رո���
	STAR_GPIO0->INTENCLR = ( 1 << SPI_CS );//�ر��ж�
	//����MOSI
	STAR_GPIO0->OUTENSET |= ( 1 << SPI_MOSI );//����MOSIΪ���
	STAR_GPIO0->ALTFUNCCLR  = ( 1 << SPI_MOSI );//�رո���
	STAR_GPIO0->INTENCLR = ( 1 << SPI_MOSI );//�ر��ж�
	//����MISO
	STAR_GPIO0->OUTENCLR = (1 << SPI_MISO);//����MISOΪ����
	STAR_GPIO0->ALTFUNCCLR  = ( 1 << SPI_MISO );//�رո���
	STAR_GPIO0->INTENCLR = ( 1 << SPI_MISO );//�ر��ж�
}
//����SPI��GPIO�ڵĵ�ƽ
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
//��ȡSPI IO�ڵĵ�ƽ
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
//SPI��ȡһ���ֽڵ�����
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
//SPI����һ���ֽڵ�����
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