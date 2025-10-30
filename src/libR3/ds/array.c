#include <include/libR3/ds/array.h>
#include <include/libR3/mem/mem.h>
#include <include/libR3/io/log.h>

#define R3_ARRAY_HEADER_SIZE sizeof(R3ArrayHeader)

inline u16 r3ArrayCount(ptr array) {
    if (!array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelArray` -- invalid pointer\n");
        return R3_RESULT_ERROR;
    } return ((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE))->count;
}

inline u16 r3ArraySlots(ptr array) {
    if (!array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelArray` -- invalid pointer\n");
        return R3_RESULT_ERROR;
    } return ((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE))->slots;
}

inline u16 r3ArrayStride(ptr array) {
    if (!array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelArray` -- invalid pointer\n");
        return R3_RESULT_ERROR;
    } return ((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE))->stride;
}

R3Result r3ArrayHeader(R3ArrayHeader* header, ptr array) {
    if (!header) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelArray` -- invalid pointer\n");
        return R3_RESULT_ERROR;
    } if (!array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelArray` -- invalid pointer\n");
        return R3_RESULT_ERROR;
    }

    *header = *((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE));

    return R3_RESULT_SUCCESS;
}


R3Result r3DelArray(ptr array) {
    if (!array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelArray` -- invalid pointer\n");
        return R3_RESULT_ERROR;
    }

    ptr raw = (ptr)((u8*)array - R3_ARRAY_HEADER_SIZE);
    r3FreeMemory(raw);
    array = NULL;

    return R3_RESULT_SUCCESS;
}

ptr r3NewArray(u64 slots, u16 stride) {
    if (!slots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewArray` -- invalid slot count: %d\n", slots);
        return NULL;
    } if (!stride) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewArray` -- invalid slot stride: %d\n", stride);
        return NULL;
    }

    u64 size = slots * stride;
    ptr raw = r3AllocMemory(R3_ARRAY_HEADER_SIZE + size);
    if (!raw) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewArray` -- array memory allocation failed\n");
        return NULL;
        return NULL;
    }

    *((R3ArrayHeader*)raw) = (R3ArrayHeader){
        .mask = 0,
        .count = 0,
        .size = size,
        .slots = slots,
        .stride = stride
    };

    ptr array = (ptr)((u8*)raw + R3_ARRAY_HEADER_SIZE);

    return array;
}

ptr r3ResizeArray(u64 slots, u16 stride, ptr array) {
    if (!slots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `ResizeArray` -- invalid slot count: %d\n", slots);
        return NULL;
    } if (!stride) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `ResizeArray` -- invalid slot stride: %d\n", stride);
        return NULL;
    } if (!array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ResizeArray` -- invalid pointer\n");
        return NULL;
    }
    
    ptr raw = (ptr)((u8*)array - R3_ARRAY_HEADER_SIZE);
    u16 ostride = ((R3ArrayHeader*)raw)->stride;
    u64 ocount = ((R3ArrayHeader*)raw)->count;

    ptr nraw = r3ReallocMemory(R3_ARRAY_HEADER_SIZE + (slots * stride), raw);
    ((R3ArrayHeader*)nraw)->stride = stride;
    ((R3ArrayHeader*)nraw)->slots = slots;

    ptr narray = (ptr)((u8*)nraw + R3_ARRAY_HEADER_SIZE);
    if (r3SetMemory(slots * stride, 0, (ptr)((u8*)narray + (ocount * ostride))) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_WARN, "Warning `ResizeArray` -- array memory set failed\n");
    }

    return narray;
}


R3Result r3RShiftArray(u64 slot, u64 shift, ptr array) {
    if (!array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `RShiftArray` -- invalid pointer\n");
        return R3_RESULT_ERROR;
    }

    R3ArrayHeader* h = ((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE));
    if (slot >= h->slots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `RShiftArray` -- invalid array slot: %d\n", slot);
        return R3_RESULT_ERROR;
    } if (shift >= h->slots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `RShiftArray` -- invalid array shift: %d\n", shift);
        return R3_RESULT_ERROR;
    } 

    if ((h->slots - h->count) >= shift) {   // OOB shift guard
        if (r3MoveMemory(h->stride * (h->count - slot), (ptr)((u8*)array + (h->stride * slot)), (ptr)((u8*)array + (h->stride * (slot + shift)))) != R3_RESULT_SUCCESS) {
            r3LogStdOut(R3_LOG_ERROR, "Failed `RShiftArray` -- array memory move failed\n");
            return R3_RESULT_ERROR;
        }
    } else {
        if (r3MoveMemory(h->stride * ((h->count - slot) - (shift - (h->slots - h->count))), (ptr)((u8*)array + (h->stride * slot)), (ptr)((u8*)array + (h->stride * (slot + shift)))) != R3_RESULT_SUCCESS) {
            r3LogStdOut(R3_LOG_ERROR, "Failed `RShiftArray` -- array memory move failed\n");
            return R3_RESULT_ERROR;
        }
    }

    if (r3SetMemory(h->stride * shift, 0, (ptr)((u8*)array + (h->stride * slot))) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_WARN, "Warning `LShiftArray` -- array memory set failed\n");
    }

    h->count = MIN(h->slots, h->count + shift);

    return R3_RESULT_SUCCESS;
}

R3Result r3LShiftArray(u64 slot, u64 shift, ptr array) {
    if (!array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `LShiftArray` -- invalid pointer\n");
        return R3_RESULT_ERROR;
    }

    R3ArrayHeader* h = ((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE));
    if (slot > h->slots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `LShiftArray` -- invalid slot: %d\n", slot);
        return R3_RESULT_ERROR;
    } if (!shift || shift > slot) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `LShiftArray` -- invalid shift: %d\n", shift);
        return R3_RESULT_ERROR;
    }

    if (r3MoveMemory(h->stride * h->count, (ptr)((u8*)array + (h->stride * slot)), (ptr)((u8*)array + (h->stride * (slot - shift)))) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `LShiftArray` -- array memory move failed\n");
        return R3_RESULT_ERROR;
    }

    if (r3SetMemory(h->stride * shift, 0, (ptr)((u8*)array + (h->stride * (h->count - shift)))) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_WARN, "Warning `LShiftArray` -- array memory set failed\n");
    }

    h->count -= shift;

    return R3_RESULT_SUCCESS;
}


R3Result r3PopArray(ptr array, ptr dest) {
    if (!dest || !array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `PopArray` -- invalid dest/array pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3ArrayHeader* h = ((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE));
    if (!h->count) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `PopArray` -- pop from empty array caught\n");
        return R3_RESULT_ERROR;
    }

    ptr readat = (ptr)((u8*)array + (h->stride * (h->count - 1)));
    if (!readat) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `PopArray` -- pop from empty slot caught\n");
        return R3_RESULT_ERROR;
    }

    if (r3ReadMemory(h->stride, readat, dest) != R3_RESULT_SUCCESS\
    ||  r3SetMemory(h->stride, 0, readat) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `PopArray` -- array memory read/set failed\n");
        return R3_RESULT_ERROR;
    }

    h->count--;

    return R3_RESULT_SUCCESS;
}

R3Result r3PushArray(ptr value, ptr array) {
    if (!value || !array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `PushArray` -- invalid value/array pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3ArrayHeader* h = ((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE));
    ptr writeat = (ptr)((u8*)array + (h->stride * h->count));
    if (r3WriteMemory(h->stride, value, writeat) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `PushArray` -- array memory write failed\n");
        return R3_RESULT_ERROR;
    }

    h->count++;

    return R3_RESULT_SUCCESS;
}

R3Result r3PullArray(u64 slot, ptr array, ptr out) {
    if (r3GetArray(slot, array, out) != R3_RESULT_SUCCESS
    ||  r3LShiftArray(slot+1, 1, array) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `PullArray` -- array shift/set failed\n");
        return R3_RESULT_ERROR;
    } return R3_RESULT_SUCCESS;
}

R3Result r3PutArray(u64 slot, ptr value, ptr array) {
    if (r3RShiftArray(slot, 1, array) != R3_RESULT_SUCCESS
    ||  r3SetArray(slot, value, array) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `PutArray` -- array shift/set failed\n");
        return R3_RESULT_ERROR;
    } return R3_RESULT_SUCCESS;
}


R3Result r3RemArray(u64 slot, ptr array, ptr out) {
    if (!out || !array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `RemArray` -- invalid out/array pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3ArrayHeader* h = ((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE));
    if (slot > h->slots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `RemArray` -- invalid array slot: %d\n", slot);
        return R3_RESULT_ERROR;
    }

    ptr readat = (ptr)((u8*)array + (h->stride * slot));
    if (!*(u64*)readat) return R3_RESULT_SUCCESS;

    if (r3ReadMemory(h->stride, readat, out) != R3_RESULT_SUCCESS\
    ||  r3SetMemory(h->stride, 0, readat) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `RemArray` -- array memory write failed\n");
        return R3_RESULT_ERROR;
    }

    h->count--;

    return R3_RESULT_SUCCESS;
}

R3Result r3GetArray(u64 slot, ptr array, ptr dest) {
    if (!dest || !array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `GetArray` -- invalid dest/array pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3ArrayHeader* h = ((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE));
    if (slot > h->slots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `GetArray` -- invalid array slot: (slot)%d (slots)%d\n", slot, h->slots);
        return R3_RESULT_ERROR;
    }

    ptr readat = (ptr)((u8*)array + (h->stride * slot));
    if (r3ReadMemory(h->stride, readat, dest) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `GetArray` -- array memory write failed\n");
        return R3_RESULT_ERROR;
    }

    return R3_RESULT_SUCCESS;
}

R3Result r3SetArray(u64 slot, ptr value, ptr array) {
    if (!value || !array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `SetArray` -- invalid value/array pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3ArrayHeader* h = ((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE));
    if (slot > h->slots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `SetArray` -- invalid array slot: %d\n", slot);
        return R3_RESULT_ERROR;
    }

    u8 used = 1;
    ptr writeat = (ptr)((u8*)array + (h->stride * slot));
    if (!*(u64*)writeat) used = 0;

    if (r3WriteMemory(h->stride, value, writeat) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `SetArray` -- array memory write failed\n");
        return R3_RESULT_ERROR;
    }

    if (!used) h->count++;

    return R3_RESULT_SUCCESS;
}

R3Result r3AssignArray(u64 slot, ptr value, ptr array) {
    if (!value || !array) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `AssignArray` -- invalid value/array pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3ArrayHeader* h = ((R3ArrayHeader*)((u8*)array - R3_ARRAY_HEADER_SIZE));
    if (slot > h->slots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `AssignArray` -- invalid array slot: %d\n", slot);
        return R3_RESULT_ERROR;
    }

    u8 used = 1;
    ptr writeat = (ptr)((u8*)array + (h->stride * slot));
    if (!*(u64*)writeat) used = 0;

    *(u64*)((u8*)array + (h->stride * slot)) = (u64)value;
    if (!used) h->count++;

    return R3_RESULT_SUCCESS;
}
