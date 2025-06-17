#include <include/r3std/r3mem.h>

u8 _ip2(u32 align) {
	return (((align - 1) & align) == 0);
}

addr _afwd(u32 align, addr ptr) {
	return (addr)(((uptr)ptr + align - 1) & ~(uptr)(align - 1));
}

void r3_dealloc(addr ptr) {
	if (!ptr) {
		printf("[r3mem' invalid ptr\n");
		return;
	}

	// retrieve ptr diff
	u16 diff = *((u16*)((u8*)ptr - sizeof(u16)));
	
	// free from diff addr
	free((addr)((u8*)ptr - diff));
}

addr r3_alloc(u64 size, u32 align) {
	if (!size || !_ip2(align)) {
		printf("[r3mem] invalid size/alignment for alloc: (size)%llu (align)%d\n", size, align);
		return NULL;
	}

	// allocate 2 bytes for diff
	// allocate align - 1 bytes for alignment op
	addr o = malloc(sizeof(u16) + (align - 1) + size);
	if (!o) {
		printf("[r3mem] failed to allocate memory: (size)%llu (align)%d\n", size, align);
		return NULL;
	}

	// cast to 2 byte type and align shifted ptr
	addr a = _afwd(align, (u8*)o + sizeof(u16));

	// store ptr diff
	*((u16*)((u8*)a - sizeof(u16))) = (u16)((uptr)a - (uptr)o);

	return a;
}

addr r3_realloc(u64 size, u32 align, addr ptr) {
	if (!size || !_ip2(align)) {
		printf("[r3mem] invalid size/alignment for realloc: (size)%llu (align)%d\n", size, align);
		return NULL;
	}; if (!ptr) {
		return r3_alloc(size, align);
	}

	// retrieve ptr diff
	u16 diff = *((u16*)((u8*)ptr - sizeof(u16)));
	
	// retrieve original ptr
	addr o = (addr)((u8*)ptr - diff);

	// reallocate from diff addr
	addr n = realloc(o, sizeof(u16) + (align - 1) + size);
	if (!n) {
		printf("[r3mem] failed to allocate memory: (size)%llu (align)%d\n", size, align);
		return NULL;
	}

	// cast to 2 byte type and align shifted ptr
	addr a = _afwd(align, (u8*)n + sizeof(u16));

	// store ptr diff
	*((u16*)((u8*)a - sizeof(u16))) = (u16)((uptr)a - (uptr)n);

	return a;
}

