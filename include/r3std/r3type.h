#ifndef __R3TYPE_H__
#define __R3TYPE_H__

#include <include/r3std/r3def.h>

typedef struct Blob {
	addr data;
	u64 size;
	u32 align;
} Blob;

#endif // __R3TYPE_H__
