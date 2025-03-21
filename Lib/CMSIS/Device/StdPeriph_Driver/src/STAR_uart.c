#include "STAR_uart.h"
#include "STAR_rcc.h"


/**
  *
  * @brief: 将UART寄存器值初始化为默认配置。
  *
  * @param: UARTx,使用哪组UART;STAR有3组UART:STAR_UART0、STAR_UART1、STAR_UART2。
  * 
  * @retVal: void
  */
void UART_DeInit(UART_TypeDef *UARTx)
{
	assert_param(IS_UART_ALL_PERIPH(UARTx));

	if(UARTx ==STAR_UART0)
	{
		//APB_PERI_RSTCTL->RESETPERI &= (~PERI_RSTCTL_UART0_Msk);
		RCC_APB1PeriphResetCmd( RCC_APB1Periph_UART0, ENABLE );
		RCC_APB1PeriphResetCmd( RCC_APB1Periph_UART0, DISABLE);
	}
	else if(UARTx == STAR_UART1)
	{
		//APB_PERI_RSTCTL->RESETPERI &= (~PERI_RSTCTL_UART1_Msk);
		RCC_APB1PeriphResetCmd( RCC_APB1Periph_UART1, ENABLE );
		RCC_APB1PeriphResetCmd( RCC_APB1Periph_UART1, DISABLE);
	}
	else if(UARTx == STAR_UART2)
	{
		//APB_PERI_RSTCTL->RESETPERI &= (~PERI_RSTCTL_UART1_Msk);
		RCC_APB1PeriphResetCmd( RCC_APB1Periph_UART2, ENABLE );
		RCC_APB1PeriphResetCmd( RCC_APB1Periph_UART2, DISABLE);
	}
}
	
/**
  *
  * @brief: UART初始化设置。
  *
  * @param: UARTx,使用哪组UART;STAR有3组UART:STAR_UART0、STAR_UART1、STAR_UART2。
  * @param: UART_InitStruct,UART设置参数结构体变量，该结构体的成员变量包括串口波特率、串口工作方式。
  *
  * @retVal: void
  */
void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_InitStruct )
{
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_USART_CTRL(UART_InitStruct->UART_CTRL));

    UARTx->BAUDDIV = PCLK / (UART_InitStruct->UART_BundRate)*4;
    UARTx->CTRL 	= UART_InitStruct->UART_CTRL;
}

/**
  *
  * @brief: UART中断设置。
  *
  * @param: UARTx,使用哪组UART;STAR有3组UART:STAR_UART0、STAR_UART1、STAR_UART2。
  * @param: UART_IT,UART中断类型；可设置的值包括:
  *							UART_TxInterrupt(0x04) - UART发送中断；
  *							UART_RxInterrupt(0x08) - UART接收中断；
  *							UART_TxInterruptOverrun(0x10) - UART发送buffer溢出中断；
  *							UART_RxInterruptOverrun(0x20) - UART接收buffer溢出中断。
  * @param: NewState,是否使能UART中断: ENABLE - 使能中断；DISABLE - 不使能中断。
  * @retVal: void
  */
void UART_ITConfig(UART_TypeDef *UARTx, uint16_t UART_IT, FunctionalState NewState)
{
    
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_CONFIG_IT(UART_IT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
		UARTx->CTRL |= UART_IT;
    }
    else
    {
		UARTx->CTRL &= ~UART_IT;
    }
}

/**
  *
  * @brief: 清除UART中断。
  *
  * @param: UARTx,使用哪组UART;STAR有3组UART:STAR_UART0、STAR_UART1、STAR_UART2。
  * @param: USART_INTCLEAR,要清除的串口中断类型；可设置的值包括:
  *							UART_INTCLEAR_TxInterrupt(0x01) - UART发送中断清除；
  *							UART_INTCLEAR_RxInterrupt(0x02) - UART接收中断清除；
  *							UART_INTCLEAR_TxInterruptOverrun(0x04) - UART发送buffer溢出中断清除；
  *							UART_INTCLEAR_RxInterruptOverrun(0x08) - UART接收buffer溢出中断清除。
  *
  * @retVal: void
  */
void UART_ITCLEAR(UART_TypeDef *UARTx, uint16_t USART_INTCLEAR)
{
    assert_param(IS_UART_ALL_PERIPH(UARTx));
	assert_param(IS_USART_INTCLEAR(INTCLEAR));
	
	UARTx->INTCLEAR |= USART_INTCLEAR;		//注意!!!只要INTCLEAR被赋值，会将INTCLEAR整个清零
}

/**
  *
  * @brief: 获取UART中断状态。
  *
  * @param: UARTx,使用哪组UART;STAR有3组UART:STAR_UART0、STAR_UART1、STAR_UART2。
  * @param: UART_IT,要获取中断状态的中断类型；可设置的值包括:
  *							UART_TxInterrupt(0x04) - UART发送中断；
  *							UART_RxInterrupt(0x08) - UART接收中断；
  *							UART_TxInterruptOverrun(0x10) - UART发送buffer溢出中断；
  *							UART_RxInterruptOverrun(0x20) - UART接收buffer溢出中断。
  *
  * @retVal: bitstatus,获取到的中断状态；
  *						SET - 产生了UART_IT指定中断类型的中断；RESET - 未产生UART_IT指定中断类型的中断。
  */
FlagStatus UART_GetITStatus(UART_TypeDef* UARTx, uint16_t UART_IT)
{
    FlagStatus bitstatus = RESET;
    
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_GET_IT(UART_IT));
    
	if((UARTx->INTSTATUS &  (UART_IT>>2) ) != RESET )
    {
      bitstatus = SET;
    }
    else
    {
      bitstatus = RESET;
    }
    return bitstatus;
}

/**
  *
  * @brief: 获取UART接收buffer和发送buffer状态。
  *
  * @param: UARTx,使用哪组UART;STAR有3组UART:STAR_UART0、STAR_UART1、STAR_UART2。
  * @param: UART_FLAG,要获取buffer的状态类型；可设置的值包括:
  *							UART_TxBufferFull_FLAG(0x01) - UART发送buffer已满状态；
  *							UART_RxBufferFull_FLAG(0x02) - UART接收buffer已满状态；
  *							UART_TxBufferOverrun_FLAG(0x04) - UART发送buffer溢出状态；
  *							UART_RxBufferOverrun_FLAG(0x08) - UART接收buffer溢出状态。
  *
  * @retVal: bitstatus,获取到的buffer状态是否是查询的状态；如UART_FLAG为UART_TxBufferFull_FLAG；返回值为SET，
  *						则表明UART发送buffer已满；如果返回值为RESET，则表明UART发送buffer未满。
  */
FlagStatus UART_GetFlagStatus(UART_TypeDef *UARTx, uint16_t UART_FLAG)
{
    FlagStatus bitstatus = RESET;
    
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_FLAG(UART_FLAG));

    if ((UARTx->STATE & UART_FLAG) != RESET)
	{
		bitstatus = SET;
	}
	else
	{
		bitstatus = RESET;
	}
    return bitstatus;
}


/**
  *
  * @brief: UART发送数据。
  *
  * @param: UARTx,使用哪组UART;STAR有3组UART:STAR_UART0、STAR_UART1、STAR_UART2。
  * @param: Data,需要发送的数据。
  *
  * @retVal: void
  */
void UART_SendData(UART_TypeDef *UARTx, uint8_t Data)
{
	assert_param(IS_UART_ALL_PERIPH(UARTx));

	/*Transmit data*/
	// Wait if Transmit Holding register is full
	while ( (UARTx->STATE & STAR_UART_STATE_TXBF_Msk));
	UARTx->DATA = (Data & (uint8_t)0xff);
}

/**
  *
  * @brief: UART接收数据。
  *
  * @param: UARTx,使用哪组UART;STAR有3组UART:STAR_UART0、STAR_UART1、STAR_UART2。
  *
  * @retVal: read_value,UART接收到的数据。
  */
uint8_t  UART_ReceiveData(UART_TypeDef *UARTx)
{
	uint8_t read_value = 0x00;

	assert_param(IS_UART_ALL_PERIPH(UARTx));
	
	/* Receive Data */
	while((UARTx->STATE & STAR_UART_STATE_RXBF_Msk) == 0);
	read_value = (uint8_t) (UARTx->DATA & (uint8_t) 0xff);

	return  read_value;
}


