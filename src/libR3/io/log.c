#include <include/libR3/io/log.h>
#include <stdarg.h>
#include <stdio.h>

#define R3_LOG_LEVELS 5

char* R3LogTag[R3_LOG_LEVELS] = {
    "[R3 LOG ERROR]",
    "[R3 LOG WARN]",
    "[R3 LOG INFO]",
    "[R3 LOG DEV]",
    "[R3 LOG OK]"
};

char* R3LogColor[R3_LOG_LEVELS] = {
    "\033[91m",
    "\033[93m",
    "\033[94m",
    "\033[95m",
    "\033[92m"
};

static u8 R3LogState[R3_LOG_LEVELS] = {1, 1, 1, 1, 1};


none r3LogToggle(R3LogLevel level) {
	if (level >= R3_LOG_LEVELS) { return; }
	R3LogState[level] = !R3LogState[level];
}

none r3LogStdOut(R3LogLevel level, char* message) {
	if (level >= R3_LOG_LEVELS) {
		printf("\033[93m[R3 LOG WARN] \033[0minvalid log level passed: %d", level);
		return;
	} if (R3LogState[level] != 1) { return;}
	printf("%s%s \033[0m%s", R3LogColor[level], R3LogTag[level], message);
}

none r3LogStdOutF(R3LogLevel level, char* message, ...) {
	if (level >= R3_LOG_LEVELS) {
		printf("\033[93m[R3 LOG WARN] \033[0minvalid log level passed: %d", level);
		return;
	} if (R3LogState[level] != 1) { return; }

	char buffer[1024];
	
	va_list args;
	va_start(args, message);
	vsnprintf(buffer, sizeof(buffer), message, args);
	va_end(args);

	r3LogStdOut(level, buffer);
}

