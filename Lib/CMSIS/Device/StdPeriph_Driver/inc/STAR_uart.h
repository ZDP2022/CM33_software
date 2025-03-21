#ifndef __STAR_UART_H
#define __STAR_UART_H


#ifdef __cplusplus
 extern "C" {
#endif

/*------------include---------*/
#include "STAR.h"



/**
  * @defgroup USART_Exported_Constants
  * @{
  */ 
#define IS_UART_ALL_PERIPH(PERIPH) (((PERIPH) == STAR_UART0) ||((PERIPH) == STAR_UART1)||((PERIPH) == STAR_UART2))
/**
  * @}
  */ 

/** 
  * @brief  USART Init Structure definition  
  */ 

typedef struct
{
	uint32_t UART_BundRate;   /*!< This member configures the USART communication baud rate.*/
	
	uint8_t UART_CTRL;

	uint8_t UART_STATE;

	uint8_t UART_INTCLEAR;
	
} UART_InitTypeDef;




/**
  * @}
  */

/**
  * @ defgroup UART_STATE
  * @{
  */
#define UART_STATE_TxBufReadOnly               ((uint8_t)0x1)
#define UART_STATE_RxBufReadOnly               ((uint8_t)0x2)
#define UART_STATE_TxBufOverrunClear           ((uint8_t)0x4)
#define UART_STATE_BuBufOverrunClear           ((uint8_t)0x8)

#define IS_USART_STATE(STATE) (((STATE) == UART_STATE_TxBufReadOnly ) || \
					((STATE) ==UART_STATE_RxBufReadOnly) || \
					((STATE) ==  UART_STATE_TxBufOverrunClear) || \
					((STATE) ==  UART_STATE_BuBufOverrunClear ))
/**
  * @}
  */

/**
 * @defgroup UART_CTRL
 * @{
 */
#define UART_CTRL_TxEnable                     ((uint8_t)0x01)
#define UART_CTRL_RxEnable                     ((uint8_t)0x02)
#define UART_CTRL_TxInterruptEnable            ((uint8_t)0x04)
#define UART_CTRL_RxInterruptEnable            ((uint8_t)0x08)
#define UART_CTRL_TxOverrunInterruptEnable     ((uint8_t)0x10)
#define UART_CTRL_RxOverrunInterruptEnable     ((uint8_t)0x20)
#define UART_CTRL_HighspeedTestMode            ((uint8_t)0x40)

#define IS_USART_CTRL(CTRL) (((CTRL) == UART_CTRL_TxEnable) || \
								((CTRL) == UART_CTRL_TxEnable) || \
								((CTRL) ==  UART_CTRL_TxInterruptEnable) || \
								((CTRL) ==  UART_CTRL_RxInterruptEnable) || \
								((CTRL) == UART_CTRL_TxOverrunInterruptEnable ) || \
								((CTRL) == UART_CTRL_RxOverrunInterruptEnable ) || \
								((CTRL) == UART_CTRL_HighspeedTestMode ) )

/**
  * @}
  */

/**
 * @defgroup UART_INTCLEAR
 * @{
 */
#define UART_INTCLEAR_TxInterrupt               ((uint8_t)0x01)
#define UART_INTCLEAR_RxInterrupt               ((uint8_t)0x02)
#define UART_INTCLEAR_TxInterruptOverrun        ((uint8_t)0x04)
#define UART_INTCLEAR_RxInterruptOverrun        ((uint8_t)0x08)

#define IS_USART_INTCLEAR(INTCLEAR) (((INTCLEAR) == UART_INTCLEAR_TxInterrupt) || \
						((INTCLEAR) ==   UART_INTCLEAR_RxInterrupt  ) || \
						((INTCLEAR) ==  UART_INTCLEAR_TxInterruptOverrun) || \
						((INTCLEAR) ==  UART_INTCLEAR_RxInterruptOverrun))

/**
  * @}
  */

/** @defgroup USART_Interrupt_definition 
  * @{
  */

#define UART_TxInterrupt                 0x04	//(0x1UL<<2)
#define UART_RxInterrupt                 0x08	//(0x1UL<<3)
#define UART_TxInterruptOverrun    			 (0x1UL<<4)
#define UART_RxInterruptOverrun    		   (0x1UL<<5)

#define IS_UART_CONFIG_IT(IT) (((IT) == UART_TxInterrupt ) || ((IT) == UART_RxInterrupt) ||\
                                                           ((IT) == UART_TxInterruptOverrun ) || ((IT) == UART_RxInterruptOverrun))

#define IS_UART_GET_IT(IT) (((IT) == UART_TxInterrupt ) || ((IT) == UART_RxInterrupt) ||\
                                                ((IT) == UART_TxInterruptOverrun ) || ((IT) == UART_RxInterruptOverrun))
/**
  * @}
  */

/** @defgroup USART_Flags 
  * @{
  */

#define UART_TxBufferFull_FLAG            ((uint8_t)0x1)
#define UART_RxBufferFull_FLAG            ((uint8_t)0x2)
#define UART_TxBufferOverrun_FLAG     		((uint8_t)0x4)
#define UART_RxBufferOverrun_FLAG     		((uint8_t)0x8)
#define IS_UART_FLAG(FLAG) 	(    ((FLAG) == UART_TxBufferFull_FLAG)    || ((FLAG) == UART_RxBufferFull_FLAG) ||\
															(   (FLAG) == UART_TxBufferOverrun_FLAG) || ((FLAG) == UART_RxBufferOverrun_FLAG))

/**
  * @}
  */

/** 
  * @defgroup USART_Exported_Functions
  * @{
  */
void UART_DeInit(UART_TypeDef *UARTx);
void UART_StructInit(UART_InitTypeDef *UART_InitStruct);
void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_InitStruct);
void UART_ITConfig(UART_TypeDef *UARTx, uint16_t UART_IT, FunctionalState NewState);
FlagStatus UART_GetITStatus(UART_TypeDef *UARTx, uint16_t UART_IT);
FlagStatus UART_GetFlagStatus(UART_TypeDef *UARTx, uint16_t UART_FLAG);
void UART_SendData(UART_TypeDef *UARTx, uint8_t Data);
uint8_t  UART_ReceiveData(UART_TypeDef *UARTx);
void UART_Finish(UART_TypeDef *UARTx);

void UART_ITCLEAR(UART_TypeDef *UARTx, uint16_t USART_INTCLEAR);
/** 
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STAR_UART_H */
