#ifndef __R3KTLOG_H__
#define __R3KTLOG_H__

#include <include/r3kt/r3ktdef.h>

typedef enum Log_Level {
	INFO_LOG,
	WARN_LOG,
	TRACE_LOG,
	ERROR_LOG,
	SUCCESS_LOG,
	LOG_LEVELS
} Log_Level;

R3KT_API none r3_log(Log_Level level, str message);
R3KT_API none r3_log_fmt(Log_Level level, str message, ...);

#endif	// __R3KTLOG_H__
