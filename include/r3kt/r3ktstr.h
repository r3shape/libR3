#ifndef __R3KTSTR_H__
#define __R3KTSTR_H__

#include <include/r3kt/r3ktdef.h>

typedef enum String_Header_Field {
	MAX_FIELD,
	SIZE_FIELD,
	COUNT_FIELD,
	STRING_HEADER_FIELDS
} String_Header_Field;

// bare 'wrapper' structure for type safety
// the internal memory block is ALWAYS allocated
// with 1 byte extra for null-termination at string max
// (along with 6 header bytes)
typedef struct String {
	addr data;
} String;

R3KT_API u8 r3_dealloc_string(String* in);
R3KT_API u8 r3_alloc_string(u16 max, String* out);
R3KT_API u16 r3_string_field(u8 field, String* in);
R3KT_API u8 r3_combine_string(String* dest, String* src);
R3KT_API u8 r3_read_string(u16 count, char* value, String* in);
R3KT_API u8 r3_write_string(u16 count, char* value, String* in);
R3KT_API u8 r3_extend_string(u16 count, char* value, String* in);
R3KT_API u8 r3_copy_string(u16 count, String* dest, String* src);
R3KT_API u8 r3_slice_string(u16 start, u16 end, char* value, String* in);
R3KT_API u8 r3_read_string_at(u16 offset, u16 count, char* value, String* in);
R3KT_API u8 r3_write_string_at(u16 offset, u16 count, char* value, String* in);
R3KT_API u8 r3_copy_string_at(u16 offset, u16 count, String* dest, String* src);

#endif // __R3KTSTR_H__
