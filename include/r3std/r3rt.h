#ifndef __R3RT_H__
#define __R3RT_H__

#include <include/r3std/r3def.h>

typedef struct R3_Runtime_Data {
	struct log {
		u64 errors:
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

	u64 size;
	u8 state;
	u8 version;
} R3_Runtime_Data;

R3STD_API u8 r3_runtime_get(addr out);
R3STD_API u8 r3_runtime_reset(none);
R3STD_API u8 r3_runtime_dump(none);

#endif	// __R3RT_H__
