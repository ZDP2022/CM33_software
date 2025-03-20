#include "STAR_rcc.h"
#include "STAR_gpio.h"
#include "STAR.h"
#include "STAR_i2c.h"
#include "BL24C08.h"
#include <stdio.h>

//STAR I2C OWN_ADDR
#define I2C_OWN_ADDR 	0x000B

//EEPROM I2C�豸��ַ
#define EEPROM_DEV_ADDR 0xA0


//�����EEPROM����д��n���ֽ�����
//slave_addr:��Ҫд�����ݵ�EEPROM ���豸��ַ
//addr:��ʼд�����ݵĴ洢��ַ
//Num:��Ҫд�����ݵĸ���
//Buffer:Ҫд�������buffer
void I2C_ByteWrite_nBytes (uint8_t slave_addr,uint8_t word_addr,uint8_t Num,uint8_t* Buffer)
{	
	uint16_t addr_wtite = 0x0000;
	
	I2C_DeInit(STAR_I2C);
	//����Ӧ��GPIO����ΪI2C�Ĺܽ�
	GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_I2C0SCL, ENABLE); //GPIO[15]
	GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_I2C0SDA, ENABLE); //GPIO[16]
	//I2C����: I2C��ģʽ����׼I2C����	
	I2C_Init(STAR_I2C, I2C_Mode_Master, I2C_Speed_Standard, I2C_OWN_ADDR);
	//ʹ�ܷ���
	I2C_TxEnable( STAR_I2C, ENABLE);
	//�����豸��ַ
	addr_wtite = 0x0000 | slave_addr;
	I2C_SlvaddrConfig( STAR_I2C, addr_wtite);
	//��ʼ�����豸��ַ������
	I2C_GenerateSTART( STAR_I2C,ENABLE);
	
	I2C_TxRxDataNumConfig(STAR_I2C,2);
	
	I2C_SendData(STAR_I2C, word_addr>>8); 	//24C64 wordAddr ��16bit �����η���
	while(I2C_CheckACKIsFail(STAR_I2C));	//�ȴ�ACK
	I2C_SendData(STAR_I2C, word_addr&0xff);
	while(I2C_CheckACKIsFail(STAR_I2C));	//�ȴ�ACK
	
	while(Num)
	{
		I2C_TxRxDataNumConfig(STAR_I2C,1);
		I2C_WaitTxFIFOIsNotFull(STAR_I2C);//�ȴ�����FIFO�п�λ
		I2C_SendData(STAR_I2C, *Buffer);
		while(I2C_CheckACKIsFail(STAR_I2C));//�ȴ�ACK
		Buffer ++;
		Num--;
	}
	I2C_WaitSendDataFinish(STAR_I2C);	
	I2C_GenerateSTOP(STAR_I2C,ENABLE );
}

//EEPROM�����ַ��ȡ
//slave_addr:��Ҫ��ȡ���ݵ�EEPROM���豸��ַ
//word_addr:��Ҫ��ʼ��ȡ���ݵĴ洢��ַ
//rx_data_num:��ȡ���ݵ��ֽ���
//Buffer:�洢��ȡ�����ݵ�buffer
void I2C_EEPROM_RandomRead(uint8_t slave_addr,uint8_t word_addr,uint16_t rx_data_num,uint8_t* Buffer)
{		
	uint16_t addr_read  = 0x8000;	//��д������SLV_ADDR��bit[15]ȷ������λΪ0��ʾд������Ϊ1��ʾ������
	uint16_t addr_wtite = 0x0000;
	
	I2C_DeInit(STAR_I2C);
	//����Ӧ��GPIO����ΪI2C�Ĺܽ�
	GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_I2C0SCL, ENABLE); //GPIO[15]
	GPIO_PinRemapConfig( STAR_GPIO0, GPIO_Remap_I2C0SDA, ENABLE); //GPIO[16]
	//I2C����: I2C��ģʽ����׼I2C����
	I2C_Init(STAR_I2C, I2C_Mode_Master, I2C_Speed_Standard, I2C_OWN_ADDR);//0x001a
	//ʹ�ܷ���
	I2C_TxEnable( STAR_I2C, ENABLE); //TX ENABLE
	//�����豸��ַ
	addr_wtite |= slave_addr;
	
	I2C_SlvaddrConfig(STAR_I2C, addr_wtite);//
	I2C_GenerateSTART(STAR_I2C,ENABLE);
	
	I2C_TxRxDataNumConfig(STAR_I2C,2);
	I2C_SendData(STAR_I2C, word_addr>>8); //24C64 wordAddr ��16bit �����η���
	while(I2C_CheckACKIsFail(STAR_I2C));//while(I2C_Get_ACK_Fail_Flag());						//�ȴ�ACK
	I2C_SendData(STAR_I2C, word_addr&0xff);//08  address
	while(I2C_CheckACKIsFail(STAR_I2C));//while(I2C_Get_ACK_Fail_Flag());						//�ȴ�ACK
	I2C_WaitSendDataFinish(STAR_I2C);
	
	
	I2C_GenerateSTOP( STAR_I2C,ENABLE );
	I2C_TxEnable(STAR_I2C,DISABLE);//STAR_I2C->CONTROL |= (0UL << CM3DS_MPS2_I2C_TX_ENA_Pos);
	
	I2C_Init(STAR_I2C, I2C_Mode_Master, I2C_Speed_Standard, I2C_OWN_ADDR);//0x001a
	addr_read |= slave_addr;
	I2C_SlvaddrConfig( STAR_I2C, addr_read);//read 
	//ʹ�ܽ���
	I2C_RxEnable( STAR_I2C,ENABLE );
	//��ʼ���ն�ȡ��������
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
	
	//��EEPROM��rw_addr��ַ��ʼд��rw_num�ֽ�����
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
	
	//��EEPROM��0��ַ������ȡrw_num�ֽ����ݣ���֤֮ǰд�������
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
