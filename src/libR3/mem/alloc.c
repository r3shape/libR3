#include <include/libR3/mem/alloc.h>
#include <include/libR3/ds/array.h>
#include <include/libR3/io/log.h>

typedef struct R3StackAllocatorMeta { u64 offset; } R3StackAllocatorMeta;
typedef struct R3ArenaAllocatorMeta { u64 offset; } R3ArenaAllocatorMeta;
typedef struct R3BlockAllocatorMeta { u64 bmask; u64 bstride; } R3BlockAllocatorMeta;

/* --------------------------------------------------------------------------
    Stack Allocator API
-------------------------------------------------------------------------- */

static inline ptr r3StackAlloc(u64 size, R3Allocator* alloc) {
    if (!alloc || alloc->type != R3_STACK_ALLOCATOR) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `StackAlloc` -- invalid allocator pointer: %p\n", alloc);
        return NULL;
    }
    
    R3MemoryHeader h = {0};
    r3MemoryHeader(&h, &alloc->meta);
    
    if (!size || size + sizeof(u64) > h.size) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `StackAlloc` -- invalid size: %llu\n", size);
        return NULL;
    }
    
    ptr result = ((u8*)alloc->base + (sizeof(u64) + ((R3StackAllocatorMeta*)alloc->meta.data)->offset));
    *(u64*)((u8*)result - sizeof(u64)) = size;

    ((R3StackAllocatorMeta*)alloc->meta.data)->offset += size + sizeof(u64);

    return result;
}

static inline R3Result r3StackFree(ptr data, R3Allocator* alloc) {
    if (!data) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `StackFree` -- invalid `data` pointer: (p)\n", data);
        return R3_RESULT_ERROR;
    } if (!alloc || alloc->type != R3_STACK_ALLOCATOR) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `StackFree` -- invalid allocator pointer: %p\n", alloc);
        return R3_RESULT_ERROR;
    }
    
    u64 size = *(u64*)((u8*)(data) - sizeof(u64));
    u64 exOffset = ((u8*)(data) + size) - (u8*)alloc->base; // 'dist' from base ptr

    if (((R3StackAllocatorMeta*)alloc->meta.data)->offset != exOffset) {
        r3LogStdOutF(R3_LOG_WARN, "Skipped call to `StackFree` -- non-LIFO free: %p, (offset)%llu (expected)%llu\n", data, ((R3StackAllocatorMeta*)alloc->meta.data)->offset, exOffset);
        return R3_RESULT_ERROR;
    }

    ((R3StackAllocatorMeta*)alloc->meta.data)->offset -= (size + sizeof(u64));
    data = NULL;
    
    return R3_RESULT_SUCCESS;
}

R3Result r3DelStackAllocator(R3Allocator* alloc) {
    if (!alloc || !alloc->base || alloc->type != R3_STACK_ALLOCATOR) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `DelStackAllocator` -- invalid allocator pointer: (type)%d %p\n", alloc->type, alloc);
        return R3_RESULT_ERROR;
    }

    r3DelMemoryBuffer(&alloc->meta);
    alloc->alloc = NULL;
    alloc->free = NULL;
    alloc->base = NULL;

    return R3_RESULT_SUCCESS;
}

R3Result r3NewStackAllocator(u64 size, R3Allocator* alloc) {
    if (!size) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewStackAllocator` -- invalid allocator size: %llu\n", size);
        return R3_RESULT_ERROR;
    } if (!alloc) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewStackAllocator` -- invalid allocator pointer: %p\n", alloc);
        return R3_RESULT_ERROR;
    }

    if (!(r3NewMemoryBuffer(sizeof(R3StackAllocatorMeta) + size, &alloc->meta) == R3_RESULT_SUCCESS)) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewStackAllocator` -- allocator meta buffer allocation failed: (meta)%p\n", alloc->meta);
        return R3_RESULT_ERROR;
    }

    *((R3StackAllocatorMeta*)alloc->meta.data) = (R3StackAllocatorMeta){ .offset = 0 };
    alloc->base = (ptr)((u8*)alloc->meta.data + sizeof(R3StackAllocatorMeta));
    alloc->type = R3_STACK_ALLOCATOR;
    alloc->alloc = r3StackAlloc;
    alloc->free = r3StackFree;

    return R3_RESULT_SUCCESS;
}


/* --------------------------------------------------------------------------
    Arena Allocator API
-------------------------------------------------------------------------- */

static inline ptr r3ArenaAlloc(u64 size, R3Allocator* alloc) {
    if (!alloc || alloc->type != R3_ARENA_ALLOCATOR) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `ArenaAlloc` -- invalid allocator pointer: %p\n", alloc);
        return NULL;
    }

    R3MemoryHeader h = {0};
    r3MemoryHeader(&h, &alloc->meta);
    
    if (!size || size + sizeof(u64) > h.size) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `ArenaAlloc` -- invalid size: %llu\n", size);
        return NULL;
    }

    ptr result = ((u8*)alloc->base + (sizeof(u64) + ((R3ArenaAllocatorMeta*)alloc->meta.data)->offset));
    *(u64*)((u8*)result - sizeof(u64)) = size;

    ((R3ArenaAllocatorMeta*)alloc->meta.data)->offset += size + sizeof(u64);

    return result;
}

static inline R3Result r3ArenaFree(ptr memory, R3Allocator* alloc) {
    if (!alloc || alloc->type != R3_ARENA_ALLOCATOR) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `ArenaFree` -- invalid allocator pointer: %p\n", alloc);
        return R3_RESULT_ERROR;
    }
    
    if (!memory) {
        r3SetMemoryBuffer(-1, &alloc->meta);
        ((R3ArenaAllocatorMeta*)alloc->meta.data)->offset = 0;
    } else {
        u64 size = *(u64*)((u8*)memory - sizeof(u64));
        r3SetMemory(size, 0, memory);
        ((R3ArenaAllocatorMeta*)alloc->meta.data)->offset -= size;
    }

    return R3_RESULT_SUCCESS;
}

R3Result r3DelArenaAllocator(R3Allocator* alloc) {
    if (!alloc || !alloc->base || alloc->type != R3_ARENA_ALLOCATOR) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `DelArenaAllocator` -- invalid allocator pointer: (type)%d %p\n", alloc->type, alloc);
        return R3_RESULT_ERROR;
    }

    r3DelMemoryBuffer(&alloc->meta);
    alloc->alloc = NULL;
    alloc->free = NULL;
    alloc->base = NULL;

    return R3_RESULT_SUCCESS;
}

R3Result r3NewArenaAllocator(u64 size, R3Allocator* alloc) {
    if (!size) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewArenaAllocator` -- invalid allocator size: %llu\n", size);
        return R3_RESULT_ERROR;
    } if (!alloc) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewArenaAllocator` -- invalid allocator pointer: %p\n", alloc);
        return R3_RESULT_ERROR;
    }

    if (!(r3NewMemoryBuffer(sizeof(R3ArenaAllocatorMeta) + size, &alloc->meta) == R3_RESULT_SUCCESS)) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewStackAllocator` -- allocator meta buffer allocation failed: (meta)%p\n", alloc->meta);
        return R3_RESULT_ERROR;
    }

    *((R3ArenaAllocatorMeta*)alloc->meta.data) = (R3ArenaAllocatorMeta){ .offset = 0 };
    alloc->base = (ptr)((u8*)alloc->meta.data + sizeof(R3ArenaAllocatorMeta));
    alloc->type = R3_ARENA_ALLOCATOR;
    alloc->alloc = r3ArenaAlloc;
    alloc->free = r3ArenaFree;

    return R3_RESULT_SUCCESS;
}


/* --------------------------------------------------------------------------
    Block Allocator API
-------------------------------------------------------------------------- */

static inline ptr r3BlockAlloc(u64 blocks, R3Allocator* alloc) {
     if (!alloc || alloc->type != R3_BLOCK_ALLOCATOR) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `BlockAlloc` -- invalid allocator pointer: %p\n", alloc);
        return NULL;
    } if (!blocks || blocks > 64) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `BlockAlloc` -- invalid block count: %llu\n", blocks);
        return NULL;
    }

    u8 bindex = 0;
    u64 bmask = ((R3BlockAllocatorMeta*)alloc->meta.data)->bmask;
    FOR(u8, b, 0, 64, 1) if ((bmask & (1 << b)) == 0) { bindex = b; break; }

    u64 bstride = ((R3BlockAllocatorMeta*)alloc->meta.data)->bstride;
    ((R3BlockAllocatorMeta*)alloc->meta.data)->bmask ^= (1 << bindex);

    ptr block = (ptr)((u8*)alloc->base + (bindex * (sizeof(u8) + bstride)));
    *(u8*)block = bindex;

    return (ptr)((u8*)block + sizeof(u8));
}

static inline R3Result r3BlockFree(ptr block, R3Allocator* alloc) {
    if (!alloc || alloc->type != R3_BLOCK_ALLOCATOR) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `BlockFree` -- invalid allocator pointer: %p\n", alloc);
        return NULL;
    } if (!block) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `BlockFree` -- invalid block pointer: %p\n", block);
        return NULL;
    }

    u8 bindex = *((u8*)block - sizeof(u8));
    ((R3BlockAllocatorMeta*)alloc->meta.data)->bmask ^= (1 << bindex);
    block = NULL;
    
    return R3_RESULT_SUCCESS;
}

R3Result r3DelBlockAllocator(R3Allocator* alloc) {
    if (!alloc || !alloc->base || alloc->type != R3_BLOCK_ALLOCATOR) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `DelBlockAllocator` -- invalid allocator pointer: (type)%d %p\n", alloc->type, alloc);
        return R3_RESULT_ERROR;
    }

    r3DelMemoryBuffer(&alloc->meta);
    alloc->alloc = NULL;
    alloc->free = NULL;
    alloc->base = NULL;

    return R3_RESULT_SUCCESS;
}

R3Result r3NewBlockAllocator(u32 bstride, R3Allocator* alloc) {
    if (!bstride) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewSlabAllocator` -- invalid allocator block stride: %d\n", bstride);
        return R3_RESULT_ERROR;
    } if (!alloc) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewSlabAllocator` -- invalid allocator pointer: %p\n", alloc);
        return R3_RESULT_ERROR;
    }

    if (!(r3NewMemoryBuffer(sizeof(R3BlockAllocatorMeta) + (64 * (sizeof(u8) + bstride)), &alloc->meta) == R3_RESULT_SUCCESS)) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewSlabAllocator` -- allocator meta buffer allocation failed: (meta)%p\n", alloc->meta);
        return R3_RESULT_ERROR;
    }

    *((R3BlockAllocatorMeta*)alloc->meta.data) = (R3BlockAllocatorMeta){
        .bmask = 0,
        .bstride = bstride
    };
    alloc->base = (ptr)((u8*)alloc->meta.data + sizeof(R3BlockAllocatorMeta));
    alloc->type = R3_BLOCK_ALLOCATOR;
    alloc->alloc = r3BlockAlloc;
    alloc->free = r3BlockFree;

    return R3_RESULT_SUCCESS;
}
