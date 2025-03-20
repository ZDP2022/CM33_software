#include "STAR_gpio.h"
#include "STAR.h"
#include "STAR_watchdog.h"
#include "watchdog.h"


//看门狗初始化
void watchdog_init(int type)
{
	//解锁看门狗
	WatchDog_UNLOCK(STAR_WATCHDOG);
	//设置看门狗加载值 设置为为50000000(即超过2s就要喂狗,PCLK=25M)
	WatchDog_Set_LOAD(STAR_WATCHDOG,50000000);
	
	//注意！！！需要使能看门狗计数器和看门狗中断，否则看门狗不会工作
	//使能看门狗中断后，在中断处理服务函数内不能清除中断 否则如果使能看门狗复位输出 看门狗不会复位重启
	//--->看门狗计数器到0后产生中断，再过2秒左右才会复位，因此如果产生中断后就清除了中断就不会再复位了
	
	if (type==0) //type为0 则不使能看门狗复位输出 使能看门狗计数器和看门狗中断
	{
		WatchDog_Set_CTRL(STAR_WATCHDOG,0,1);
	} 
	else  //type为1 则使能看门狗复位输出(不喂狗就会进行系统复位) 使能看门狗计数器和看门狗中断
	{
		WatchDog_Set_CTRL(STAR_WATCHDOG,1,1); 
	}
	//锁定看门狗
	WatchDog_LOCK(STAR_WATCHDOG);
}

//看门狗中断服务函数
void NMI_Handler(void)
{
	
	//LED-D5亮灭控制
	GPIO_TogglePin(STAR_GPIO0, GPIO_Pin_0);	
	
	//WatchDog_Set_INTCLR(STAR_WATCHDOG);	//清除中断---如果需要产生系统复位，则不要清除中断 在中断产生后会进行系统复位
}

//看门狗喂狗函数
void watchdog_feed(uint32_t reload)
{
	WatchDog_UNLOCK(STAR_WATCHDOG);
	WatchDog_Set_LOAD(STAR_WATCHDOG,reload); //重新设置reload值进行喂狗
	WatchDog_LOCK(STAR_WATCHDOG);
}

void watchdog_test(void)
{
	uint32_t loadnum;
	
	loadnum=WatchDog_Get_VALUE(STAR_WATCHDOG); //获取看门狗当前计数值，如果计数值快要为0 则重新喂狗，在看门狗计数值为0前要喂狗 否则会复位
	if(loadnum<500)
	{		
		watchdog_feed(50000000);	//喂狗-不喂狗会进行复位
	}
		
}



