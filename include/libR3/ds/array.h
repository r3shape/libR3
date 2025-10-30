#ifndef __R3_DS_ARRAY_H__
#define __R3_DS_ARRAY_H__

#include <include/libR3/r3def.h>

typedef struct R3ArrayHeader {
    u64 size;
    u16 mask;
    u64 count;
    u64 slots;
    u16 stride;
} R3ArrayHeader;

R3_PUBLIC_API u16 r3ArrayCount(ptr array);
R3_PUBLIC_API u16 r3ArraySlots(ptr array);
R3_PUBLIC_API u16 r3ArrayStride(ptr array);
R3_PUBLIC_API R3Result r3ArrayHeader(R3ArrayHeader* header, ptr array);

R3_PUBLIC_API R3Result r3DelArray(ptr array);
R3_PUBLIC_API ptr r3NewArray(u64 slots, u16 stride);
R3_PUBLIC_API ptr r3ResizeArray(u64 slots, u16 stride, ptr array);

R3_PUBLIC_API R3Result r3RShiftArray(u64 slot, u64 shift, ptr array);
R3_PUBLIC_API R3Result r3LShiftArray(u64 slot, u64 shift, ptr array);

R3_PUBLIC_API R3Result r3PopArray(ptr array, ptr dest);
R3_PUBLIC_API R3Result r3PushArray(ptr value, ptr array);
R3_PUBLIC_API R3Result r3PullArray(u64 slot, ptr array, ptr out);
R3_PUBLIC_API R3Result r3PutArray(u64 slot, ptr value, ptr array);

R3_PUBLIC_API R3Result r3RemArray(u64 slot, ptr array, ptr out);
R3_PUBLIC_API R3Result r3GetArray(u64 slot, ptr array, ptr dest);
R3_PUBLIC_API R3Result r3SetArray(u64 slot, ptr value, ptr array);
R3_PUBLIC_API R3Result r3AssignArray(u64 slot, ptr value, ptr array);

#endif // __R3_DS_ARRAY_H__
