#include <r3kit/include/mem/alloc.h>
#include <r3kit/include/io/log.h>
#include <string.h>
#include <stdlib.h>

u8 _ip2(u8 align) {
	return (((align - 1) & align) == 0);
}

ptr _afwd(u8 align, ptr mem) {
	return (ptr)(((uptr)mem + align - 1) & ~(uptr)(align - 1));
}

void r3_mem_dealloc(ptr mem) {
	if (!mem) {
		r3_log_stdout(ERROR_LOG, "[alloc] invalid pointer for deallocation\n");
		return;
	}

	// retrieve ptr diff
	u16 diff = *((u16*)((u8*)mem - sizeof(u16)));
	
	// free from diff ptr
	free((ptr)((u8*)mem - diff));
}

ptr r3_mem_alloc(u64 size, u8 align) {
	if (!size || !_ip2(align)) {
		r3_log_stdoutf(ERROR_LOG, "[alloc] invalid size/alignment for allocation: (size)%llu (align)%d\n", size, align);
		return NULL;
	}

	// allocate 2 bytes for diff
	// allocate align - 1 bytes for alignment op
	ptr o = malloc(sizeof(u16) + (align - 1) + size);
	if (!o) {
		r3_log_stdoutf(ERROR_LOG, "[alloc] failed to allocate memory: (size)%llu (align)%d\n", size, align);
		return NULL;
	}

	// cast to 2 byte type and align shifted ptr
	ptr a = _afwd(align, (u8*)o + sizeof(u16));

	// zero memory
	if (!r3_mem_set(size, 0, a)) {
		r3_log_stdout(WARN_LOG, "[alloc] failed to zero allocated memory\n");
	}

	// store ptr diff
	*((u16*)((u8*)a - sizeof(u16))) = (u16)((uptr)a - (uptr)o);

	return a;
}

ptr r3_mem_realloc(u64 size, u8 align, ptr mem) {
	if (!size || !_ip2(align)) {
		r3_log_stdoutf(ERROR_LOG, "[alloc] invalid size/alignment for reallocation: (size)%llu (align)%d\n", size, align);
		return NULL;
	}; if (!mem) {
		return r3_mem_alloc(size, align);
	}

	// retrieve ptr diff
	u16 diff = *((u16*)((u8*)mem - sizeof(u16)));
	
	// retrieve original ptr
	ptr o = (ptr)((u8*)mem - diff);

	// reallocate from diff ptr
	ptr n = realloc(o, sizeof(u16) + (align - 1) + size);
	if (!n) {
		r3_log_stdoutf(ERROR_LOG, "[alloc] failed to reallocate memory: (size)%llu (align)%d\n", size, align);
		return NULL;
	}

	// cast to 2 byte type and align shifted ptr
	ptr a = _afwd(align, (u8*)n + sizeof(u16));

	// store ptr diff
	*((u16*)((u8*)a - sizeof(u16))) = (u16)((uptr)a - (uptr)n);

	return a;
}

u8 r3_mem_read(u64 size, ptr value, ptr mem) {
	u8 size_err; u8 value_err; u8 ptr_err;
	if ((size_err = (!size)) || (value_err = (!value)) || (ptr_err = (!mem))) {
		if (size_err) r3_log_stdoutf(ERROR_LOG, "[alloc] invalid read size: %llu\n", size);
		if (value_err) r3_log_stdout(ERROR_LOG, "[alloc] value error for read\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[alloc] invalid pointer for read\n");
		return 0;
	}
	
	memcpy(value, mem, size);

	return 1;
}

u8 r3_mem_move(u64 size, ptr value, ptr mem) {
	u8 size_err; u8 value_err; u8 ptr_err;
	if ((size_err = (!size)) || (value_err = (!value)) || (ptr_err = (!mem))) {
		if (size_err) r3_log_stdoutf(ERROR_LOG, "[alloc] invalid write size: %llu\n", size);
		if (value_err) r3_log_stdout(ERROR_LOG, "[alloc] value error for write\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[alloc] invalid pointer for write\n");
		return 0;
	}
	
	memmove(mem, value, size);

	return 1;
}

u8 r3_mem_write(u64 size, ptr value, ptr mem) {
	u8 size_err; u8 value_err; u8 ptr_err;
	if ((size_err = (!size)) || (value_err = (!value)) || (ptr_err = (!mem))) {
		if (size_err) r3_log_stdoutf(ERROR_LOG, "[alloc] invalid write size: %llu\n", size);
		if (value_err) r3_log_stdout(ERROR_LOG, "[alloc] value error for write\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[alloc] invalid pointer for write\n");
		return 0;
	}
	
	memcpy(mem, value, size);

	return 1;
}

u8 r3_mem_set(u64 size, u8 value, ptr mem) {
	u8 size_err; u8 ptr_err;
	if ((size_err = (!size)) || (ptr_err = (!mem))) {
		if (size_err) r3_log_stdoutf(ERROR_LOG, "[alloc] invalid write size: %llu\n", size);
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[alloc] invalid pointer for write\n");
		return 0;
	}
	
	memset(mem, value, size);

	return 1;
}

u8 r3_mem_assign(ptr value, ptr mem) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!mem))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[alloc] value error for assign\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[alloc] invalid pointer for assign\n");
		return 0;
	}
	*(uptr*)mem = *(uptr*)value;
	return 1;
}
