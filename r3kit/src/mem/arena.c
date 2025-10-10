#include <r3kit/include/io/log.h>
#include <r3kit/include/mem/alloc.h>
#include <r3kit/include/mem/arena.h>

u8 r3_arena_reset(Arena_Allocator* alloc) {
    u8 ptr_err; u8 size_err;
    if ((ptr_err = (!alloc || !alloc->buffer)) || (size_err = (!alloc->size))) {
        if (ptr_err) r3_log_stdout(ERROR_LOG, "[arena] invalid arena allocator pointer\n");
        if (size_err) r3_log_stdoutf(ERROR_LOG, "[arena] failed to reset arena; invalid size: (size)%d\n", alloc->size);
        return 0;
    }

    if (!r3_mem_set(alloc->size, 0, alloc->buffer)) {
        r3_log_stdout(WARN_LOG, "[arena] failed to zero arena memory on reset\n");
    }

    alloc->offset = 0;
    return 1;
}

ptr r3_arena_alloc(u64 size, Arena_Allocator* alloc) {
    if (!alloc) {
        r3_log_stdout(ERROR_LOG, "[arena] invalid arena pointer\n");
        return NULL;
    }
    
    if (!size) {
        r3_log_stdout(ERROR_LOG, "[arena] invalid arena size\n");
        return NULL;
    }

    if (!alloc->buffer || !alloc->size) {
        if ((alloc->buffer = r3_mem_alloc(size, 8)) && !alloc->buffer) {
            r3_log_stdout(ERROR_LOG, "[arena] failed to allocate arena buffer\n");;
            return NULL;
        } else {
            alloc->size = size;
            alloc->offset = 0;
            return NULL;
        }
    }

    if (alloc->offset + size > alloc->size) {
        r3_log_stdout(ERROR_LOG, "[arena] arena out of memory\n");
        return NULL;
    } else {
        ptr buffer = (ptr)((u8*)alloc->buffer + alloc->offset);
        alloc->offset += size;
        return buffer;
    }
}

// TODO: implement arena rewinds to avoid dealloc directly
u8 r3_arena_dealloc(u64 size, ptr value, Arena_Allocator* alloc) {
    // NULL value, assume allocator deallocation
    if (!size || !value) {
        r3_mem_dealloc(alloc->buffer);
        alloc->offset = 0;
        alloc->size = 0;
        return 1;
    }

    u8 ptr_err; u8 buf_err; u8 size_err;
    if ((ptr_err = (!alloc)) || (buf_err = (!alloc->buffer)) || (size_err = (!alloc->size))) {
        if (ptr_err) {
            r3_log_stdout(ERROR_LOG, "[arena] invalid arena allocator pointer\n");
            return 0;
        }
        if (size_err) {
            r3_log_stdoutf(ERROR_LOG, "[arena] failed to reset arena; invalid size: (size)%d\n", alloc->size);
            return 0;
        }
        if (buf_err) {
            r3_log_stdout(ERROR_LOG, "[arena] invalid arena allocator pointer\n");
            return 0;
        }
    }

    if (!r3_mem_set(size, 0, value)) {
        r3_log_stdout(WARN_LOG, "[arena] failed to zero deallocated arena memory\n");
    }
    alloc->offset -= size;

    return 1;
}
