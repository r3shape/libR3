/*
    libR3 — Memory Allocator API
    -----------------------------

    Overview
    ~~~~~~~~
    This module defines the base allocator interface and the built-in
    stack allocator implementation for the libR3 runtime kernel.

    Allocators provide localized memory control for subsystems that
    require deterministic, zero-overhead memory usage. Each allocator
    exposes a consistent API for allocation and freeing, allowing
    interchangeable strategies (stack, arena, slab, etc.) to share a
    common interface.

    Stack Allocator
    ~~~~~~~~~~~~~~
    The stack allocator is the simplest and fastest form of allocation.
    It linearly advances a single offset through a preallocated buffer.
    Individual frees are only valid if performed in strict LIFO order.
    This prevents offset/buffer corruption and guarantees that memory
    usage is always contiguous.

    Design Notes
    ~~~~~~~~~~~~
    * Each allocation prepends a 64-bit size header to the returned block.
    * The allocator maintains its own metadata buffer for bookkeeping.
    * Deallocation validates LIFO order before rolling back the offset.
    * No dynamic OS allocations occur after initialization.

    Changelog
    ~~~~~~~~~
    [@zafflins 10/18/25 v1.0]
    ~ Initial implementation of the stack allocator.
               Added `r3NewStackAllocator()`, `r3DelStackAllocator()`.
               Added `R3Allocator` interface and function pointers.

    [@zafflins 10/18/25 v1.0] 
    Stack Allocator Example
    ```c
        R3Allocator a = {0};
        r3NewStackAllocator(1024, &a);

        f32* v = a.alloc(sizeof(f32) * 4, &a);
        ...
        a.free((ptr*)&v, &a);

        r3DelStackAllocator(&a);
    ```

    Arena Allocator
    ~~~~~~~~~~~~
    The arena allocator provides a linear, monotonic allocation model similar
    to the stack allocator, but without LIFO restrictions. Individual frees are
    ignored — the entire arena is reset in one operation via `free()`.

    This makes it ideal for short-lived or batch allocations where the lifetime
    of all objects is shared (e.g., per-frame systems, parser scratch space).

    Design Notes
    ~~~~~~~~~~~~
    * Allocations advance a single offset through a preallocated buffer.
    * A 64-bit header stores each allocation’s size for debugging or validation.
    * Calling `free()` resets the arena’s offset to zero and reclaims all memory.
    * No per-allocation deallocation occurs; reset is all-or-nothing.
    * No dynamic OS allocations occur after initialization.

    Changelog
    ~~~~~~~~~
    [@zafflins 10/21/25 v1.0]
    ~ Initial implementation of the arena allocator.
    Added `r3NewArenaAllocator()`, `r3DelArenaAllocator()`.
    
    [@zafflins 10/21/25 v1.0]
    Arena Allocator Example
    ~~~~~~~~~~~~
    ```c
        R3Allocator a = {0};
        r3NewArenaAllocator(4096, &a);

        char* temp1 = a.alloc(128, &a);
        char* temp2 = a.alloc(512, &a);
        ...
        
        a.free(NULL, &a);  // resets entire arena and issues a call to `memset` filling the arena with `-1`'s (temp1 and temp2 are still valid)
        r3DelArenaAllocator(&a); (temp1 and temp2 are now invalid)
    ```

    Block Allocator
    ~~~~~~~~~~~~
    The block allocator provides a simple and fast allocation algorithm,
    along with a much more lenient deallocation model, allowing calls to `free()`
    to take place in no specific order, invalidating that block only.

    This is ideal for storage of any number of similar types, of any lifetime. (e.g runtime resources, persistent assets).

    Design Notes
    ~~~~~~~~~~~~
    * Allocations involve simple pointer arithmetic and bitwise operations marking blocks as unused for later reuse.
    * An 8-bit header stores each block's index for debugging or validation.
    * Calling `free()` invalidates the block pointer passed in, filling the block with `-1`'s.
    * No per-allocation deallocation occurs, just simple pointer arithmetic and bitwise operations.
    * No dynamic OS allocation occurs after initialization -- R3 block allocators allocate no more than 64 fixed size blocks.
    
    Changelog
    ~~~~~~~~~
    [@zafflins 10/23/25 v1.0]
    ~ Initial implementation of the block allocator.
    Added `r3NewBlockAllocator()`, `r3DelBlockAllocator()`.

    [@zafflins 10/23/25 v1.0]
    Block Allocator Example
    ~~~~~~~~~~~~
    ```c
        R3Allocator a = {0};
        r3NewBlockAllocator((sizeof(f32) * 3) * 3, &a);    // each block is an array of 3 vec3f
        
        f32** vec3v1 = a.alloc(1, &a);  // allocate a single block of vec3f arrays
        f32** vec3v2 = a.alloc(2, &a);  // allocate two blocks of vec3f arrays
        ...

        a.free(&vec3v2, &a);
        a.free(&vec3v1, &a);
        r3DelBlockAllocator(&a);
    ```
*/

#ifndef __R3_ALLOC_H__
#define __R3_ALLOC_H__

#include <include/libR3/mem/mem.h>

/* --------------------------------------------------------------------------
    Allocator Types
-------------------------------------------------------------------------- */

typedef enum R3AllocatorType {
    R3_STACK_ALLOCATOR,
    R3_ARENA_ALLOCATOR,
    R3_BLOCK_ALLOCATOR,
    R3_ALLOCATOR_TYPES
} R3AllocatorType;

/* --------------------------------------------------------------------------
    Allocator Interface
-------------------------------------------------------------------------- */

typedef struct R3Allocator {
    R3AllocatorType type;     // Allocation strategy
    R3MemoryBuffer meta;      // Per-allocator metadata buffer
    ptr base;                 // Base memory region used for allocations

    ptr (*alloc)(u64 size, struct R3Allocator* alloc);
    R3Result (*free)(ptr data, struct R3Allocator* alloc);
} R3Allocator;

/* --------------------------------------------------------------------------
    Stack Allocator API
-------------------------------------------------------------------------- */

R3_PUBLIC_API R3Result r3NewStackAllocator(u64 size, R3Allocator* alloc);
R3_PUBLIC_API R3Result r3DelStackAllocator(R3Allocator* alloc);

/* --------------------------------------------------------------------------
    Arena Allocator API
-------------------------------------------------------------------------- */

R3_PUBLIC_API R3Result r3NewArenaAllocator(u64 size, R3Allocator* alloc);
R3_PUBLIC_API R3Result r3DelArenaAllocator(R3Allocator* alloc);

/* --------------------------------------------------------------------------
    Block Allocator API
-------------------------------------------------------------------------- */

R3_PUBLIC_API R3Result r3NewBlockAllocator(u32 bstride, R3Allocator* alloc);
R3_PUBLIC_API R3Result r3DelBlockAllocator(R3Allocator* alloc);


#endif /* __R3_ALLOC_H__ */