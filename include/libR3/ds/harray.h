#ifndef __R3_DS_HARRAY_H__
#define __R3_DS_HARRAY_H__

#include <include/libR3/ds/array.h>

R3_PUBLIC_API R3Result r3DelHashArray(ptr array);
R3_PUBLIC_API ptr r3NewHashArray(u64 slots, u16 stride);
R3_PUBLIC_API ptr r3ResizeHashArray(u64 slots, ptr array);

R3_PUBLIC_API R3Result r3PopHashArray(char* key, ptr array, ptr value);
R3_PUBLIC_API R3Result r3GetHashArray(char* key, ptr array, ptr value);
R3_PUBLIC_API R3Result r3SetHashArray(char* key, ptr value, ptr array);
R3_PUBLIC_API R3Result r3RemHashArray(char* key, ptr array, ptr value);
R3_PUBLIC_API R3Result r3AssignHashArray(char* key, ptr value, ptr array);

#endif // __R3_DS_HARRAY_H__