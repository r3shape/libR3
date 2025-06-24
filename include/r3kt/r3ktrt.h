#ifndef __R3KTRT_H__
#define __R3KTRT_H__

#include <include/r3kt/r3ktdef.h>

typedef struct R3kt_Runtime {
	struct log {
		u64 errors;
		u64 warns;
		u8 active;
	} log;

	struct mem {
		u64 min;
		u64 max;
		u64 used;
		u64 allocs;
		u64 reallocs;
		u64 deallocs;
	} mem;

	struct arr {
		u64 creates;
		u64 resizes;
		u64 destroys;
	} arr;

	struct str {
		u64 creates;
		u64 resizes;
		u64 destroys;
	} str;

	struct file {
		u64 opened;
		u64 closed;
		u64 writes;
		u64 reads;
		u64 wrote;
		u64 read;
	} file;

	str version;
	u64 size;
	u8 state;
} R3kt_Runtime;

R3KT_API u8 r3_runtime_dump(none);
R3KT_API u8 r3_runtime_reset(none);
R3KT_API u8 r3_runtime_pointer(addr* out);

#endif	// __R3KTRT_H__
