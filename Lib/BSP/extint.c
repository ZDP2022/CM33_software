#include "STAR_gpio.h"
#include "STAR_rcc.h"
#include "STAR.h"
#include "extint.h"
#include "misc.h"
#include "uart.h"
#include <stdio.h>

//外部中断0初始化设置
//中断源为来自FPGA的SW8 STAR的外部中断EXTI只有高电平触发 不支持其他触发方式
void extint0_init(void)
{	
	NVIC_InitTypeDef nvic_init_t;
	
	NVIC_ClearPendingIRQ(EXT0_IRQn);                   //clear NVIC pending interrupts
	
	//中断优先级设置
	nvic_init_t.NVIC_IRQChannel = EXT0_IRQn;
	nvic_init_t.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级
	nvic_init_t.NVIC_IRQChannelSubPriority = 0;	//响应优先级
	nvic_init_t.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init_t);
	
	NVIC_EnableIRQ(EXT0_IRQn);
}

void extint15_init(void)
{	
	NVIC_InitTypeDef nvic_init_t;
	
	NVIC_ClearPendingIRQ(EXT15_IRQn);                   //clear NVIC pending interrupts
	
	//中断优先级设置
	nvic_init_t.NVIC_IRQChannel = EXT15_IRQn;
	nvic_init_t.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级
	nvic_init_t.NVIC_IRQChannelSubPriority = 0;	//响应优先级
	nvic_init_t.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init_t);
	
	NVIC_EnableIRQ(EXT15_IRQn);
}

void EXT0_Handler(void)
{
	//点亮/熄灭LED D5
	GPIO_TogglePin(STAR_GPIO0, GPIO_Pin_0);
	printf("EXT0_Handler\r\n");
	//清除中断标志
	NVIC_ClearPendingIRQ(EXT0_IRQn);		
}

void EXT15_Handler(void)
{
	//点亮/熄灭LED D5
	GPIO_TogglePin(STAR_GPIO0, GPIO_Pin_0);
	printf("EXT15_Handler\r\n");
	//清除中断标志
	NVIC_ClearPendingIRQ(EXT15_IRQn);		
}

void extint_test(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	printf("STAR EXTINT test...\r\n");
	extint0_init();
}

