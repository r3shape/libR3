#include <include/r3kt/r3ktlog.h>
#include <stdio.h>
#include <stdarg.h>

const u32 _log_buffer_size = 1024;

str _log_tags[LOG_LEVELS] = {
	"[r3kt::INFO]",
	"[r3kt::WARN]",
	"[r3kt::TRACE]",
	"[r3kt::ERROR]",
	"[r3kt::SUCCESS]"
};

str _log_colors[LOG_LEVELS] = {
	"\033[94m",
	"\033[93m",
	"\033[95m",
	"\033[91m",
	"\033[92m"
};

none r3_log(Log_Level level, str message) {
	if (level < 0 || level >= LOG_LEVELS) {
		printf("\033[93m[r3kt::WARN] \033[0minvalid log level passed: %d", level);
		return;
	}
	printf("%s%s \033[0m%s", _log_colors[level], _log_tags[level], message);
}

none r3_log_fmt(Log_Level level, str message, ...) {
	char buffer[_log_buffer_size];
	
	va_list args;
	va_start(args, message);
	vsnprintf(buffer, _log_buffer_size, message, args);
	va_end(args);

	r3_log(level, buffer);
}

