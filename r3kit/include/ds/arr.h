#ifndef __R3_ARRAY_H__
#define __R3_ARRAY_H__

#include <r3kit/include/defines.h>

typedef enum Array_Header_Field {
	ARRAY_MAX_FIELD,
	ARRAY_SIZE_FIELD,
	ARRAY_COUNT_FIELD,
	ARRAY_STRIDE_FIELD,
	ARRAY_HEADER_FIELDS
} Array_Header_Field;

// bare 'wrapper' structure for type saftey
// the internal data field is ALWAYS allocated
// with 8 header bytes
typedef struct Array {
	ptr data;
} Array;

// TODO: r3_arr_put: at index, shift right 1 and write a value

R3_API u16 r3_arr_field(u8 field, Array* in);

R3_API u8 r3_arr_dealloc(Array* in);
R3_API u8 r3_arr_realloc(u16 max, Array* in);
R3_API u8 r3_arr_alloc(u16 max, u16 stride, Array* out);

R3_API u8 r3_arr_shift_left(u16 index, u16 shift, Array *in);
R3_API u8 r3_arr_shift_right(u16 index, u16 shift, Array *in);

R3_API u8 r3_arr_pop(ptr value, Array* in);
R3_API u8 r3_arr_push(ptr value, Array* in);
R3_API u8 r3_arr_pull(u16 index, ptr value, Array* in);

R3_API u8 r3_arr_read(u16 index, ptr value, Array* in);
R3_API u8 r3_arr_write(u16 index, ptr value, Array* in);
R3_API u8 r3_arr_assign(u16 index, ptr value, Array* in);

/* HASH ARRAY */
R3_API u8 r3_arr_hashed_dealloc(Array* in);
R3_API u8 r3_arr_hashed_alloc(u16 max, u16 stride, Array* out);

R3_API u8 r3_arr_hashed_pull(const char* key, ptr value, Array* in);

R3_API u8 r3_arr_hashed_read(const char* key, ptr value, Array* in);
R3_API u8 r3_arr_hashed_write(const char* key, ptr value, Array* in);
R3_API u8 r3_arr_hashed_assign(const char* key, ptr value, Array* in);

#endif // __R3_ARRAY_H__
