
#ifndef ADC_MUX_H
#define ADC_MUX_H


/****************** Include Files ********************/
#include "stdint.h"
#include "STAR.h"

#define	ADC_IP_BASEADDR CM3DS_MPS2_TARGEXP1_BASE	//ADC IP����ַ��Ҳ����AHB����1�ĵ�ַ
/****************** Memory Map ********************/
//   8'h00: rdata =  ctr_reg0	;	//0xA000_0000 �ش�	//�Ĵ���0�����������ݣ����������ݣ�������ʱ�䣬������ʱ�䣬������ͣ
//   8'h04: rdata =  ctr_reg0	;	//0xA000_0004 Ԥ��
//   8'h08: rdata =  ctr_reg0	;	//0xA000_0008 Ԥ��
//   8'h0c: rdata =  ctr_reg0	;	//0xA000_000c Ԥ�� 
//
//   8'h10: rdata =  TIME_DATA[127:96];					//0xA000_0010 ����ʱ�� ʱ����
//   8'h14: rdata =  TIME_DATA[63:32];					//0xA000_0014 ����ʱ�� ʱ����
//   8'h18: rdata =  {4'h0,TIME_DATA[83:64],INDEX_DATA};//0xA000_0018 ����ʱ��+�ߵͱ�ǩ 4+20+8
//   8'h1c: rdata =  {4'h0,TIME_DATA[19:0],INDEX_DATA};	//0xA000_001c ����ʱ��+�ߵͱ�ǩ 4+20+8
//
//   8'h20: rdata =  AMP_ch01	;	//0xA000_0020fft��Чֵ
//   8'h24: rdata =  AMP_ch23	;	//0xA000_0024
//   8'h28: rdata =  AMP_ch45	;	//0xA000_0028
//   8'h2c: rdata =  AMP_ch67	;	//0xA000_002c
//
//   8'h30: rdata =  ADC_DATA	;	//0xA000_0030
#define REG_ADC_CONTROL (ADC_IP_BASEADDR+0x20)
#define REG_ADC_MINMAX  (ADC_IP_BASEADDR+0x24)
#define REG_ADC_STATE   (ADC_IP_BASEADDR+0x28)
#define REG_ADC_FREQ    (ADC_IP_BASEADDR+0x2C)


void SET_ADC_REG(uint32_t *address,uint32_t data);
uint32_t GET_ADC_REG(uint32_t address);
void Set_ADC_Mode(uint32_t Channel,uint32_t Sample_Rate,
		uint32_t Trigger_Val,uint32_t Trigger_Mode);
uint32_t Get_ADC_Freq();
float Get_ADC_Vmin();
float Get_ADC_Vmax();
float Get_ADC_Vmid();
uint16_t Get_ADC_MidVal();

#endif // ADC_MUX_H
