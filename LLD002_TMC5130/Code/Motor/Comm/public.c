#include "main.h"
#include "public.h"
#include "rtthread.h"




/*
 * @function: Delay_US
 * @details : us������ʱ��
 * @input   : 1��us�����ʱ999us��
 * @output  : NULL
 * @return  : 0:������1����ʱ����Χ��
 */
uint8_t Delay_US(uint16_t us)
{
	uint8_t     ret = 0;
    uint32_t    start, now, dely_cnt, reload, us_tick;
	
    start = SysTick->VAL;
    reload =  SysTick->LOAD;
    us_tick = (SystemCoreClock / 1000000UL) * us;
	
	if(us_tick > reload)
	{
		ret = 1;
		return (ret);
	}
	
    do {
        now = SysTick->VAL;
        dely_cnt = (start > now) ? (start - now) : (reload + start - now);
    } while(dely_cnt < us_tick);
	
	
	return (ret);
}


/*
 * @function: Delay_MS_NOBlock
 * @details : ms������ʱ��
 * @input   : 1��ms�����ʱ255us��
 * @output  : NULL
 * @return  : 0:������1����ʱ����Χ��
 */
uint8_t Delay_MS_NOBlock(uint8_t ms)
{
	uint8_t     ret = 0;
    uint32_t    start, now, reload, dely_cnt;
	
	reload = 0xFFFFFFFF;
    start = rt_tick_get();
    do {
        now = rt_tick_get();
        dely_cnt = (start > now) ? (reload - start + now) : (now - start);
    } while(dely_cnt < ms);
	
	
	return (ret);
}




