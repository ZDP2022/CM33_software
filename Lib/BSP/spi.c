#include "spi.h"
#include "STAR_gpio.h"
#include "STAR_spi.h"
#include "STAR_rcc.h"
#include "uart.h"
#include "STAR_uart.h"	

#include <stdio.h> 
#include <string.h> 


//SPI�ܽŸ�������
void SSP_PinRemap(SSP_TypeDef* spix)
{
	if(spix ==STAR_SSP0)
	{
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI0_CLK_OUT, ENABLE); //GPIO[7]  CLK_OUT   P9
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI0_SEL, 	  ENABLE); //GPIO[8]	CS_n      P10
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI0_DATA0,   ENABLE); //GPIO[9]	MOSI	    R11
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI0_DATA1,   ENABLE); //GPIO[10]	MISO      M12
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI0_DATA2,   ENABLE); //GPIO[17] WPn       K11
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI0_DATA3,   ENABLE); //GPIO[18]	HOLDn     R12
	}
	else	//SPI1
	{
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI1_CLK_OUT, ENABLE); //GPIO[11]  CLK_OUT   P9
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI1_SEL, 	  ENABLE); //GPIO[12]	CS_n      P10
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI1_DATA0,   ENABLE); //GPIO[13]	MOSI	    R11
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI1_DATA1,   ENABLE); //GPIO[14]	MISO      M12
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI1_DATA2,   ENABLE); //GPIO[19] WPn       K11
		GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_SPI1_DATA3,   ENABLE); //GPIO[20]	HOLDn     R12
	}
	
}

//ͨ��SPI��ȡFlash��MID��ID
void SPI_Read_Flash_ID(SSP_TypeDef* spix)
{
	SSP_InitTypeDef  SSP_InitStructure; 
	uint8_t MID,ID1,ID2;
	
	SSP_DeInit(spix);
	
	//SPI�ܽŸ�������
	SSP_PinRemap(spix);
	//SPI��ʼ������
	SSP_InitStructure.CLK 			 = SSP_CLK_12Prescale;	//ʱ��Ԥ��Ƶ
	SSP_InitStructure.SCR			 = SSP_SCR_0;	//ʱ����������
	SSP_InitStructure.Data_Size		 = SSP_Data_Size_8bit;	//����λ��
	SSP_InitStructure.LBM 			 = SSP_LBM_Normal;		//�Ƿ��Ի�
	SSP_InitStructure.Mode 			 = SSP_SPH0SPO0;		//����ģʽ
	SSP_InitStructure.TXRXSIMULT 	 = SSP_TXRXSIMULT_Time_Sharing;	//�շ���ʽ
	SSP_InitStructure.Width 		 = SSP_Width_Standard;	//���ݿ��
	SSP_Init(spix,&SSP_InitStructure);
		
	SSP_Set_ReceiveDataNum(spix, 0x0000003);
	//���Ͷ�ȡFlashID����
	SSP_SendData(spix, 0x9F);//Read Flash ID Operation Code
	SSP_Enable(spix,ENABLE);
	SSP_Wait_SendFinish(spix);
	//���ն�ȡ����ID		
	SSP_Wait_ReceiveReady(spix);
	MID = SSP_ReceiveData(spix);
	SSP_Wait_ReceiveReady(spix);
	ID1 = SSP_ReceiveData(spix);
	SSP_Wait_ReceiveReady(spix);
	ID2 = SSP_ReceiveData(spix);
	SSP_Enable(spix,DISABLE);
	
	printf("FlashID:MID:0x%x ,ID:0x%x%x\r\n",MID,ID1,ID2);
}	

//SPI�Ի�����
void SSP_Loop(SSP_TypeDef* spix)
{		
	SSP_InitTypeDef  SSP_InitStructure; 
	uint8_t data1,data2,data3,data4;
	
	SSP_DeInit(spix);
	
	//SPI�ܽŸ�������
	SSP_PinRemap(spix);
	//SPI��ʼ������
	SSP_InitStructure.CLK 			 = SSP_CLK_12Prescale;	//ʱ��Ԥ��Ƶ
	SSP_InitStructure.SCR			 = SSP_SCR_0;	//ʱ����������
	SSP_InitStructure.Data_Size		 = SSP_Data_Size_8bit;	//����λ��
	SSP_InitStructure.LBM 			 = SSP_LBM_LoopBack;		//�Ƿ��Ի�
	SSP_InitStructure.Mode 			 = SSP_SPH0SPO0;		//����ģʽ
	SSP_InitStructure.TXRXSIMULT 	 = SSP_TXRXSIMULT_simultaneous;	//�շ���ʽ
	SSP_InitStructure.Width 		 = SSP_Width_Standard;	//���ݿ��
	SSP_Init(spix,&SSP_InitStructure);
	
	//ͨ��SPI���Ͳ�������
	data1=0x89;
	data2=0x34;
	printf("SPI_LoopBack_Send:dat1=0x%x,dat2=0x%x\r\n",data1,data2);
	
	SSP_SendData(spix, data1);
	SSP_SendData(spix, data2);
	SSP_Enable(spix,ENABLE);
	//SSP_SendFinish(spix);	//����ȴ��������
	//�����Ի�����������
	SSP_Wait_ReceiveReady(spix);
	data3 = SSP_ReceiveData(spix);
	SSP_Wait_ReceiveReady(spix);
	data4 = SSP_ReceiveData(spix);
	SSP_Enable(spix,DISABLE);
	
	printf("SPI_LoopBack_Recv:dat1=0x%x,dat2=0x%x\r\n",data3,data4);
}


void SPI_test(void)
{
	SSP_TypeDef* spix;
	
	//��Ҫ��Flash��Hold�ܽŵ�ƽ����
	GPIO_SetBit(STAR_GPIO0, GPIO_Pin_21); //HOLDn Set 1
	
	spix = STAR_SSP0;
	SPI_Read_Flash_ID(spix);
	SSP_Loop(spix);
}


