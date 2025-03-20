#include "spi.h"
#include "flash_spi.h"
#include "STAR_gpio.h"
#include "STAR_spi.h"
#include "STAR_rcc.h"
#include <stdio.h> 
#include <string.h>

//Flash SPI��ʼ��
void flash_spi_init(SSP_TypeDef* SSPx)
{
	SSP_InitTypeDef  SSP_InitStructure;
	
	//��Ҫ��Flash��Hold�ܽŵ�ƽ����
	GPIO_SetBit(STAR_GPIO0, GPIO_Pin_21); //HOLDn Set 1
	
	SSP_DeInit(SSPx);
	
	//SPI�ܽŸ�������
	SSP_PinRemap(SSPx);
	
	SSP_InitStructure.CLK 			 = SSP_CLK_12Prescale;	//ʱ��Ԥ��Ƶ
	SSP_InitStructure.SCR			 = SSP_SCR_0;	//ʱ����������
	SSP_InitStructure.Data_Size  	= SSP_Data_Size_8bit;	//����λ��
	SSP_InitStructure.Mode			 = SSP_SPH0SPO0;		//����ģʽ
	SSP_InitStructure.LBM 			 = SSP_LBM_Normal;   	//�Ƿ��Ի�
	SSP_InitStructure.Width 		 = SSP_Width_Standard;	//���ݿ��
	SSP_InitStructure.TXRXSIMULT = SSP_TXRXSIMULT_Time_Sharing; //�շ���ʽ
	SSP_Init(SSPx,&SSP_InitStructure);
	
}

//Flashдʹ��
void Flash_WriterEnable(SSP_TypeDef* SSPx)
{	
	SSP_SendData(SSPx, WB_W25Q_WRITE_EN);// Write Enable
	SSP_Enable(SSPx, ENABLE);
	SSP_Wait_SendFinish(SSPx);
	SSP_Enable(SSPx, DISABLE);
	
}

//��ȡFLASH״̬�Ĵ���1��ֵ
uint8_t Flash_Read_StatusRegister1(SSP_TypeDef* SSPx)
{	
	uint8_t sr1data;
	
	SSP_Set_ReceiveDataNum(SSPx, 0x0001);
	SSP_SendData(SSPx, WB_W25Q_READ_SR1);// Read State-1   0x05
	SSP_Enable( SSPx, ENABLE);
	SSP_Wait_SendFinish(SSPx);
	
	SSP_Wait_ReceiveReady(SSPx);
	sr1data = SSP_ReceiveData( SSPx);
	SSP_Enable( SSPx, DISABLE);
	return sr1data;
}

//��ȡFLASH��״̬�Ĵ���2��ֵ
uint8_t Flash_Read_StatusRegister2(SSP_TypeDef* SSPx)
{	
	uint8_t sr2data;
	
	SSP_Set_ReceiveDataNum(SSPx, 0x0001);
	SSP_SendData(SSPx, WB_W25Q_READ_SR2);// Read State-2     0x35
	SSP_Enable( SSPx, ENABLE);
	SSP_Wait_SendFinish(SSPx);
	SSP_Wait_ReceiveReady(SSPx);
	sr2data = SSP_ReceiveData( SSPx);
	SSP_Enable( SSPx, DISABLE);
	
	return sr2data;
}


//дFlash״̬�Ĵ���1��״̬�Ĵ���2
void Flash_Write_StatusRegister(SSP_TypeDef* SSPx,uint8_t R1,uint8_t R2)
{	
	Flash_WriterEnable(SSPx);
	
	SSP_SendData(SSPx, WB_W25Q_WRITE_SR);	// Write Status Register Ena
	SSP_SendData(SSPx, R1);		//Register-1
	SSP_SendData(SSPx, R2);		//Register-2
	SSP_Enable( SSPx, ENABLE);
	SSP_Wait_SendFinish(SSPx);
	SSP_Enable( SSPx, DISABLE);
	
	//Flash_Read_StatusRegister1(SSPx);
	//Flash_Read_StatusRegister2(SSPx);
	
}

//�ȴ�FLASH��״̬�Ĵ���1��BUSYλΪ0
void SSP_Flash_WaitBusyEnd(SSP_TypeDef* SSPx)
{	
	uint8_t revdata;
	do
	{
		SSP_Set_ReceiveDataNum(SSPx, 0x00001);
		SSP_SendData(SSPx, WB_W25Q_READ_SR1);// Read State-1   0x05
		SSP_Enable( SSPx, ENABLE);
		SSP_Wait_SendFinish(SSPx);
		
		SSP_Wait_ReceiveReady(SSPx);
		revdata = SSP_ReceiveData(SSPx);
		SSP_Enable( SSPx, DISABLE);
	}while((revdata & 0x0001) == 0x0001);
}

//Flash�������� 
void Flash_SectorErase(SSP_TypeDef* SSPx,uint32_t Sectoraddr)
{	
	Flash_WriterEnable(SSPx);
	SSP_SendData(SSPx, WB_W25Q_SECTOR_ERASE);							// 4KB Sector Erase
	SSP_SendData(SSPx, (Sectoraddr & 0xFF0000)>>16);	// Erase Addr:high
	SSP_SendData(SSPx, (Sectoraddr & 0xFF00)>>8);		// Erase Addr:middle
	SSP_SendData(SSPx, (Sectoraddr & 0xFF));			// Erase Addr:low
	SSP_Enable( SSPx, ENABLE);
	SSP_Wait_SendFinish(SSPx);
	SSP_Enable( SSPx, DISABLE);
	SSP_Flash_WaitBusyEnd(SSPx);
}


//Flashҳд����
void Flash_PageWrite(SSP_TypeDef* SSPx,uint8_t* Buffer,uint32_t Addr,uint16_t Num)
{	
	
	int j;
	Flash_WriterEnable(SSPx);
	
	SSP_SendData(SSPx, 0x02);// Page Write
	SSP_SendData(SSPx, (Addr & 0xFF0000)>>16);// Write Addr:high
	SSP_SendData(SSPx, (Addr & 0xFF00)>>8);	 // Write Addr:middle
	SSP_SendData(SSPx, (Addr & 0xFF));			   // Write Addr:low
	SSP_Enable( SSPx, ENABLE);		//Enable SSP;

	for(j=0;j<Num;j++)
	{
		SSP_SendData(SSPx, *Buffer);
		SSP_Wait_TxFIFONotFull(SSPx);//while(!(SSPx->SR & SSP_SR_TNF_Msk));
		Buffer ++;
	}
	SSP_Wait_SendFinish(SSPx);
	SSP_Enable( SSPx, DISABLE);
	
	SSP_Flash_WaitBusyEnd(SSPx);
}


//Flash��������оƬ
void Flash_ChipErase(SSP_TypeDef* SSPx)
{	
	Flash_WriterEnable(SSPx);	
	
	SSP_SendData(SSPx, WB_W25Q_ALL_ERASE);
	SSP_Enable( SSPx, ENABLE);
	SSP_Wait_SendFinish(SSPx);
	SSP_Enable( SSPx, DISABLE);
	
	SSP_Flash_WaitBusyEnd(SSPx);
}


//Flashҳ������
void Flash_PageRead(SSP_TypeDef* SSPx,uint8_t* Buffer,uint32_t Addr,uint16_t Num)
{
	int j=0;
	
	SSP_Set_ReceiveDataNum(SSPx, Num);
	SSP_SendData(SSPx, WB_W25Q_READ);			//Read Data Order  0x03  ��2��0x3b  ��4��0x6b
	SSP_SendData(SSPx, (Addr & 0xFF0000)>>16);	//Read Data Addr:high
	SSP_SendData(SSPx, (Addr & 0xFF00)>>8);		//Read Data	Addr:middle
	SSP_SendData(SSPx, (Addr & 0xFF));					//Read Data Addr:low
	
	SSP_Enable(SSPx, ENABLE);	//Enable SSP;
	SSP_Wait_SendFinish(SSPx);
	SSP_Wait_ReceiveReady(SSPx);
	for(j=0;j<Num;j++)
	{
		SSP_Wait_ReceiveReady(SSPx);
		*Buffer = SSP_ReceiveData(SSPx);
		Buffer ++;
	}
	SSP_Enable( SSPx, DISABLE);
}

uint8_t Tx_Buffer[]="STAR flash read and write test...\r\n";
uint8_t Rx_Buffer[256];

void flash_test(void)
{
	SSP_TypeDef* spix;
	uint32_t datlen=0;
	
	spix = STAR_SSP0;
	
	
	flash_spi_init(spix);
	
	if(spix==STAR_SSP0)
	{
		printf("-----STAR_SPI0 Test-----\r\n");
	}
	else
	{
		printf("-----STAR_SPI1 Test-----\r\n");
	}
	
	datlen = sizeof(Tx_Buffer);
	//��ȡFlashID
	//SPI_Read_Flash_ID(spix);
	
	//Sector������Sector��ʼ��ַΪ0x0
	Flash_SectorErase(spix,0x0000);
	printf ("Flash_SectorErase,addr=0x%x\n",0x0);
	//��ҳд������,ҳ����ʼ��ַΪ0x0
	Flash_PageWrite(spix,Tx_Buffer, 0x0000,datlen);
	printf ("Flash_PageWrite,addr=0x%x,write_data:%s\n",0x0,Tx_Buffer);
	//��ҳ��ȡ���ݣ�ҳ����ʼ��ַΪ0x0
	memset(Rx_Buffer,0x0,sizeof(Rx_Buffer));
	Flash_PageRead(spix,Rx_Buffer,0x0000,datlen);
	printf ("Flash_PageRead,addr=0x0,read_data:%s\n", Rx_Buffer);
}

