#ifndef __R3MEM_H__
#define __R3MEM_H__

#include <include/r3std/r3def.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

R3STD_API void r3_dealloc(addr ptr);
R3STD_API addr r3_alloc(size_t size, unsigned int align);
R3STD_API addr r3_realloc(size_t size, unsigned int align, addr ptr);

#endif // __R3MEM_H__
