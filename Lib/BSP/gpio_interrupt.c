#include "STAR.h"
#include "STAR_gpio.h"
#include "STAR_rcc.h"


//GPIO4中断初始化
void gpio4_interrupt_init(void)
{	
	//GPIO4设置为低电平触发
	GPIO_PinInterruptTypeConfig(STAR_GPIO0,GPIO_Pin_4,Bit_LOWLEVEL,Interrupt_ENABLE );
	NVIC_ClearPendingIRQ(PORT0_4_IRQn);
	NVIC_EnableIRQ(PORT0_4_IRQn);
}

//GPIO4中断处理函数
void PORT0_4_Handler(void)
{
	//LED-D5 ON
	GPIO_ResetBit(STAR_GPIO0, GPIO_Pin_0);
	//清除中断
	GPIO_PinInterruptClearFlag(STAR_GPIO0,GPIO_Pin_4);	
	NVIC_ClearPendingIRQ(PORT0_4_IRQn);	
}





