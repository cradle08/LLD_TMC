#include "main.h"
#include "public.h"
#include "rtthread.h"




/*
*   us延时函数
*/
void Delay_US(uint32_t ulUS)
{
    uint32_t start, now, delta, reload, us_tick;
    start = SysTick->VAL;
    reload =  SysTick->LOAD;
    us_tick = (SystemCoreClock / 1000000UL)*ulUS;
    do {
        now = SysTick->VAL;
        delta = start > now ? start - now : reload + start - now;
    } while(delta < us_tick);
}






/*
 *  No Block delay
 */
uint8_t Delay_MS_NOBlock(uint32_t ulMS)
{
	static uint32_t s_ulTick = 0;
	uint32_t ulTempTick = 0;

	ulTempTick = rt_tick_get();// HAL_GetTick();
	if(ulTempTick - s_ulTick >= ulMS)
	{
		s_ulTick = ulTempTick;
		return 1;
	}

	return 0;
}



/*
*  非堵塞延时，时间间隔10ms
*/
uint8_t Delay_MS_NOBlock_10ms(void)
{
	static uint32_t s_ulTick = 0;
	uint32_t ulTempTick = 0;

	ulTempTick = rt_tick_get();//HAL_GetTick();
	if(ulTempTick - s_ulTick >= 10)
	{
		s_ulTick = ulTempTick;
		return 1;
	}

	return 0;
}






