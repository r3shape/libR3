#include <include/r3kt/r3ktmem.h>
#include <include/r3kt/r3ktlog.h>

u8 _ip2(u32 align) {
	return (((align - 1) & align) == 0);
}

addr _afwd(u32 align, addr ptr) {
	return (addr)(((uptr)ptr + align - 1) & ~(uptr)(align - 1));
}

void r3_dealloc(addr ptr) {
	if (!ptr) {
		r3_log(ERROR_LOG, "[r3ktmem] invalid pointer for deallocation\n");
		return;
	}

	// retrieve ptr diff
	u16 diff = *((u16*)((u8*)ptr - sizeof(u16)));
	
	// free from diff addr
	free((addr)((u8*)ptr - diff));
}

addr r3_alloc(u64 size, u32 align) {
	if (!size || !_ip2(align)) {
		r3_log_fmt(ERROR_LOG, "[r3ktmem] invalid size/alignment for allocation: (size)%llu (align)%d\n", size, align);
		return NULL;
	}

	// allocate 2 bytes for diff
	// allocate align - 1 bytes for alignment op
	addr o = malloc(sizeof(u16) + (align - 1) + size);
	if (!o) {
		r3_log_fmt(ERROR_LOG, "[r3ktmem] failed to allocate memory: (size)%llu (align)%d\n", size, align);
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
		r3_log_fmt(ERROR_LOG, "[r3ktmem] invalid size/alignment for reallocation: (size)%llu (align)%d\n", size, align);
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
		r3_log_fmt(ERROR_LOG, "[r3ktmem] failed to reallocate memory: (size)%llu (align)%d\n", size, align);
		return NULL;
	}

	// cast to 2 byte type and align shifted ptr
	addr a = _afwd(align, (u8*)n + sizeof(u16));

	// store ptr diff
	*((u16*)((u8*)a - sizeof(u16))) = (u16)((uptr)a - (uptr)n);

	return a;
}

u8 r3_read_memory(u64 size, addr value, addr ptr) {
	u8 size_err; u8 value_err; u8 ptr_err;
	if ((size_err = (!size)) || (value_err = (!value)) || (ptr_err = (!ptr))) {
		if (size_err) r3_log_fmt(ERROR_LOG, "[r3ktmem] invalid read size: %llu\n", size);
		if (value_err) r3_log(ERROR_LOG, "[r3ktmem] value error for read\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktmem] ivalid pointer for read\n");
		return 0;
	}
	
	memcpy(value, ptr, size);

	return 1;
}

u8 r3_write_memory(u64 size, addr value, addr ptr) {
	u8 size_err; u8 value_err; u8 ptr_err;
	if ((size_err = (!size)) || (value_err = (!value)) || (ptr_err = (!ptr))) {
		if (size_err) r3_log_fmt(ERROR_LOG, "[r3ktmem] invalid write size: %llu\n", size);
		if (value_err) r3_log(ERROR_LOG, "[r3ktmem] value error for write\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktmem] ivalid pointer for write\n");
		return 0;
	}
	
	memcpy(ptr, value, size);

	return 1;
}

u8 r3_set_memory(u64 size, u8 value, addr ptr) {
	u8 size_err; u8 ptr_err;
	if ((size_err = (!size)) || (ptr_err = (!ptr))) {
		if (size_err) r3_log_fmt(ERROR_LOG, "[r3ktmem] invalid write size: %llu\n", size);
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktmem] ivalid pointer for write\n");
		return 0;
	}
	
	memset(ptr, value, size);

	return 1;
}


