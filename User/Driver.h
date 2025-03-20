#ifndef __Driver_H__
#define __Driver_H__

#include "ADC_mux.h"
#include "GNSS.h"
#include "timer.h"




//函数声明
void AHB_SlaveWrite(uint32_t *address,uint32_t data);//写寄存器操作函数
uint32_t AHB_SlaveRead(uint32_t address);//写寄存器操作函数
uint32_t ADC_DataTransfer(uint16_t *BuffAddr, uint32_t Length);

//声明操作函数
void Read_Wave_Data();		//从DDR读取波形数据
extern void Init_Homepage();		//初始化主界面
void Refresh_Measure_Val();	//刷新测量值
void Refresh_WaveWindow();	//刷新波形窗口
void Touch_Scan();			//触摸扫描
void STOP_Disable_Press();		//在STOP状态下冻结某些操作
void STOP_Ensable_Press();		//解除在STOP状态下被冻结的操作
void Trigger_Disable_Press();	//在单次触发状态下冻结某些操作
void Trigger_Enable_Press();	//解除在单次触发状态下被冻结的操作
void Perform_STOP();		//执行STOP操作

void Handle_Single_Trigger();
void Handle_Round_Done();

#endif /* Driver_H__ */
