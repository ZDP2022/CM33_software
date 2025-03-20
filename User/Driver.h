#ifndef __Driver_H__
#define __Driver_H__

#include "ADC_mux.h"
#include "GNSS.h"
#include "timer.h"




//��������
void AHB_SlaveWrite(uint32_t *address,uint32_t data);//д�Ĵ�����������
uint32_t AHB_SlaveRead(uint32_t address);//д�Ĵ�����������
uint32_t ADC_DataTransfer(uint16_t *BuffAddr, uint32_t Length);

//������������
void Read_Wave_Data();		//��DDR��ȡ��������
extern void Init_Homepage();		//��ʼ��������
void Refresh_Measure_Val();	//ˢ�²���ֵ
void Refresh_WaveWindow();	//ˢ�²��δ���
void Touch_Scan();			//����ɨ��
void STOP_Disable_Press();		//��STOP״̬�¶���ĳЩ����
void STOP_Ensable_Press();		//�����STOP״̬�±�����Ĳ���
void Trigger_Disable_Press();	//�ڵ��δ���״̬�¶���ĳЩ����
void Trigger_Enable_Press();	//����ڵ��δ���״̬�±�����Ĳ���
void Perform_STOP();		//ִ��STOP����

void Handle_Single_Trigger();
void Handle_Round_Done();

#endif /* Driver_H__ */
