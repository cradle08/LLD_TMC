#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "bsp_usart.h"


#define LOG_BUFFER_LEN 	512

/*
 *  Log Output Level
 */
typedef enum
{
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_WARM,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_END,

}Log_Level_e;


/*
 * Log Output Style
 */
typedef enum
{
	LOG_STYLE_NONE	= 0,	//output none log
	LOG_STYLE_LESS,			//output log : less than g_eLogLevel
	LOG_STYLE_MORE,			//output log : more than g_eLogLevel
	LOG_STYLE_ALL,			//output all log

}Log_Style_e;



extern __IO Log_Level_e g_eLogLevel;
#define LOG_Info(fmt, ...)	//{if(g_eLogLevel <= LOG_LEVEL_INFO)  printf(fmt"\r\n", ##__VA_ARGS__);}
#define LOG_Debug(fmt, ...)	//{if(g_eLogLevel <= LOG_LEVEL_DEBUG) printf(fmt"\r\n", ##__VA_ARGS__);}
#define LOG_Warn(fmt, ...)	//{if(g_eLogLevel <= LOG_LEVEL_WARM)  printf("WARN: %s-%d "fmt"\r\n", __FILE__, __LINE__, ##__VA_ARGS__);}
#define LOG_Error(fmt, ...) //{if(g_eLogLevel <= LOG_LEVEL_ERROR) printf("ERROR: %s-%d "fmt"\r\n", __FILE__, __LINE__, ##__VA_ARGS__);}




//log format and output
void Log_Output(Log_Level_e eLogLevel, char *fmt, ...);
void Log_Output_Usart(uint8_t *pucaLogBuffer);






#ifdef __cplusplus
}
#endif




#endif //__LOG_H__



