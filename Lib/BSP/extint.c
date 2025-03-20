#include "STAR_gpio.h"
#include "STAR_rcc.h"
#include "STAR.h"
#include "extint.h"
#include "misc.h"
#include "uart.h"
#include <stdio.h>

//�ⲿ�ж�0��ʼ������
//�ж�ԴΪ����FPGA��SW8 STAR���ⲿ�ж�EXTIֻ�иߵ�ƽ���� ��֧������������ʽ
void extint0_init(void)
{	
	NVIC_InitTypeDef nvic_init_t;
	
	NVIC_ClearPendingIRQ(EXT0_IRQn);                   //clear NVIC pending interrupts
	
	//�ж����ȼ�����
	nvic_init_t.NVIC_IRQChannel = EXT0_IRQn;
	nvic_init_t.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�
	nvic_init_t.NVIC_IRQChannelSubPriority = 0;	//��Ӧ���ȼ�
	nvic_init_t.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init_t);
	
	NVIC_EnableIRQ(EXT0_IRQn);
}

void extint15_init(void)
{	
	NVIC_InitTypeDef nvic_init_t;
	
	NVIC_ClearPendingIRQ(EXT15_IRQn);                   //clear NVIC pending interrupts
	
	//�ж����ȼ�����
	nvic_init_t.NVIC_IRQChannel = EXT15_IRQn;
	nvic_init_t.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�
	nvic_init_t.NVIC_IRQChannelSubPriority = 0;	//��Ӧ���ȼ�
	nvic_init_t.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init_t);
	
	NVIC_EnableIRQ(EXT15_IRQn);
}

void EXT0_Handler(void)
{
	//����/Ϩ��LED D5
	GPIO_TogglePin(STAR_GPIO0, GPIO_Pin_0);
	printf("EXT0_Handler\r\n");
	//����жϱ�־
	NVIC_ClearPendingIRQ(EXT0_IRQn);		
}

void EXT15_Handler(void)
{
	//����/Ϩ��LED D5
	GPIO_TogglePin(STAR_GPIO0, GPIO_Pin_0);
	printf("EXT15_Handler\r\n");
	//����жϱ�־
	NVIC_ClearPendingIRQ(EXT15_IRQn);		
}

void extint_test(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	printf("STAR EXTINT test...\r\n");
	extint0_init();
}

