#include "STAR_rcc.h"
#include "STAR_gpio.h"
#include "STAR.h"	
#include "STAR_dma.h"	
#include "dma.h"	
#include "STAR_spi.h"
#include "spi.h"
#include <stdio.h>	


static uint8_t memory_read_byte(uint32_t addr)
{
	uint8_t val ;
	val = *((uint8_t *)addr);
	return val;
}

static void memory_write_byte(uint32_t addr,uint8_t dat)
{
	uint8_t *paddr = ((uint8_t *)addr);
	 *paddr= dat;
}


static void Delay_ms(unsigned int ms)
{
	unsigned int m, n;
	for (m = 0; m <= ms; m++)
	{
		for(n= 0; n <= 3000; n++);
	}
}

//DMA内存空间到内存空间测试
void DMA_Test_MtoM(void)
{
	int i,k; 
	uint32_t w_addr = 0x0001F000; 
	uint8_t w_data=0x22;            
	uint32_t r_addr_1F1 = 0x0001F100;
	uint8_t read_addr_1F1[10];
	uint8_t rw_num =9;
	
	printf("---------------DMA Memory-To-Memory Test---------------\n");
	printf("Source_Addr:0x%x,Destination_Addr:0x%x\n",w_addr,r_addr_1F1);
	printf("-------------------------------------------------------\n");
	printf("Source_Data:");
	
	for(i=0;i<rw_num;i++)   
	{
		printf("0x%x ",w_data);
		memory_write_byte(w_addr,w_data);
		w_addr+=1;
		w_data+=1;
	}
	printf("\n");
	printf("Start DMA transfer...\n");
	
	//使能DMA控制器
	DMA_Set_Configuration(STAR_DMA,0,1); //Enable DMAC
	//设置DMA源地址和目的地址
	DMA_Set_SourceAddress(STAR_DMA,     0x0,0x0001F000);
 	DMA_Set_DestinationAddress(STAR_DMA,0x0,0x0001F100);
	//设置DMA源地址和目的地址递增
	DMA_Set_Increment(STAR_DMA,Channel_0,I_IT_Dis,DI_En,SI_En);
	//设置DMA目的传送数据位宽和源传送数据位宽均为8bit
	DMA_Set_DataWidth(STAR_DMA,Channel_0,SWorDWidth_8bit_Byte,SWorDWidth_8bit_Byte);
	//设置DMA发送数据个数为9
	DMA_Set_TransferSize(STAR_DMA,Channel_0,DB_BurstSize_1,SB_BurstSize_1,9);
	
	DMA_ChannelConfig(STAR_DMA,Channel_0, Halt_Dis, Lock_En, ITC_Mask, IE_Mask);
	//设置DMA传输类型为内存到内存
	DMA_ChannelConfig_Flow(STAR_DMA,Channel_0,MtoM_DMA);
	DMA_ChannelConfig_Peri(STAR_DMA,Channel_0,0x0,0x0,DMA_En);
	printf("DMA transfer finish!\n");
	printf("Destination_Data(Des_Addr=0x%x):",r_addr_1F1);
	//读取收到的数据
	for(k=0;k<rw_num;k++)
	{
		read_addr_1F1[k]=memory_read_byte(r_addr_1F1);
		r_addr_1F1 +=1;
		printf ("0x%x ", read_addr_1F1[k]);
		Delay_ms(100);
	}
	printf("\n");
	
	DMA_Set_Configuration(STAR_DMA,0,0); //Disable DMAC
}

//SPI 初始化并发送测试数据
static void SPI_Init_And_Send(void)
{		
	uint8_t i;
	uint8_t dat=0xa1;
	uint8_t rw_num = 23;
	
	//SPI Pin设置
	SSP_PinRemap(STAR_SSP0);	
	SSP_InitTypeDef  SSP_Initstructure; 
	//SPI设置-配置为环回模式
	SSP_Initstructure.CLK 			 = SSP_CLK_50Prescale;
	SSP_Initstructure.SCR			 = SSP_SCR_0;	//时钟速率因子
	SSP_Initstructure.Data_Size  	= SSP_Data_Size_8bit;
	SSP_Initstructure.Mode			 = SSP_SPH0SPO0;
	SSP_Initstructure.LBM 			 = SSP_LBM_LoopBack;     //SSP_LBM_Normal; SSP_LBM_LoopBack
	SSP_Initstructure.Width 		 = SSP_Width_Standard;   //SSP_Width_Standard ; SSP_Width_Dual; SSP_Width_Quad
	//收发同步																														 
	SSP_Initstructure.TXRXSIMULT = SSP_TXRXSIMULT_simultaneous;//SSP_TXRXSIMULT_Time_Sharing ; SSP_TXRXSIMULT_simultaneous
	
	SSP_Init(STAR_SSP0,&SSP_Initstructure);
	//使能SPI 接收DMA
	SSP_DMA_Request(STAR_SSP0,0,1);
	
	//SPI发送数据
	printf("SPI send data:");
	for(i=0;i<rw_num;i++)
	{
		SSP_SendData(STAR_SSP0, dat+i);
		printf("0x%x ",dat+i);
		if(SSP_Tx_FIFO_Is_Full(STAR_SSP0)==1)
		{
			SSP_Enable( STAR_SSP0, ENABLE);		//Enable SSP;
			SSP_Wait_TxFIFONotFull(STAR_SSP0);
		}
	}
	printf("\n");
	SSP_Enable( STAR_SSP0, ENABLE);		//Enable SSP;
	SSP_Wait_SendFinish(STAR_SSP0);
	SSP_Enable( STAR_SSP0, DISABLE);
	DMA_Set_Configuration(STAR_DMA,0,0); //Disable DMAC
}


//DMA外设到内存空间测试
//SPI0 接收端通过DMA接收数据并保存到内存
//SPI0 工作在自环模式下，发送的数据在接收端可以收到
void DMA_SPI_Test_PtoM(void)
{
	int k; 
	uint32_t r_addr_1F1 = 0x0001F100;
	uint8_t read_addr_1F1[64];
	uint8_t rw_num = 23;
	
	printf("---------------DMA Peripheral-To-Memory Test---------------\n");
	printf("Source_Addr:0x%x(SPI0_RX),Destination_Addr:0x%x\n",0x4000B008,r_addr_1F1);
	printf("-------------------------------------------------------\n");
	
	//使能DMA控制器
	DMA_Set_Configuration(STAR_DMA,0,1); 	
	//设置DMA的源地址为SPI0数据寄存器地址 0x4000B008
	DMA_Set_SourceAddress(STAR_DMA,0x0,STAR_SSP0_BASE+0x0008);
	//设置DMA的目的地址为内存地址0x0001F100
 	DMA_Set_DestinationAddress(STAR_DMA,0x0,0x0001F100);
	//设置DMA通道为0 源地址不递增 目的地址递增
	DMA_Set_Increment(STAR_DMA,Channel_0,I_IT_Dis,DI_En,SI_Dis);
	//设置DMA目的传送数据位宽和源传送数据位宽均为8bit
	DMA_Set_DataWidth(STAR_DMA,Channel_0,SWorDWidth_8bit_Byte,SWorDWidth_8bit_Byte);
	//设置DMA发送数据个数
	DMA_Set_TransferSize(STAR_DMA,Channel_0,DB_BurstSize_1,SB_BurstSize_1,rw_num);
	//设置DMA通道其他参数
    DMA_ChannelConfig(STAR_DMA,Channel_0, Halt_Dis, Lock_En, ITC_Mask, IE_Mask); 
	//设置DMA传输类型为外设到内存
	DMA_ChannelConfig_Flow(STAR_DMA,Channel_0,PtoM_DMA); 
	//设置DMA源外设为SPI0-RX(5) 并使能DMA 通道0
	DMA_ChannelConfig_Peri(STAR_DMA,Channel_0,0x0,0x5,DMA_En);//SPI0-TX 0x4,SPI0-RX 0x5,SPI1-TX 0x6,SPI1-RX 0x7
	
	printf("Start DMA transfer...\n");
	//SPI初始化并发送数据
	SPI_Init_And_Send();
	
	printf("\r\nDMA transfer finish!\n");
	printf("Destination_Data(Des_Addr=0x%x):",r_addr_1F1);
	
	//读取SPI发送的数据
	for(k=0;k<rw_num;k++)
	{
		read_addr_1F1[k]=memory_read_byte(r_addr_1F1);
		r_addr_1F1+=1;
		printf ("0x%x ", read_addr_1F1[k]);
		Delay_ms(100);
	}
	printf("\n");
	
}



//DMA内存空间到外设设置
static void DMA_MemoryToPeripheral(void)
{
	int i; 
	uint32_t w_addr = 0x0001F000;  
	uint8_t  w_data = 0xB2;
	uint8_t rw_num = 6;
	
	printf("Memory_Data:");	
	for(i=0;i<rw_num;i++)
	{
		printf("0x%x ",w_data);
		memory_write_byte(w_addr,w_data);
		w_addr+=1;
		w_data+=1;
	}	
	//使能DMA控制器
	DMA_Set_Configuration(STAR_DMA,0,1); //Enable DMAC	
	//设置DMA的源地址为内存地址0x0001F100
	DMA_Set_SourceAddress(STAR_DMA,     0x0,0x0001F000);
	//设置DMA的源地址为SPI0数据寄存器地址 0x4000B008
 	DMA_Set_DestinationAddress(STAR_DMA,0x0,0x4000B008);
	//设置DMA通道为0 源地址递增 目的地址不递增
	DMA_Set_Increment(STAR_DMA,Channel_0,I_IT_Dis,DI_Dis,SI_En);
	//设置DMA目的传送数据位宽和源传送数据位宽均为8bit
	DMA_Set_DataWidth(STAR_DMA,Channel_0,SWorDWidth_8bit_Byte,SWorDWidth_8bit_Byte);
	//设置DMA发送数据个数为6
	DMA_Set_TransferSize(STAR_DMA,Channel_0,DB_BurstSize_1,SB_BurstSize_1,rw_num);
	DMA_ChannelConfig(STAR_DMA,Channel_0, Halt_Dis, Lock_Dis, ITC_Mask, ITC_Mask);
	//设置DMA传输类型为内存到外设
	DMA_ChannelConfig_Flow(STAR_DMA,Channel_0,MtoP_DMA); 
	//设置DMA源外设为SPI0-TX(4) 并使能DMA 通道0
	DMA_ChannelConfig_Peri(STAR_DMA,Channel_0,0x4,0x0,DMA_En); //SPI0-TX 0x4,SPI0-RX 0x5,SPI1-TX 0x6,SPI1-RX 0x7

}

//DMA 内存空间到外设测试
//SPI 发送端通过DMA发送数据,发送的数据在内存区域
//SPI工作在自环模式下，发送的数据在接收端可以收到
void DMA_SPI_Test_MtoP(void)
{
	uint8_t rx_data_spi[10];
	int m;
	uint8_t rw_num = 6;
	
	printf("---------------DMA Memory-To-Peripheral Test---------------\n");
	printf("Source_Addr:0x%x,Destination_Addr:0x%x(SPI0_TX)\n",0x0001F000,0x4000B008);
	printf("-------------------------------------------------------\n");
	
	printf("Start DMA transfer...\n");
	
	SSP_DeInit(STAR_SSP0);
	//SPI Pin设置
	SSP_PinRemap(STAR_SSP0);
	////SPI设置-配置为环回模式
	SSP_InitTypeDef  SSP_Initstructure; 
	SSP_Initstructure.CLK 			 = SSP_CLK_50Prescale;
	SSP_Initstructure.SCR			 = SSP_SCR_0;	//时钟速率因子
	SSP_Initstructure.Data_Size  = SSP_Data_Size_8bit;
	SSP_Initstructure.Mode			 = SSP_SPH0SPO0;
	SSP_Initstructure.LBM 			 = SSP_LBM_LoopBack;     //SSP_LBM_Normal     ; SSP_LBM_LoopBack
	SSP_Initstructure.Width 		 = SSP_Width_Standard;   //SSP_Width_Standard ; SSP_Width_Dual   ; SSP_Width_Quad
	//收发同步																														 
	SSP_Initstructure.TXRXSIMULT = SSP_TXRXSIMULT_simultaneous;//SSP_TXRXSIMULT_Time_Sharing ;SSP_TXRXSIMULT_simultaneous
	SSP_Init(STAR_SSP0,&SSP_Initstructure);
	
	SSP_Enable( STAR_SSP0, ENABLE);		//Enable SSP;
	
	//DMA设置
	DMA_MemoryToPeripheral();
	//使能SPI 发送DMA
	SSP_DMA_Request(STAR_SSP0,1,0);//int TX_EN,int RX_EN
	
	printf("\r\nDMA transfer finish!\n");
	
	//SSP_ReceiveReady(STAR_SSP0);
	printf("SPI Recv Data:");
	for(m=0;m<rw_num;m++)
	{
		rx_data_spi[m] = SSP_ReceiveData(STAR_SSP0);
		printf ("0x%x ", rx_data_spi[m]);
	}
	printf("\n");
	SSP_Enable( STAR_SSP0, DISABLE);
	DMA_Set_Configuration(STAR_DMA,0,0); //Disable DMAC
}


void DMA_test(void)
{
	//DMA_Test_MtoM();
	DMA_SPI_Test_PtoM();
	//DMA_SPI_Test_MtoP();
}
