#ifndef __R3KTARR_H__
#define __R3KTARR_H__

#include <include/r3kt/r3ktdef.h>

typedef enum Array_Header_Field {
	MAX_FIELD,
	SIZE_FIELD,
	COUNT_FIELD,
	STRIDE_FIELD,
	ARRAY_HEADER_FIELDS
} Array_Header_Field;

// bare 'wrapper' structure for type saftey
// the internal memory block is ALWAYS allocated
// with 8 header bytes
typedef struct Array {
	addr data;
} Array;

/*	DYNAMIC ARRAY	*/
R3KT_API u8 r3_dealloc_array(Array* in);
R3KT_API u8 r3_push_array(addr value, Array* in);
R3KT_API u8 r3_pull_array(addr value, Array* in);
R3KT_API u16 r3_array_field(u8 field, Array* in);
R3KT_API u8 r3_alloc_array(u16 max, u16 stride, Array* out);
R3KT_API u8 r3_read_array(u16 index, addr value, Array* in);
R3KT_API u8 r3_write_array(u16 index, addr value, Array* in);

#endif // __R3KTARR_H__
