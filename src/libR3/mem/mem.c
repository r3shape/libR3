#include <include/libR3/mem/mem.h>
#include <include/libR3/io/log.h>
#include <stdlib.h>
#include <string.h>

#define R3_MEMORY_HEADER_SIZE sizeof(R3MemoryHeader)

static inline u8 _ip2(u8 align) { return (((align - 1) & align) == 0); }
static inline ptr _afwd(u8 align, ptr mem) { return (ptr)(((uintptr_t)mem + align - 1) & ~(uintptr_t)(align - 1)); }

/* --------------------------------------------------------------------------
    Base Memory API
-------------------------------------------------------------------------- */

R3Result r3FreeMemory(ptr memory) {
    if (!memory) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `FreeMemory` -- invalid pointer\n");
        return R3_RESULT_ERROR;
    }
    
    u16 mdiff = *(u16*)((u8*)memory - sizeof(u16));
    free((ptr)((u8*)memory - mdiff));
    
    return R3_RESULT_SUCCESS;
}

ptr r3AllocMemory(u64 size) {
    if (!size) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `AllocMemory` -- invalid allocation size: %llu\n", size);
        return NULL;
    }

    ptr memory = malloc(sizeof(u16) + (R3_ALIGN - 1) + size);
    if (!memory) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `AllocMemory` -- out of memory\n");
        return NULL;
    }

    ptr amemory = _afwd(R3_ALIGN, (u8*)memory + sizeof(u16));
    *(u16*)((u8*)amemory - sizeof(u16)) = (u16)((u64)amemory - (u64)memory);
    if (r3SetMemory(size, 0, amemory) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_WARN, "Failed `AllocMemory` -- failed to zero memory\n");
    }

    return amemory;
}

ptr r3ReallocMemory(u64 size, ptr memory) {
    if (!size) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReallocMemory` -- invalid re-allocation size\n");
        return NULL;
    } if (!memory) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReallocMemory` -- invalid pointer\n");
        return NULL;
    }

    u16 mdiff = *(u16*)((u8*)memory - sizeof(u16));
    ptr omemory = (ptr)((u8*)memory - mdiff);

    ptr nmemory = realloc(omemory, sizeof(u16) + (R3_ALIGN - 1) + size);
    if (!nmemory) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReallocMemory` -- out of memory\n");
        return NULL;
    }

    ptr amemory = _afwd(R3_ALIGN, (u8*)nmemory + sizeof(u16));
    *(u16*)((u8*)amemory - sizeof(u16)) = (u16)((u64)amemory - (u64)nmemory);

    return amemory;
}

R3Result r3AssignMemory(ptr source, ptr dest) {
    if (!source || !dest) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `AssignMemory` -- invalid source/dest pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    *(u64*)dest = (u64)source;

    return R3_RESULT_SUCCESS;
}

R3Result r3SetMemory(u64 bytes, u8 value, ptr memory) {
    if (!bytes) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `SetMemory` -- invalid byte count: %llu\n", bytes);
        return R3_RESULT_ERROR;
    } if (!memory) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `SetMemory` -- invalid pointer\n");
        return R3_RESULT_ERROR;
    }

    memset(memory, value, bytes);

    return R3_RESULT_SUCCESS;
}

R3Result r3ReadMemory(u64 bytes, ptr source, ptr dest) {
    if (!bytes) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadMemory` -- invalid byte count\n");
        return R3_RESULT_ERROR;
    } if (!source || !dest) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadMemory` -- source/dest invalid pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    memcpy(dest, source, bytes);

    return R3_RESULT_SUCCESS;
}

R3Result r3MoveMemory(u64 bytes, ptr source, ptr dest) {
    if (!bytes) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `MoveMemory` -- invalid byte count\n");
        return R3_RESULT_ERROR;
    } if (!source || !dest) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `MoveMemory` -- invalid source/dest pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    memmove(dest, source, bytes);

    return R3_RESULT_SUCCESS;
}

R3Result r3WriteMemory(u64 bytes, ptr source, ptr dest) {
    if (!bytes) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteMemory` -- invalid byte count\n");
        return R3_RESULT_ERROR;
    } if (!source || !dest) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteMemory` -- invalid source/dest pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    memcpy(dest, source, bytes);
    return R3_RESULT_SUCCESS;
}

R3Result r3CompareMemory(u64 bytes, ptr mem1, ptr mem2) {
    if (!bytes) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `CompareMemory` -- invalid byte count\n");
        return R3_RESULT_ERROR;
    } if (!mem1 || !mem2) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `CompareMemory` -- invalid memory pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    return (!memcmp(mem1, mem2, bytes)) ? R3_RESULT_SUCCESS : R3_RESULT_ERROR;
}


/* --------------------------------------------------------------------------
    Memory Buffer API
-------------------------------------------------------------------------- */

R3Result r3DelMemoryBuffer(R3MemoryBuffer* buffer) {
    if (!buffer || !buffer->data) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelMemoryBuffer` -- invalid `MemoryBuffer` pointer\n");
        return R3_RESULT_ERROR;
    }

    ptr rawBuffer = (ptr)((u8*)buffer->data - R3_MEMORY_HEADER_SIZE);
    buffer->data = NULL;
    r3FreeMemory(rawBuffer);

    return R3_RESULT_SUCCESS;
}

R3Result r3NewMemoryBuffer(u64 size, R3MemoryBuffer* buffer) {
    if (!size) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewMemoryBuffer` -- invalid buffer size\n");
        return R3_RESULT_ERROR;
    } if (!buffer) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewMemoryBuffer` -- invalid `MemoryBuffer` pointer\n");
        return R3_RESULT_ERROR;
    }

    ptr rawBuffer = r3AllocMemory(R3_MEMORY_HEADER_SIZE + size);
    if (!rawBuffer) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewMemoryBuffer` -- out of memory\n");
        return R3_RESULT_ERROR;
    }

    *((R3MemoryHeader*)rawBuffer) = (R3MemoryHeader){
        .rOps = 0,
        .wOps = 0,
        .meta = 0,
        .size = size
    };
    buffer->data = (ptr)((u8*)rawBuffer + R3_MEMORY_HEADER_SIZE);

    return R3_RESULT_SUCCESS;
}

R3Result r3SetMemoryBuffer(i32 value, R3MemoryBuffer* buffer) {
    if (value > (1LL << 32)) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `SetMemoryBuffer` -- i32 overflow: %d\n", value);
        return R3_RESULT_ERROR;
    } if (!buffer || !buffer->data) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `SetMemoryBuffer` -- invalid `MemoryBuffer` pointer\n");
        return R3_RESULT_ERROR;
    }

    u64 size = ((R3MemoryHeader*)((u8*)buffer->data - R3_MEMORY_HEADER_SIZE))->size;
    r3SetMemory(size, value, buffer->data);

    return R3_RESULT_SUCCESS;
}

R3Result r3ReadMemoryBuffer(u64 size, R3MemoryBuffer* source, ptr dest) {
    if (!size || size > ((R3MemoryHeader*)((u8*)source->data - R3_MEMORY_HEADER_SIZE))->size) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadMemoryBuffer` -- invalid buffer size\n");
        return R3_RESULT_ERROR;
    } if (!source || !source->data || !dest) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadMemoryBuffer` -- invalid `MemoryBuffer` pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    r3ReadMemory(size, source->data, dest);
    ((R3MemoryHeader*)((u8*)source->data - R3_MEMORY_HEADER_SIZE))->rOps++;   // increment rOps

    return R3_RESULT_SUCCESS;
}

R3Result r3WriteMemoryBuffer(u64 size, ptr source, R3MemoryBuffer* dest) {
    if (!size || size > ((R3MemoryHeader*)((u8*)dest->data - R3_MEMORY_HEADER_SIZE))->size) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteMemoryBuffer` -- invalid buffer size\n");
        return R3_RESULT_ERROR;
    } if (!source || !dest || !dest->data) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteMemoryBuffer` -- invalid `MemoryBuffer` pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    r3WriteMemory(size, source, dest->data);
    ((R3MemoryHeader*)((u8*)dest->data - R3_MEMORY_HEADER_SIZE))->wOps++; // increment wOps

    return R3_RESULT_SUCCESS;
}

R3Result r3MemoryHeader(R3MemoryHeader* header, R3MemoryBuffer* buffer) {
    if (!header) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `MemoryHeader` -- invalid `MemoryHeader` pointer\n");
        return R3_RESULT_ERROR;
    } if (!buffer) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `MemoryHeader` -- invalid `MemoryBuffer` pointer\n");
        return R3_RESULT_ERROR;
    }

    *header = *(R3MemoryHeader*)((u8*)buffer->data - R3_MEMORY_HEADER_SIZE);

    return R3_RESULT_SUCCESS;
}
