#include <include/r3std/r3log.h>
#include <stdio.h>
#include <stdarg.h>

const u32 _log_buffer_size = 1024;

str _log_tags[R3_LOG_LEVELS] = {
	"[r3log::INFO]",
	"[r3log::WARN]",
	"[r3log::TRACE]",
	"[r3log::ERROR]",
	"[r3log::SUCCESS]"
};

str _log_colors[R3_LOG_LEVELS] = {
	"\033[94m",
	"\033[93m",
	"\033[95m",
	"\033[91m",
	"\033[92m"
};

none r3_log(R3_Log_Level level, str message) {
	if (level < 0 || level >= R3_LOG_LEVELS) {
		printf("\033[93m[r3log::WARN] \033[0minvalid log level passed: %d", level);
		return;
	}
	printf("%s%s \033[0m%s", _log_colors[level], _log_tags[level], message);
}

none r3_log_fmt(R3_Log_Level level, str message, ...) {
	char buffer[_log_buffer_size];
	
	va_list args;
	va_start(args, message);
	vsnprintf(buffer, _log_buffer_size, message, args);
	va_end(args);

	r3_log(level, buffer);
}

