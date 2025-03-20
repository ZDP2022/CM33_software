#include "STAR_timer.h"
#include "STAR_rcc.h"
#include "timer.h"
#include "STAR.h"
#include "STAR_gpio.h"

//LED0-GPIO0_0
#define LED0_PORT	STAR_GPIO0
#define LED0_PIN	GPIO_Pin_0

//LED1-GPIO0_1
#define LED1_PORT	STAR_GPIO0
#define LED1_PIN	GPIO_Pin_1

//LED开启控制
#define LED0_ON		GPIO_ResetBit(LED0_PORT,LED0_PIN)
#define LED1_ON		GPIO_ResetBit(LED1_PORT,LED1_PIN)

//LED关闭控制
#define LED0_OFF	GPIO_SetBit(LED0_PORT,LED0_PIN)
#define LED1_OFF 	GPIO_SetBit(LED1_PORT,LED1_PIN)



//定时器初始化
static void timerx_init(TIMER_TypeDef* TIMERx)
{		
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	TIM_TimeBaseInitStruct.TIM_Reload = 25000000; //定时时间设为1秒 定时器时钟PCLK=25M，定时1秒 定时器装载值为25000000
	TIM_TimeBaseInitStruct.TIM_Value  = 25000000; 
	TIM_TimeBaseInit(TIMERx, &TIM_TimeBaseInitStruct);
	//使能定时器
	TIM_CtrlCmd(TIMERx,TIM_ENABLE,ENABLE);
	
	//使能定时器中断
	TIM_CtrlCmd(TIMERx,TIM_INTERRUPT_ENABLE,ENABLE);
	if(TIMERx ==STAR_TIMER0)
	{
		NVIC_ClearPendingIRQ(TIMER0_IRQn);
		NVIC_EnableIRQ(TIMER0_IRQn);
	}
	else
	{
		NVIC_ClearPendingIRQ(TIMER1_IRQn);
		NVIC_EnableIRQ(TIMER1_IRQn);
	}
}

void timer_init(void)
{
	TIMER_TypeDef* TIMERx;
	
	TIMERx = STAR_TIMER0;
	timerx_init(TIMERx);
}

//定时器0中断处理函数
void TIMER0_Handler(void)
{
	GPIO_TogglePin(LED0_PORT,LED0_PIN);
	//清除中断
	TIM_ClearIT(STAR_TIMER0);
}

//定时器1中断处理函数
void TIMER1_Handler(void)
{
	GPIO_TogglePin(LED1_PORT,LED1_PIN);
	//清除中断
	TIM_ClearIT(STAR_TIMER1);
}



