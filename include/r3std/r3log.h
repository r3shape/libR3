#ifndef __R3LOG_H__
#define __R3LOG_H__

#include <include/r3std/r3def.h>

typedef enum R3_Log_Level {
	INFO_LOG,
	WARN_LOG,
	TRACE_LOG,
	ERROR_LOG,
	SUCCESS_LOG,
	R3_LOG_LEVELS
} R3_Log_Level;

#ifdef R3_MODULES
R3STD_API none r3_log(R3_Log_Level level, str message);
R3STD_API none r3_log_fmt(R3_Log_Level level, str message, ...);
#endif	// R3_MODULES

#endif	// __R3LOG_H__
