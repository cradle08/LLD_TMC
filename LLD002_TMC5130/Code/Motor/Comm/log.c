#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"

#include "log.h"
#include "bsp_usart.h"


//var, define log output level, default info level
__IO Log_Level_e g_eLogLevel = LOG_LEVEL_INFO;
// log output style
__IO Log_Style_e g_eLogStyle = LOG_STYLE_ALL;

//log buffer
uint8_t g_ucaLogBuffer[LOG_BUFFER_LEN+1] = {0};


//log format output
void Log_Output(Log_Level_e eLogLevel, char *fmt, ...)
{
	int res = 0;
	va_list args;

	//not need output log
	if(LOG_STYLE_NONE == g_eLogStyle) return;

	//init
	memset((void*)g_ucaLogBuffer, 0, sizeof(g_ucaLogBuffer));
	va_start(args, fmt);

	//format
	res = vsnprintf((char*)g_ucaLogBuffer, LOG_BUFFER_LEN, fmt, args);
	va_end(args);

	//output
	if((res > -1) & (res <= LOG_BUFFER_LEN))
	{
		switch(eLogLevel)
		{
			case LOG_STYLE_NONE:
			{
				;
			}
			break;
			case LOG_STYLE_LESS:
			{
				if(eLogLevel <= g_eLogLevel) Log_Output_Usart(g_ucaLogBuffer);
			}
			break;
			case LOG_STYLE_MORE:
			{
				if(eLogLevel >= g_eLogLevel) Log_Output_Usart(g_ucaLogBuffer);
			}
			break;
			case LOG_STYLE_ALL:
			{
				Log_Output_Usart(g_ucaLogBuffer);
			}
			break;
			default:break;
		}
	}else{
		LOG_Error("Log Buffer Is OverFlow");
	}
}


/*
 * Output Log via Usart
 */
void Log_Output_Usart(uint8_t *pucaLogBuffer)
{
	printf("%s", pucaLogBuffer);
}



