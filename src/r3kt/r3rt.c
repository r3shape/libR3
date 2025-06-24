#ifdef R3KT_BUILD_RUNTIME_SOURCE

#include <include/r3kt/r3log.h>
#include <include/r3kt/r3rt.h>

#define R3KT_RUNTIME_VERSION "1.0.0"

R3kt_Runtime _runtime_internal = {
	.log = {0},
	.mem = {0},
	.arr = {0},
	.str = {0},
	.file = {0},

	.state = 0,
	.size = sizeof(R3kt_Runtime),
	.version = R3KT_RUNTIME_VERSION
};

u8 r3_runtime_dump(none) {
	r3_log_fmt(INFO_LOG, "[r3ktrt] v%s\n", _runtime_internal.version);
	return 1;
}

u8 r3_runtime_reset(none) { return 1; }

u8 r3_runtime_pointer(addr* out) {
	if (!out) {
		r3_log(WARN_LOG, "[r3ktrt] failed to return runtime pointer. invalid pointer parameter.");
		return 0;
	}

	*out = &_runtime_internal;
	return 1;
}

#endif	// R3KT_BUILD_RUNTIME_SOURCE
