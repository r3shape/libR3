/*
    libR3 — Base Memory API
    ------------------------

    Overview
    ~~~~~~~~
    This module defines the low-level memory primitives for libR3.
    It provides a lightweight abstraction over raw memory regions
    with automatic tracking of operational statistics and metadata.

    Each memory buffer begins with an internal header (`R3MemoryHeader`)
    describing its size, metadata flags, and number of read/write operations.
    This enables introspection and instrumentation across the entire runtime,
    while maintaining a clean, low-level allocation model.

    Memory Model
    ~~~~~~~~~~~~
    A memory buffer in libR3 is represented as:

        [ `R3MemoryHeader` | User Data ... ]

    The header occupies 32 bytes (4 × 64-bit fields) and is directly
    accessible via `r3MemoryHeader()`. All buffer operations update
    the header counters automatically (rOps, wOps) and validate
    access bounds.

    Design Notes
    ~~~~~~~~~~~~
    • Headers are stored inline, preceding the data region.
    • The API abstracts allocation, reads, and writes, not OS mapping.
    • Metadata (`meta`) may be used by higher-level systems to tag or
      flag buffers for internal runtime tracking.

    Changelog
    ~~~~~~~~~
    [@zafflins 10/18/25 v1.0]
    ~ Initial implementation of the base memory API.
      Added `r3NewMemoryBuffer()`, `r3DelMemoryBuffer()`,
      `r3ReadMemoryBuffer()`, `r3WriteMemoryBuffer()`,
      and `r3MemoryHeader()`.

    [@zafflins 10/18/25 v1.0]
    Memory Buffer Example
    ```c
        R3MemoryBuffer buf = {0};
        r3NewMemoryBuffer(sizeof(f32) * 3, &buf);

        f32 value = 420.0f;
        r3WriteMemoryBuffer(sizeof(f32), &value, &buf);

        f32 read = 0.0f;
        r3ReadMemoryBuffer(sizeof(f32), &buf, &read);

        r3DelMemoryBuffer(&buf);
    ```
*/

#ifndef __R3_MEM_BASE_H__
#define __R3_MEM_BASE_H__

#include <include/libR3/r3def.h>

/* --------------------------------------------------------------------------
    Memory Structures
-------------------------------------------------------------------------- */

typedef struct R3MemoryHeader {
    u64 size;   // size of the memory buffer in bytes
    u64 meta;   // metadata bitmask for internal runtime tagging
    u64 rOps;   // total number of read operations performed
    u64 wOps;   // total number of write operations performed
} R3MemoryHeader;

typedef struct R3MemoryBuffer {
    ptr data;   // pointer to the start of user data
} R3MemoryBuffer;

/* --------------------------------------------------------------------------
    Base Memory API
-------------------------------------------------------------------------- */

R3_PUBLIC_API ptr r3AllocMemory(u64 size);
R3_PUBLIC_API R3Result r3FreeMemory(ptr memory);
R3_PUBLIC_API ptr r3ReallocMemory(u64 size, ptr memory);

R3_PUBLIC_API R3Result r3AssignMemory(ptr source, ptr dest);
R3_PUBLIC_API R3Result r3SetMemory(u64 bytes, u8 value, ptr memory);
R3_PUBLIC_API R3Result r3ReadMemory(u64 bytes, ptr source, ptr dest);
R3_PUBLIC_API R3Result r3MoveMemory(u64 bytes, ptr source, ptr dest);
R3_PUBLIC_API R3Result r3WriteMemory(u64 bytes, ptr source, ptr dest);
R3_PUBLIC_API R3Result r3CompareMemory(u64 bytes, ptr mem1, ptr mem2);

/* --------------------------------------------------------------------------
    Memory Buffer API
-------------------------------------------------------------------------- */

R3_PUBLIC_API R3Result r3NewMemoryBuffer(u64 size, R3MemoryBuffer* buffer);
R3_PUBLIC_API R3Result r3DelMemoryBuffer(R3MemoryBuffer* buffer);

R3_PUBLIC_API R3Result r3SetMemoryBuffer(i32 value, R3MemoryBuffer* buffer);
R3_PUBLIC_API R3Result r3ReadMemoryBuffer(u64 size, R3MemoryBuffer* source, ptr dest);
R3_PUBLIC_API R3Result r3WriteMemoryBuffer(u64 size, ptr source, R3MemoryBuffer* dest);

R3_PUBLIC_API R3Result r3MemoryHeader(R3MemoryHeader* header, R3MemoryBuffer* buffer);

#endif /* __R3_MEM_BASE_H__ */
