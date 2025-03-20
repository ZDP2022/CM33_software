#include <stdio.h>
#include <inttypes.h>
#include "STAR.h"
#include "STAR_gpio.h"
#include "uart.h"
#include "GNSS.h"



uint32_t GNSS_Data[GNSS_DATA_LENGTH] = {0}; // 从RAM读出的GNSS的数据

void delay_ms(uint16_t dly)
{
	uint32_t i,j;
	for(i=0;i<dly;i++)
	{
		for(j=0;j<4000;j++);
	}
}

int main(void)
{	
	GPIO_DeInit(STAR_GPIO0);

	uart_init(115200);
	
	//Get_GNSS_TIME(GNSS_Data);

			printf("abcde");
				printf("hello");

	while(1)
	{

//delay_ms(1000);
	Get_GNSS_TIME(GNSS_Data);
	//	printf("当前时间\r\n %4d%4d%4d%4d 年 %4d 月 %4d 日 %2d%4d ：%4d%4d ：%4d%4d \r\n 毫秒：%10d ,微秒：%10d ,纳秒 %10d\r\n",
	//	GNSS_Data[0]>>28,GNSS_Data[0]>>24,GNSS_Data[0]>>20,GNSS_Data[0]>>16,GNSS_Data[0]>>12,GNSS_Data[0]>>8,GNSS_Data[0]>>4,GNSS_Data[0],
	//	GNSS_Data[1]>>30,GNSS_Data[1]>>26,GNSS_Data[1]>>22,GNSS_Data[1]>>18,GNSS_Data[1]>>14,GNSS_Data[1]>>10,GNSS_Data[1],
	//	GNSS_Data[2]>>10,GNSS_Data[2]);
	}
}
