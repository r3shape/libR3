#ifndef __R3KTMEM_H__
#define __R3KTMEM_H__

#include <include/r3kt/r3ktdef.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

R3KT_API void r3_dealloc(addr ptr);
R3KT_API addr r3_alloc(size_t size, unsigned int align);
R3KT_API u8 r3_set_memory(u64 size, u8 value, addr ptr);
R3KT_API u8 r3_read_memory(u64 size, addr value, addr ptr);
R3KT_API u8 r3_write_memory(u64 size, addr value, addr ptr);
R3KT_API addr r3_realloc(size_t size, unsigned int align, addr ptr);

#endif	// __R3KTMEM_H__
