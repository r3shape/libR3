#include <r3kit/include/io/log.h>
#include <stdarg.h>
#include <stdio.h>

const u32 _log_buffer_size = 1024;

char* _log_tags[LOG_LEVELS] = {
	"[r3kit::INFO]",
	"[r3kit::WARN]",
	"[r3kit::TRACE]",
	"[r3kit::ERROR]",
	"[r3kit::SUCCESS]"
};

char* _log_colors[LOG_LEVELS] = {
	"\033[94m",
	"\033[93m",
	"\033[95m",
	"\033[91m",
	"\033[92m"
};

struct Log_State {
	u8 toggle[LOG_LEVELS];
} _Log_State = { .toggle = {1, 1, 1, 1, 1} };


none r3_log_toggle(Log_Level level) {
	if (level >= LOG_LEVELS) { return; }
	_Log_State.toggle[level] = !_Log_State.toggle[level];
}

none r3_log_stdout(Log_Level level, char* message) {
	if (level >= LOG_LEVELS) {
		printf("\033[93m[r3kit::WARN] \033[0minvalid log level passed: %d", level);
		return;
	} if (_Log_State.toggle[level] != 1) { return;}
	printf("%s%s \033[0m%s", _log_colors[level], _log_tags[level], message);
}

none r3_log_stdoutf(Log_Level level, char* message, ...) {
	if (level >= LOG_LEVELS) {
		printf("\033[93m[r3kit::WARN] \033[0minvalid log level passed: %d", level);
		return;
	} if (_Log_State.toggle[level] != 1) { return; }

	char buffer[_log_buffer_size];
	
	va_list args;
	va_start(args, message);
	vsnprintf(buffer, _log_buffer_size, message, args);
	va_end(args);

	r3_log_stdout(level, buffer);
}

