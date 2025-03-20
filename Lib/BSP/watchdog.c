#include "STAR_gpio.h"
#include "STAR.h"
#include "STAR_watchdog.h"
#include "watchdog.h"


//���Ź���ʼ��
void watchdog_init(int type)
{
	//�������Ź�
	WatchDog_UNLOCK(STAR_WATCHDOG);
	//���ÿ��Ź�����ֵ ����ΪΪ50000000(������2s��Ҫι��,PCLK=25M)
	WatchDog_Set_LOAD(STAR_WATCHDOG,50000000);
	
	//ע�⣡������Ҫʹ�ܿ��Ź��������Ϳ��Ź��жϣ������Ź����Ṥ��
	//ʹ�ܿ��Ź��жϺ����жϴ���������ڲ�������ж� �������ʹ�ܿ��Ź���λ��� ���Ź����Ḵλ����
	//--->���Ź���������0������жϣ��ٹ�2�����ҲŻḴλ�������������жϺ��������жϾͲ����ٸ�λ��
	
	if (type==0) //typeΪ0 ��ʹ�ܿ��Ź���λ��� ʹ�ܿ��Ź��������Ϳ��Ź��ж�
	{
		WatchDog_Set_CTRL(STAR_WATCHDOG,0,1);
	} 
	else  //typeΪ1 ��ʹ�ܿ��Ź���λ���(��ι���ͻ����ϵͳ��λ) ʹ�ܿ��Ź��������Ϳ��Ź��ж�
	{
		WatchDog_Set_CTRL(STAR_WATCHDOG,1,1); 
	}
	//�������Ź�
	WatchDog_LOCK(STAR_WATCHDOG);
}

//���Ź��жϷ�����
void NMI_Handler(void)
{
	
	//LED-D5�������
	GPIO_TogglePin(STAR_GPIO0, GPIO_Pin_0);	
	
	//WatchDog_Set_INTCLR(STAR_WATCHDOG);	//����ж�---�����Ҫ����ϵͳ��λ����Ҫ����ж� ���жϲ���������ϵͳ��λ
}

//���Ź�ι������
void watchdog_feed(uint32_t reload)
{
	WatchDog_UNLOCK(STAR_WATCHDOG);
	WatchDog_Set_LOAD(STAR_WATCHDOG,reload); //��������reloadֵ����ι��
	WatchDog_LOCK(STAR_WATCHDOG);
}

void watchdog_test(void)
{
	uint32_t loadnum;
	
	loadnum=WatchDog_Get_VALUE(STAR_WATCHDOG); //��ȡ���Ź���ǰ����ֵ���������ֵ��ҪΪ0 ������ι�����ڿ��Ź�����ֵΪ0ǰҪι�� ����Ḵλ
	if(loadnum<500)
	{		
		watchdog_feed(50000000);	//ι��-��ι������и�λ
	}
		
}



