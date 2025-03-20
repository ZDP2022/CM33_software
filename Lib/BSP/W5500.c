#include "STAR_spi.h"
#include "W5500.h"

#define W5500_HAL 1 //选择使用硬件SPI
#define W5500_IO 0 //选择IO模拟SPI

/***************----- 网络参数变量定义 -----***************/
unsigned char Gateway_IP[4]; //网关IP地址
unsigned char Sub_Mask[4]; //子网掩码
unsigned char Phy_Addr[6]; //物理地址(MAC)
unsigned char IP_Addr[4]; //本机IP地址

unsigned char S0_Port[2]; //端口0的端口号
unsigned char S0_DIP[4]; //端口0目的IP地址
unsigned char S0_DPort[2]; //端口0目的端口号

unsigned char UDP_DIPR[4]; //UDP(广播)模式,目的主机IP地址
unsigned char UDP_DPORT[2]; //UDP(广播)模式,目的主机端口号

/***************----- 端口的运行模式 -----***************/
unsigned char S0_Mode = 3; //端口0的运行模式,0:TCP服务器模式,1:TCP客户端模式,2:UDP(广播)模式

#define TCP_SERVER	0x00	//TCP服务器模式
#define TCP_CLIENT	0x01	//TCP客户端模式 
#define UDP_MODE	0x02	//UDP(广播)模式 
/***************----- 端口的运行状态 -----***************/
unsigned char S0_State = 0; //端口0状态记录,1:端口完成初始化,2端口完成连接(可以正常传输数据)
#define S_INIT		0x01	//端口完成初始化 
#define S_CONN		0x02	//端口完成连接,可以正常传输数据 
/***************----- 端口收发数据的状态 -----***************/
unsigned char S0_Data; //端口0接收和发送数据的状态,1:端口接收到数据,2:端口发送数据完成
#define S_RECEIVE	 0x01	//端口接收到一个数据包 
#define S_TRANSMITOK 0x02	//端口发送一个数据包完成 
/***************----- 端口数据缓冲区 -----***************/
unsigned char Rx_Buffer[3000]; //端口接收数据缓冲区
unsigned char Tx_Buffer[3000]; //端口发送数据缓冲区

unsigned char W5500_Interrupt; //W5500中断标志(0:无中断,1:有中断)

//控制W5500的片选信号
unsigned char W5500_CS(unsigned char i)
{
	GPIO_WriteBit(STAR_GPIO0,SPI_CS,i);
}

void W5500_SPI_Init(void)
{
	#if W5500_HAL 
	SSP_InitTypeDef  SSP_Initstructure; 
	SSP_PinRemap();
	SSP_Initstructure.CLK 			 = SSP_CLK_4Prescale;//SPI的工作频率
	SSP_Initstructure.Data_Size  = SSP_Data_Size_8bit;//数据位数
	SSP_Initstructure.Mode			 = SSP_SPH0SPO0;	//工作模式选择：模式0：CPOL=0 CPOHA=0
	SSP_Initstructure.LBM 			 = SSP_LBM_Normal;   //SSP_LBM_Normal（正常串口模式） ; SSP_LBM_LoopBack（回环模式）
	SSP_Initstructure.Width 		 = SSP_Width_Standard;   //数据位宽
	SSP_Initstructure.TXRXSIMULT = SSP_TXRXSIMULT_Time_Sharing;//传输方式（分时或同时）：同时收发只适用于回环模式；分时收发适用于单独的传输或接收
	SSP_Init(STAR_SSP1,&SSP_Initstructure);
	#endif
	
	#if W5500_IO
	SPI_GPIO_Init();//IO模拟SPI初始化
	#endif
	
	W5500_CS(1);//初始化CS为高电平
}

//GPIO复用,用以设置SPI
void SSP_PinRemap(void)
{		
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI1, DISABLE);
	GPIO_PinRemapConfig(STAR_GPIO0,SPI_SCLK,ENABLE); //GPIO[11]  CLK_OUT   
	GPIO_PinRemapConfig(STAR_GPIO0,SPI_MOSI,ENABLE); //GPIO[13]	MOSI	    
	GPIO_PinRemapConfig(STAR_GPIO0,SPI_MISO,ENABLE); //GPIO[14]	MISO
	//设置CS
	STAR_GPIO0->OUTENSET |= ( 1 << SPI_CS );//设置CS为输出
	STAR_GPIO0->ALTFUNCCLR  = ( 1 << SPI_CS );//关闭复用
	STAR_GPIO0->INTENCLR = ( 1 << SPI_CS );//关闭中断	
}
/*******************************************************************************
 * 函数名  : SPI_ReadByte
 * 描述    : 读取一个字节SPI返回的数据
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 读取到的寄存器数据
 * 说明    : 无
 *******************************************************************************/
unsigned char SPI_Read_Byte(void) {
	unsigned char rByte = 0;
	
	#if W5500_HAL
	SSP_Wait_ReceiveReady(STAR_SSP1);//是否可以开始传输数据
	rByte = SSP_ReceiveData(STAR_SSP1);
	SSP_Enable(STAR_SSP1,DISABLE);//SPI不使能
	#endif
	
	#if W5500_IO
	rByte = GPIO_SPI_Read_Byte();//IO模拟读取数据
	#endif
	
	return rByte;
}
/*******************************************************************************
 * 函数名  : SPI_Read_2Byte
 * 描述    : 读取两个字节SPI返回的数据
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 读取到的寄存器数据
 * 说明    : 无
 *******************************************************************************/
unsigned short SPI_Read_2Byte(void) {
	unsigned char rByteH = 0;
	unsigned char rByteL = 0;
	
	#if W5500_HAL
	SSP_Wait_ReceiveReady(STAR_SSP1);//是否可以开始传输数据
	rByteH = SSP_ReceiveData(STAR_SSP1);
	SSP_Wait_ReceiveReady(STAR_SSP1);//是否可以开始传输数据
	rByteL = SSP_ReceiveData(STAR_SSP1);
	SSP_Enable(STAR_SSP1,DISABLE);//SPI不使能
	#endif
	
	#if W5500_IO
	rByteH = GPIO_SPI_Read_Byte();
	rByteL = GPIO_SPI_Read_Byte();
	#endif
	
	return ((rByteH<<8) | rByteL);
}
/*******************************************************************************
 * 函数名  : SPI_Read_NByte
 * 描述    : 读取N个字节SPI返回的数据
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 读取到的寄存器数据
 * 说明    : 无
 *******************************************************************************/
void SPI_Read_NByte (unsigned short Byte,unsigned char *dat_ptr) {
	unsigned short i;
	unsigned char j;
	
	#if W5500_HAL
	for (i = 0; i < Byte; i++) //循环读取Byte个字节数据
	{
		SSP_Wait_ReceiveReady(STAR_SSP1);//是否可以开始传输数据
		j = SSP_ReceiveData(STAR_SSP1); //读取1个字节数据
		*dat_ptr = j; //将读取到的数据保存到数据保存缓冲区
		dat_ptr++; //数据保存缓冲区指针地址自增1
	}
	SSP_Enable(STAR_SSP1,DISABLE);//SPI不使能
	#endif
	
	#if W5500_IO
	for (i = 0; i < Byte; i++) //循环读取Byte个字节数据
	{
		j = GPIO_SPI_Read_Byte(); //读取1个字节数据
		*dat_ptr = j; //将读取到的数据保存到数据保存缓冲区
		dat_ptr++; //数据保存缓冲区指针地址自增1
	}
	#endif
}

/*******************************************************************************
 * 函数名  : SPI_SendByte
 * 描述    : SPI发送一个字节数据
 * 输入    : dt:待发送的数据
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void SPI_Send_Byte(unsigned char dt) {
	#if W5500_HAL
	SSP_SendData(STAR_SSP1, dt);
	#endif
	
	#if W5500_IO
	GPIO_SPI_Write_Byte(dt);
	#endif
}

/*******************************************************************************
 * 函数名  : SPI_Send_Short
 * 描述    : SPI发送2个字节数据(16位)
 * 输入    : dat:待发送的16位数据
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void SPI_Send_Short(unsigned short dt) {
	#if W5500_HAL
	SPI_Send_Byte((unsigned char) (dt / 256)); //写数据高位
	SPI_Send_Byte(dt); //写数据低位 
	#endif
	
	#if W5500_IO
	GPIO_SPI_Write_Byte((unsigned char) (dt / 256));
	GPIO_SPI_Write_Byte(dt);
	#endif
}

/*******************************************************************************
 * 函数名  : Write_W5500_1Byte
 * 描述    : 通过SPI向指定地址寄存器写1个字节数据
 * 输入    : reg:16位寄存器地址,dat:待写入的数据
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void Write_W5500_1Byte(unsigned short reg, unsigned char dat) {
	W5500_CS(0);//拉低片选，开始传输数据
	SPI_Send_Short(reg); //通过SPI写16位寄存器地址
	SPI_Send_Byte(FDM1 | RWB_WRITE | COMMON_R); //通过SPI写控制字节,1个字节数据长度,写数据,选择通用寄存器
	SPI_Send_Byte(dat); //写1个字节数据
	
	#if W5500_HAL
	SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
	SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	SSP_Enable(STAR_SSP1,DISABLE);
	#endif
	
	W5500_CS(1);
}

/*******************************************************************************
 * 函数名  : Write_W5500_2Byte
 * 描述    : 通过SPI向指定地址寄存器写2个字节数据
 * 输入    : reg:16位寄存器地址,dat:16位待写入的数据(2个字节)
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void Write_W5500_2Byte(unsigned short reg, unsigned short dat) {
	W5500_CS(0);//拉低片选，开始传输数据
	SPI_Send_Short(reg); //通过SPI写16位寄存器地址
	SPI_Send_Byte(FDM2 | RWB_WRITE | COMMON_R); //通过SPI写控制字节,2个字节数据长度,写数据,选择通用寄存器
	SPI_Send_Short(dat); //写16位数据
	
	#if W5500_HAL
	SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
	SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	SSP_Enable(STAR_SSP1,DISABLE);
	#endif 
	
	W5500_CS(1);
}

/*******************************************************************************
 * 函数名  : Write_W5500_nByte
 * 描述    : 通过SPI向指定地址寄存器写n个字节数据
 * 输入    : reg:16位寄存器地址,*dat_ptr:待写入数据缓冲区指针,size:待写入的数据长度
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void Write_W5500_nByte(unsigned short reg, unsigned char *dat_ptr,
		unsigned short size) {
	unsigned short i;
	W5500_CS(0);
	SPI_Send_Short(reg); //通过SPI写16位寄存器地址
	SPI_Send_Byte(VDM | RWB_WRITE | COMMON_R); //通过SPI写控制字节,N个字节数据长度,写数据,选择通用寄存器
	for (i = 0; i < size; i++) //循环将缓冲区的size个字节数据写入W5500
			{
		SPI_Send_Byte(*dat_ptr++); //写一个字节数据
	}
			
	#if W5500_HAL
	SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
	SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	SSP_Enable(STAR_SSP1,DISABLE);
	#endif
	
	W5500_CS(1);
}

/*******************************************************************************
 * 函数名  : Write_W5500_SOCK_1Byte
 * 描述    : 通过SPI向指定端口寄存器写1个字节数据
 * 输入    : s:端口号,reg:16位寄存器地址,dat:待写入的数据
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void Write_W5500_SOCK_1Byte(SOCKET s, unsigned short reg, unsigned char dat) {
	W5500_CS(0);
	SPI_Send_Short(reg); //通过SPI写16位寄存器地址
	SPI_Send_Byte(FDM1 | RWB_WRITE | (s * 0x20 + 0x08)); //通过SPI写控制字节,1个字节数据长度,写数据,选择端口s的寄存器
	SPI_Send_Byte(dat); //写1个字节数据
	
	#if W5500_HAL
	SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
	SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	SSP_Enable(STAR_SSP1,DISABLE);
	#endif
	
	W5500_CS(1);
}

/*******************************************************************************
 * 函数名  : Write_W5500_SOCK_2Byte
 * 描述    : 通过SPI向指定端口寄存器写2个字节数据
 * 输入    : s:端口号,reg:16位寄存器地址,dat:16位待写入的数据(2个字节)
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void Write_W5500_SOCK_2Byte(SOCKET s, unsigned short reg, unsigned short dat) {
	W5500_CS(0);
	SPI_Send_Short(reg); //通过SPI写16位寄存器地址
	SPI_Send_Byte(FDM2 | RWB_WRITE | (s * 0x20 + 0x08)); //通过SPI写控制字节,2个字节数据长度,写数据,选择端口s的寄存器
	SPI_Send_Short(dat); //写16位数据
	
	#if W5500_HAL
	SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
	SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	SSP_Enable(STAR_SSP1,DISABLE);
	#endif
	
	W5500_CS(1);
}

/*******************************************************************************
 * 函数名  : Write_W5500_SOCK_4Byte
 * 描述    : 通过SPI向指定端口寄存器写4个字节数据
 * 输入    : s:端口号,reg:16位寄存器地址,*dat_ptr:待写入的4个字节缓冲区指针
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void Write_W5500_SOCK_4Byte(SOCKET s, unsigned short reg,
		unsigned char *dat_ptr) {
	W5500_CS(0);
	SPI_Send_Short(reg); //通过SPI写16位寄存器地址
	SPI_Send_Byte(FDM4 | RWB_WRITE | (s * 0x20 + 0x08)); //通过SPI写控制字节,4个字节数据长度,写数据,选择端口s的寄存器

	SPI_Send_Byte(*dat_ptr++); //写第1个字节数据
	SPI_Send_Byte(*dat_ptr++); //写第2个字节数据
	SPI_Send_Byte(*dat_ptr++); //写第3个字节数据
	SPI_Send_Byte(*dat_ptr++); //写第4个字节数据
	
	#if W5500_HAL
	SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
	SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	SSP_Enable(STAR_SSP1,DISABLE);
	#endif		
	
	W5500_CS(1);
}

/*******************************************************************************
 * 函数名  : Read_W5500_1Byte
 * 描述    : 读W5500指定地址寄存器的1个字节数据
 * 输入    : reg:16位寄存器地址
 * 输出    : 无
 * 返回值  : 读取到寄存器的1个字节数据
 * 说明    : 无
 *******************************************************************************/
unsigned char Read_W5500_1Byte(unsigned short reg) {
	unsigned char i;
	W5500_CS(0);
	
	#if W5500_HAL
	SSP_Set_ReceiveDataNum(STAR_SSP1, 1);
	#endif
	
	SPI_Send_Short(reg); //通过SPI写16位寄存器地址
	SPI_Send_Byte(FDM1 | RWB_READ | COMMON_R); //通过SPI写控制字节,1个字节数据长度,读数据,选择通用寄存器
	
	#if W5500_HAL
	SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
	SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	#endif
	
	i = SPI_Read_Byte();
	W5500_CS(1);
	return i; //返回读取到的寄存器数据
}

/*******************************************************************************
 * 函数名  : Read_W5500_SOCK_1Byte
 * 描述    : 读W5500指定端口寄存器的1个字节数据
 * 输入    : s:端口号,reg:16位寄存器地址
 * 输出    : 无
 * 返回值  : 读取到寄存器的1个字节数据
 * 说明    : 无
 *******************************************************************************/
unsigned char Read_W5500_SOCK_1Byte(SOCKET s, unsigned short reg) {
	unsigned char i;
	W5500_CS(0);
	
	#if W5500_HAL
	SSP_Set_ReceiveDataNum(STAR_SSP1, 1);
	#endif
	
	SPI_Send_Short(reg); //通过SPI写16位寄存器地址
	SPI_Send_Byte(FDM1 | RWB_READ | (s * 0x20 + 0x08)); //通过SPI写控制字节,1个字节数据长度,读数据,选择端口s的寄存器
	
	#if W5500_HAL
	SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
	SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	#endif
	
	i = SPI_Read_Byte();
	W5500_CS(1);
	return i; //返回读取到的寄存器数据
}

/*******************************************************************************
 * 函数名  : Read_W5500_SOCK_2Byte
 * 描述    : 读W5500指定端口寄存器的2个字节数据
 * 输入    : s:端口号,reg:16位寄存器地址
 * 输出    : 无
 * 返回值  : 读取到寄存器的2个字节数据(16位)
 * 说明    : 无
 *******************************************************************************/
unsigned short Read_W5500_SOCK_2Byte(SOCKET s, unsigned short reg) {
	unsigned short i;
	W5500_CS(0);
	
	#if W5500_HAL
	SSP_Set_ReceiveDataNum(STAR_SSP1, 2);
	#endif
	
	SPI_Send_Short(reg); //通过SPI写16位寄存器地址
	SPI_Send_Byte(FDM2 | RWB_READ | (s * 0x20 + 0x08)); //通过SPI写控制字节,2个字节数据长度,读数据,选择端口s的寄存器
  
	#if W5500_HAL
	SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
	SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	#endif
	
	i = SPI_Read_2Byte();
	W5500_CS(1);
	return i; //返回读取到的寄存器数据
}

/*******************************************************************************
 * 函数名  : Read_SOCK_Data_Buffer
 * 描述    : 从W5500接收数据缓冲区中读取数据
 * 输入    : s:端口号,*dat_ptr:数据保存缓冲区指针
 * 输出    : 无
 * 返回值  : 读取到的数据长度,rx_size个字节
 * 说明    : 无
 *******************************************************************************/
unsigned short Read_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr) {
	unsigned short rx_size;
	unsigned short offset, offset1;
	unsigned short i;
	unsigned char j;
	unsigned int cnt = rx_size;
	
	rx_size = Read_W5500_SOCK_2Byte(s, Sn_RX_RSR);
	if (rx_size == 0)
		return 0; //没接收到数据则返回
	if (rx_size > 1460)
		rx_size = 1460;

	offset = Read_W5500_SOCK_2Byte(s, Sn_RX_RD);
	offset1 = offset;
	offset &= (S_RX_SIZE - 1); //计算实际的物理地址
	
	W5500_CS(0);
	
	#if W5500_HAL
	SSP_Set_ReceiveDataNum(STAR_SSP1, rx_size);
	#endif
	
	SPI_Send_Short(offset); //写16位地址
	SPI_Send_Byte(VDM | RWB_READ | (s * 0x20 + 0x18)); //写控制字节,N个字节数据长度,读数据,选择端口s的寄存器
  
	#if W5500_HAL
	SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
	SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	#endif
	
	if ((offset + rx_size) < S_RX_SIZE) //如果最大地址未超过W5500接收缓冲区寄存器的最大地址
	{
			SPI_Read_NByte(rx_size,dat_ptr);

	} else //如果最大地址超过W5500接收缓冲区寄存器的最大地址
	{
		offset = S_RX_SIZE - offset;
		SPI_Read_NByte(offset,dat_ptr);
		
		W5500_CS(1);
		W5500_CS(0);
		
		#if W5500_HAL
		SSP_Set_ReceiveDataNum(STAR_SSP1, rx_size);
		#endif
		
		SPI_Send_Short(0x00); //写16位地址
		SPI_Send_Byte(VDM | RWB_READ | (s * 0x20 + 0x18)); //写控制字节,N个字节数据长度,读数据,选择端口s的寄存器
		
		#if W5500_HAL
		SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
		SSP_Wait_SendFinish(STAR_SSP1);//传输结束
		#endif
		
		SPI_Read_NByte(rx_size,dat_ptr);
	}
	W5500_CS(1);
	offset1 += rx_size; //更新实际物理地址,即下次读取接收到的数据的起始地址
	Write_W5500_SOCK_2Byte(s, Sn_RX_RD, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, RECV); //发送启动接收命令
	return rx_size; //返回接收到数据的长度
}

/*******************************************************************************
 * 函数名  : Write_SOCK_Data_Buffer
 * 描述    : 将数据写入W5500的数据发送缓冲区
 * 输入    : s:端口号,*dat_ptr:数据保存缓冲区指针,size:待写入数据的长度
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void Write_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr,
		unsigned short size) {
	unsigned short offset, offset1;
	unsigned short i;
	unsigned int cnt = size;

	//如果是UDP模式,可以在此设置目的主机的IP和端口号
	if ((Read_W5500_SOCK_1Byte(s, Sn_MR) & 0x0f) != SOCK_UDP) //如果Socket打开失败
	{
		Write_W5500_SOCK_4Byte(s, Sn_DIPR, UDP_DIPR); //设置目的主机IP
		Write_W5500_SOCK_2Byte(s, Sn_DPORTR, UDP_DPORT[0] * 256 + UDP_DPORT[1]); //设置目的主机端口号
	}

	offset = Read_W5500_SOCK_2Byte(s, Sn_TX_WR);
	offset1 = offset;
	offset &= (S_TX_SIZE - 1); //计算实际的物理地址
	
	W5500_CS(0);
	SPI_Send_Short(offset); //写16位地址
	SPI_Send_Byte(VDM | RWB_WRITE | (s * 0x20 + 0x10)); //写控制字节,N个字节数据长度,写数据,选择端口s的寄存器

	#if W5500_HAL
	//结束本轮
	SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
	SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	
	if ((offset + size) < S_TX_SIZE) //如果最大地址未超过W5500发送缓冲区寄存器的最大地址
	{
		while(cnt) {
			if(cnt <= 8) {
				for (i = 0; i < cnt; i++) //循环写入cnt个字节数据
				{
					SPI_Send_Byte(*dat_ptr++); //写入一个字节的数据
				}
				SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
				SSP_Wait_SendFinish(STAR_SSP1);//传输结束
				cnt = 0;
			} else {
				for (i = 0; i < 8; i++) //循环写入cnt个字节数据
				{
					SPI_Send_Byte(*dat_ptr++); //写入一个字节的数据
				}
				SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
				SSP_Wait_SendFinish(STAR_SSP1);//传输结束
				cnt -= 8;
			}
		}
			
	} else //如果最大地址超过W5500发送缓冲区寄存器的最大地址
	{
		offset = S_TX_SIZE - offset;
		while(cnt) {
			if(cnt <= 8) {
				for (i = 0; i < cnt; i++) //循环写入cnt个字节数据
				{
					SPI_Send_Byte(*dat_ptr++); //写入一个字节的数据
				}
				
				SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
				SSP_Wait_SendFinish(STAR_SSP1);//传输结束
				
				cnt = 0;
			} else {
				for (i = 0; i < 8; i++) //循环写入cnt个字节数据
				{
					SPI_Send_Byte(*dat_ptr++); //写入一个字节的数据
				}
				SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
				SSP_Wait_SendFinish(STAR_SSP1);//传输结束
				
				cnt -= 8;
			}
		}
		W5500_CS(1);
		W5500_CS(0);
		
		SPI_Send_Short(0x00); //写16位地址
		SPI_Send_Byte(VDM | RWB_WRITE | (s * 0x20 + 0x10)); //写控制字节,N个字节数据长度,写数据,选择端口s的寄存器

		while(cnt) {
		if(cnt <= 8) {
			for (i = 0; i < cnt; i++) //循环写入cnt个字节数据
			{
				SPI_Send_Byte(*dat_ptr++); //写入一个字节的数据
			}
			SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
			SSP_Wait_SendFinish(STAR_SSP1);//传输结束
			cnt = 0;
		} else {
			for (i = 0; i < 8; i++) //循环写入cnt个字节数据
			{
				SPI_Send_Byte(*dat_ptr++); //写入一个字节的数据
			}
			SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
			SSP_Wait_SendFinish(STAR_SSP1);//传输结束
			cnt -= 8;
		}
	}
		SSP_Enable(STAR_SSP1,ENABLE);//SPI使能
		SSP_Wait_SendFinish(STAR_SSP1);//传输结束
	}
	#endif
	
	#if W5500_IO
	if ((offset + size) < S_TX_SIZE) //如果最大地址未超过W5500发送缓冲区寄存器的最大地址
	{
		for (i = 0; i < size; i++) //循环写入size个字节数据
				{
			SPI_Send_Byte(*dat_ptr++); //写入一个字节的数据
		}
	} else //如果最大地址超过W5500发送缓冲区寄存器的最大地址
	{
		offset = S_TX_SIZE - offset;
		for (i = 0; i < offset; i++) //循环写入前offset个字节数据
				{
			SPI_Send_Byte(*dat_ptr++); //写入一个字节的数据
		}
    //置W5500的SCS为高电平
			W5500_CS(1);
   //置W5500的SCS为低电平
			W5500_CS(0);

		SPI_Send_Short(0x00); //写16位地址
		SPI_Send_Byte(VDM | RWB_WRITE | (s * 0x20 + 0x10)); //写控制字节,N个字节数据长度,写数据,选择端口s的寄存器

		for (; i < size; i++) //循环写入size-offset个字节数据
				{
			SPI_Send_Byte(*dat_ptr++); //写入一个字节的数据
		}
	}
	#endif
	
	W5500_CS(1);
	offset1 += size; //更新实际物理地址,即下次写待发送数据到发送数据缓冲区的起始地址
	Write_W5500_SOCK_2Byte(s, Sn_TX_WR, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, SEND); //发送启动发送命令
}

/*******************************************************************************
 * 函数名  : W5500_Hardware_Reset
 * 描述    : 硬件复位W5500
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 无
 * 说明    : W5500的复位引脚保持低电平至少500us以上,才能重围W5500
 *******************************************************************************/
void W5500_Hardware_Reset(void) {
//	W5500_RST=0;//复位引脚拉低
	GPIO_WriteBit(STAR_GPIO0,W5500_RST,0);
	Delay(200);
//	W5500_RST=1;//复位引脚拉高
	GPIO_WriteBit(STAR_GPIO0,W5500_RST,1);
	Delay(200);
	while ((Read_W5500_1Byte(PHYCFGR) & LINK) == 0)
		; //等待以太网连接完成
}

/*******************************************************************************
 * 函数名  : W5500_Init
 * 描述    : 初始化W5500寄存器函数
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 在使用W5500之前，先对W5500初始化
 *******************************************************************************/
void W5500_Init(void) {
	unsigned char i = 0;
	Write_W5500_1Byte(MR, RST); //软件复位W5500,置1有效,复位后自动清0
	Delay(10); //延时10ms,自己定义该函数

	//设置网关(Gateway)的IP地址,Gateway_IP为4字节unsigned char数组,自己定义 
	//使用网关可以使通信突破子网的局限，通过网关可以访问到其它子网或进入Internet
	Write_W5500_nByte(GAR, Gateway_IP, 4);

	//设置子网掩码(MASK)值,SUB_MASK为4字节unsigned char数组,自己定义
	//子网掩码用于子网运算
	Write_W5500_nByte(SUBR, Sub_Mask, 4);

	//设置物理地址,PHY_ADDR为6字节unsigned char数组,自己定义,用于唯一标识网络设备的物理地址值
	//该地址值需要到IEEE申请，按照OUI的规定，前3个字节为厂商代码，后三个字节为产品序号
	//如果自己定义物理地址，注意第一个字节必须为偶数
	Write_W5500_nByte(SHAR, Phy_Addr, 6);

	//设置本机的IP地址,IP_ADDR为4字节unsigned char数组,自己定义
	//注意，网关IP必须与本机IP属于同一个子网，否则本机将无法找到网关
	Write_W5500_nByte(SIPR, IP_Addr, 4);

	//设置发送缓冲区和接收缓冲区的大小，参考W5500数据手册
	for (i = 0; i < 8; i++) {
		Write_W5500_SOCK_1Byte(i, Sn_RXBUF_SIZE, 0x02); //Socket Rx memory size=2k
		Write_W5500_SOCK_1Byte(i, Sn_TXBUF_SIZE, 0x02); //Socket Tx mempry size=2k
	}

	//设置重试时间，默认为2000(200ms) 
	//每一单位数值为100微秒,初始化时值设为2000(0x07D0),等于200毫秒
	Write_W5500_2Byte(RTR, 0x07d0);

	//设置重试次数，默认为8次 
	//如果重发的次数超过设定值,则产生超时中断(相关的端口中断寄存器中的Sn_IR 超时位(TIMEOUT)置“1”)
	Write_W5500_1Byte(RCR, 1);
}

/*******************************************************************************
 * 函数名  : Detect_Gateway
 * 描述    : 检查网关服务器
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
 * 说明    : 无
 *******************************************************************************/
unsigned char Detect_Gateway(void) {
	unsigned char ip_adde[4];
	ip_adde[0] = IP_Addr[0] + 1;
	ip_adde[1] = IP_Addr[1] + 1;
	ip_adde[2] = IP_Addr[2] + 1;
	ip_adde[3] = IP_Addr[3] + 1;

	//检查网关及获取网关的物理地址
	Write_W5500_SOCK_4Byte(0, Sn_DIPR, ip_adde); //向目的地址寄存器写入与本机IP不同的IP值
	Write_W5500_SOCK_1Byte(0, Sn_MR, MR_TCP); //设置socket为TCP模式
	Write_W5500_SOCK_1Byte(0, Sn_CR, OPEN); //打开Socket
	Delay(5); //延时5ms

	if (Read_W5500_SOCK_1Byte(0, Sn_SR) != SOCK_INIT) //如果socket打开失败
	{
		Write_W5500_SOCK_1Byte(0, Sn_CR, CLOSE); //打开不成功,关闭Socket
		return FALSE; //返回FALSE(0x00)
	}

	Write_W5500_SOCK_1Byte(0, Sn_CR, CONNECT); //设置Socket为Connect模式

	do {
		unsigned char j = 0;
		j = Read_W5500_SOCK_1Byte(0, Sn_IR); //读取Socket0中断标志寄存器
		if (j != 0)
			Write_W5500_SOCK_1Byte(0, Sn_IR, j);
		Delay(5); //延时5ms
		if ((j & IR_TIMEOUT) == IR_TIMEOUT) {
			return FALSE;
		} else if (Read_W5500_SOCK_1Byte(0, Sn_DHAR) != 0xff) {
			Write_W5500_SOCK_1Byte(0, Sn_CR, CLOSE); //关闭Socket
			return TRUE;
		}
	} while (1);
	return 0;
}

/*******************************************************************************
 * 函数名  : Socket_Init
 * 描述    : 指定Socket(0~7)初始化
 * 输入    : s:待初始化的端口
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void Socket_Init(SOCKET s) {
	//设置分片长度，参考W5500数据手册，该值可以不修改	
	Write_W5500_SOCK_2Byte(0, Sn_MSSR, 1460); //最大分片字节数=1460
	//设置指定端口
	switch (s) {
	case 0:
		//设置端口0的端口号
		Write_W5500_SOCK_2Byte(0, Sn_PORT, S0_Port[0] * 256 + S0_Port[1]);
		//设置端口0目的(远程)端口号
		Write_W5500_SOCK_2Byte(0, Sn_DPORTR, S0_DPort[0] * 256 + S0_DPort[1]);
		//设置端口0目的(远程)IP地址
		Write_W5500_SOCK_4Byte(0, Sn_DIPR, S0_DIP);

		break;

	case 1:
		break;

	case 2:
		break;

	case 3:
		break;

	case 4:
		break;

	case 5:
		break;

	case 6:
		break;

	case 7:
		break;

	default:
		break;
	}
}

/*******************************************************************************
 * 函数名  : Socket_Connect
 * 描述    : 设置指定Socket(0~7)为客户端与远程服务器连接
 * 输入    : s:待设定的端口
 * 输出    : 无
 * 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
 * 说明    : 当本机Socket工作在客户端模式时,引用该程序,与远程服务器建立连接
 *			如果启动连接后出现超时中断，则与服务器连接失败,需要重新调用该程序连接
 *			该程序每调用一次,就与服务器产生一次连接
 *******************************************************************************/
unsigned char Socket_Connect(SOCKET s) {
	Write_W5500_SOCK_1Byte(s, Sn_MR, MR_TCP); //设置socket为TCP模式
	Write_W5500_SOCK_1Byte(s, Sn_CR, OPEN); //打开Socket
	Delay(5); //延时5ms
	if (Read_W5500_SOCK_1Byte(s, Sn_SR) != SOCK_INIT) //如果socket打开失败
	{
		Write_W5500_SOCK_1Byte(s, Sn_CR, CLOSE); //打开不成功,关闭Socket
		return FALSE; //返回FALSE(0x00)
	}
	Write_W5500_SOCK_1Byte(s, Sn_CR, CONNECT); //设置Socket为Connect模式
	return TRUE; //返回TRUE,设置成功
}

/*******************************************************************************
 * 函数名  : Socket_Listen
 * 描述    : 设置指定Socket(0~7)作为服务器等待远程主机的连接
 * 输入    : s:待设定的端口
 * 输出    : 无
 * 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
 * 说明    : 当本机Socket工作在服务器模式时,引用该程序,等等远程主机的连接
 *			该程序只调用一次,就使W5500设置为服务器模式
 *******************************************************************************/
unsigned char Socket_Listen(SOCKET s) {
	Write_W5500_SOCK_1Byte(s, Sn_MR, MR_TCP); //设置socket为TCP模式
	Write_W5500_SOCK_1Byte(s, Sn_CR, OPEN); //打开Socket
	Delay(5); //延时5ms
	if (Read_W5500_SOCK_1Byte(s, Sn_SR) != SOCK_INIT) //如果socket打开失败
	{
		Write_W5500_SOCK_1Byte(s, Sn_CR, CLOSE); //打开不成功,关闭Socket
		return FALSE; //返回FALSE(0x00)
	}
	Write_W5500_SOCK_1Byte(s, Sn_CR, LISTEN); //设置Socket为侦听模式
	Delay(5); //延时5ms
	if (Read_W5500_SOCK_1Byte(s, Sn_SR) != SOCK_LISTEN) //如果socket设置失败
	{
		Write_W5500_SOCK_1Byte(s, Sn_CR, CLOSE); //设置不成功,关闭Socket
		return FALSE; //返回FALSE(0x00)
	}

	return TRUE;

	//至此完成了Socket的打开和设置侦听工作,至于远程客户端是否与它建立连接,则需要等待Socket中断，
	//以判断Socket的连接是否成功。参考W5500数据手册的Socket中断状态
	//在服务器侦听模式不需要设置目的IP和目的端口号
}

/*******************************************************************************
 * 函数名  : Socket_UDP
 * 描述    : 设置指定Socket(0~7)为UDP模式
 * 输入    : s:待设定的端口
 * 输出    : 无
 * 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
 * 说明    : 如果Socket工作在UDP模式,引用该程序,在UDP模式下,Socket通信不需要建立连接
 *			该程序只调用一次，就使W5500设置为UDP模式
 *******************************************************************************/
unsigned char Socket_UDP(SOCKET s) {
	Write_W5500_SOCK_1Byte(s, Sn_MR, MR_UDP); //设置Socket为UDP模式*/
	Write_W5500_SOCK_1Byte(s, Sn_CR, OPEN); //打开Socket*/
	Delay(5); //延时5ms
	if (Read_W5500_SOCK_1Byte(s, Sn_SR) != SOCK_UDP) //如果Socket打开失败
	{
		Write_W5500_SOCK_1Byte(s, Sn_CR, CLOSE); //打开不成功,关闭Socket
		return FALSE; //返回FALSE(0x00)
	} else
		return TRUE;

	//至此完成了Socket的打开和UDP模式设置,在这种模式下它不需要与远程主机建立连接
	//因为Socket不需要建立连接,所以在发送数据前都可以设置目的主机IP和目的Socket的端口号
	//如果目的主机IP和目的Socket的端口号是固定的,在运行过程中没有改变,那么也可以在这里设置
}

/*******************************************************************************
 * 函数名  : W5500_Interrupt_Process
 * 描述    : W5500中断处理程序框架
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void W5500_Interrupt_Process(void) {
	unsigned char i, j;

	IntDispose:

	i = Read_W5500_1Byte(SIR); //读取端口中断标志寄存器
	if ((i & S0_INT) == S0_INT) //Socket0事件处理
	{
		j = Read_W5500_SOCK_1Byte(0, Sn_IR); //读取Socket0中断标志寄存器
		Write_W5500_SOCK_1Byte(0, Sn_IR, j);
		if (j & IR_CON) //在TCP模式下,Socket0成功连接
		{
			S0_State |= S_CONN; //网络连接状态0x02,端口完成连接，可以正常传输数据
		}
		if (j & IR_DISCON) //在TCP模式下Socket断开连接处理
		{
			Write_W5500_SOCK_1Byte(0, Sn_CR, CLOSE); //关闭端口,等待重新打开连接
			Socket_Init(0); //指定Socket(0~7)初始化,初始化端口0
			S0_State = 0; //网络连接状态0x00,端口连接失败
		}
		if (j & IR_SEND_OK) //Socket0数据发送完成,可以再次启动Write_SOCK_Data_Buffer()函数发送数据
		{
			S0_Data |= S_TRANSMITOK; //端口发送一个数据包完成
		}
		if (j & IR_RECV) //Socket接收到数据,可以启动Read_SOCK_Data_Buffer()函数
		{
			S0_Data |= S_RECEIVE; //端口接收到一个数据包
		}
		if (j & IR_TIMEOUT) //Socket连接或数据传输超时处理
		{
			Write_W5500_SOCK_1Byte(0, Sn_CR, CLOSE); // 关闭端口,等待重新打开连接
			S0_State = 0; //网络连接状态0x00,端口连接失败
		}
	}

	if (Read_W5500_1Byte(SIR) != 0)
		goto IntDispose;
}


/*******************************************************************************
 * 函数名  : W5500_Initialization
 * 描述    : W5500初始化配置
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void W5500_Initialization(void) {
	W5500_Init(); //初始化W5500寄存器函数
	Detect_Gateway(); //检查网关服务器
	Socket_Init(0); //指定Socket(0~7)初始化,初始化端口0
}

/*******************************************************************************
 * 函数名  : Load_Net_Parameters
 * 描述    : 装载网络参数
 * 输入    : 接口体变量_Typedef_Internet
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 网关、掩码、物理地址、本机IP地址、端口号、目的IP地址、目的端口号、端口工作模式
 *******************************************************************************/
void Load_Net_Parameters(_Typedef_Internet * Intnet_set) {
	//加载端口0的工作模式
	S0_Mode = Intnet_set->s0_ModeSet;

	Gateway_IP[0] = Intnet_set->Gateway_IP0; //加载设备网关参数
	Gateway_IP[1] = Intnet_set->Gateway_IP1;
	Gateway_IP[2] = Intnet_set->Gateway_IP2;
	Gateway_IP[3] = Intnet_set->Gateway_IP3;

	Sub_Mask[0] = Intnet_set->Sub_Mask0; //加载设备子网掩码
	Sub_Mask[1] = Intnet_set->Sub_Mask1;
	Sub_Mask[2] = Intnet_set->Sub_Mask2;
	Sub_Mask[3] = Intnet_set->Sub_Mask3;

	Phy_Addr[0] = Intnet_set->Phy_Addr0; //加载设备物理地址（mac）
	Phy_Addr[1] = Intnet_set->Phy_Addr1;
	Phy_Addr[2] = Intnet_set->Phy_Addr2;
	Phy_Addr[3] = Intnet_set->Phy_Addr3;
	Phy_Addr[4] = Intnet_set->Phy_Addr4;
	Phy_Addr[5] = Intnet_set->Phy_Addr5;

	IP_Addr[0] = Intnet_set->IP_Addr0; //加载设备IP地址
	IP_Addr[1] = Intnet_set->IP_Addr1;
	IP_Addr[2] = Intnet_set->IP_Addr2;
	IP_Addr[3] = Intnet_set->IP_Addr3;
	//加载设备端口0（socket 0）的端口号5000
	S0_Port[0] = Intnet_set->S0_Port0;
	S0_Port[1] = Intnet_set->S0_Port1;

	//设定的是客户端模式
	if(S0_Mode == TCP_CLIENT)
	{
		//加载端口0的目的（如调试用的pc机）IP地址
		S0_DIP[0] = Intnet_set->S0_DIP0;
		S0_DIP[1] = Intnet_set->S0_DIP1;
		S0_DIP[2] = Intnet_set->S0_DIP2;
		S0_DIP[3] = Intnet_set->S0_DIP3;

	//加载端口0的目的（如调试用的pc机）端口号6000
		S0_DPort[0] = Intnet_set->S0_DPort0;
		S0_DPort[1] = Intnet_set->S0_DPort1;
	}

	//设定的是UDP模式,初始化给一个目的IP和端口号。
	if(S0_Mode == UDP_MODE)
	{
	//加载端口0的目的（如调试用的pc机）IP地址
		UDP_DIPR[0] = Intnet_set->UDP_DIPR0;
		UDP_DIPR[1] = Intnet_set->UDP_DIPR1;
		UDP_DIPR[2] = Intnet_set->UDP_DIPR2;
		UDP_DIPR[3] = Intnet_set->UDP_DIPR3;
	//加载端口0的目的（如调试用的pc机）端口号6000
		UDP_DPORT[0] = Intnet_set->UDP_DPORT0;
		UDP_DPORT[1] = Intnet_set->UDP_DPORT1;
	}
}

/*******************************************************************************
 * 函数名  : W5500_Socket_Set
 * 描述    : W5500端口初始化配置
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 分别设置4个端口,根据端口工作模式,将端口置于TCP服务器、TCP客户端或UDP模式.
 *			从端口状态字节Socket_State可以判断端口的工作情况
 *******************************************************************************/
void W5500_Socket_Set(void) {
	if (S0_State == 0) //端口0初始化配置
			{
		if (S0_Mode == TCP_SERVER) //TCP服务器模式
		{
			if (Socket_Listen(0) == TRUE)
				S0_State = S_INIT;
			else
				S0_State = 0;
		} else if (S0_Mode == TCP_CLIENT) //TCP客户端模式
		{
			if (Socket_Connect(0) == TRUE)
				S0_State = S_INIT;
			else
				S0_State = 0;
		} else //UDP模式
		{
			if (Socket_UDP(0) == TRUE)
				S0_State = S_INIT | S_CONN;
			else
				S0_State = 0;
		}
	}
}

/*******************************************************************************
 * 函数名  : Process_Socket_Data
 * 描述    : W5500接收并发送接收到的数据
 * 输入    : s:端口号
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 本过程先调用Read_SOCK_Data_Buffer()从W5500的端口接收数据缓冲区读取数据,
 *			然后将读取的数据从Rx_Buffer拷贝到Tx_Buffer缓冲区进行处理。
 *			处理完毕，调用Write_SOCK_Data_Buffer()发送数据。
 *******************************************************************************/
void Process_Socket_Data(SOCKET s) {
	unsigned short size;
	size = Read_SOCK_Data_Buffer(s, Rx_Buffer);
	//如果是UDP模式，从前8个字节中提取出远方IP和端口号
	if(S0_Mode == UDP_MODE)
	{
		UDP_DIPR[0] = Rx_Buffer[0];
		UDP_DIPR[1] = Rx_Buffer[1];
		UDP_DIPR[2] = Rx_Buffer[2];
		UDP_DIPR[3] = Rx_Buffer[3];

		UDP_DPORT[0] = Rx_Buffer[4];
		UDP_DPORT[1] = Rx_Buffer[5];
//有效数据长度为数据包总长度减去8，（数据包前8个字节为远方IP和端口号）
		size = size - 8;	
		memcpy(Tx_Buffer, Rx_Buffer+8, size);
	}
	else
		memcpy(Tx_Buffer, Rx_Buffer, size);

	//回传接受到的数据
	Write_SOCK_Data_Buffer(s, Tx_Buffer, size);
}
/*******************************************************************************
 * 函数名  : Delay
 * 描述    : 延时函数(ms)
 * 输入    : d:延时系数，单位为毫秒
 * 输出    : 无
 * 返回    : 无
 * 说明    :
 *******************************************************************************/
void Delay(unsigned int x) {
	unsigned int i, j;

	for (j = 0; j < 7200; j++)
		for (i = 0; i < x; i++)
			;
}
