#include <include/r3kt/r3ktlog.h>
#include <include/r3kt/r3ktarr.h>

#include <include/r3kt/r3ktmem.h>


u16 r3_array_field(u8 field, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (field < 0 || field >= ARRAY_HEADER_FIELDS)) || ((ptr_err = (!in)))) {
		if (value_err) printf("[r3ktarr] invalid array header field: %d\n", field);
		if (ptr_err) printf("[r3ktarr] invalid array pointer\n");
		return 0;
	}
	
	switch(field) {
		case MAX_FIELD:
			return *((u16*)((u8*)in->data - (sizeof(u16) * 4)));
		case SIZE_FIELD:
			return *((u16*)((u8*)in->data - (sizeof(u16) * 3)));
		case COUNT_FIELD:
			return *((u16*)((u8*)in->data - (sizeof(u16) * 2)));
		case STRIDE_FIELD:
			return *((u16*)((u8*)in->data - sizeof(u16)));
		default: return 0;
	}
}

/*	DYNAMIC ARRAY	*/
u8 r3_alloc_array(u16 max, u16 stride, Array* out) {
	u8 size_err; u8 ptr_err; u8 alloc_err;
	if ((size_err = (!max || max >= I16_MAX)) || (ptr_err = (!out))) {
		if (size_err) r3_log_fmt(ERROR_LOG, "[r3ktarr] invalid array max: %d\n", max);
		else r3_log(ERROR_LOG, "[r3ktarr] invalid array pointer\n");
		return 0;
	}

	addr raw = r3_alloc((sizeof(u16) * 4) + (max * stride), 8);
	if ((alloc_err = (!raw))) {
		r3_log(ERROR_LOG, "[r3ktarr] array allocation failed");
		return 0;
	}

	u16 count = 0;
	u16 size = max * stride;
	*(u16*)((u8*) raw) = max;
	*(u16*)((u8*) raw + sizeof(u16)) = size;
	*(u16*)((u8*) raw + (sizeof(u16) * 2)) = count;
	*(u16*)((u8*) raw + (sizeof(u16) * 3)) = stride;

	out->data = (addr)((u8*)raw + (sizeof(u16) * 4));
	return 1;
}

u8 r3_dealloc_array(Array* in) {
	if (!in) {
		r3_log(ERROR_LOG, "[r3ktarr] invalid array pointer\n");
		return 0;
	}

	addr raw = (addr)((u8*)in->data - (sizeof(u16) * 4));
	r3_dealloc(raw);

	return 1;
}

u8 r3_read_array(u16 index, addr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log(ERROR_LOG, "[r3ktarr] value error\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktarr] invalid array pointer\n");
		return 0;
	}
	
	u16 max = r3_array_field(MAX_FIELD, in);
	if (index >= max) {
		r3_log_fmt(WARN_LOG, "[r3ktarr] attempted read outside array bounds: (max)%d (index)%d\n", max, index);
		return 0;
	}
	
	u16 stride = r3_array_field(STRIDE_FIELD, in);
	if (!r3_read_memory(stride, value, (addr)((u8*)in->data + (index * stride)))) {
		r3_log(ERROR_LOG, "[r3ktarr] failed array write\n");
		return 0;
	}

	return 1;
}

u8 r3_write_array(u16 index, addr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log(ERROR_LOG, "[r3ktarr] value error\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktarr] invalid array pointer\n");
		return 0;
	}
	
	u16 max = r3_array_field(MAX_FIELD, in);
	if (index >= max) {
		r3_log_fmt(WARN_LOG, "[r3ktarr] attempted write outside array bounds: (max)%d (index)%d\n", max, index);
		return 0;
	}
	
	u16 count = r3_array_field(COUNT_FIELD, in);
	if (index >= count) {
		*(u16*)((u8*)in->data - (sizeof(u16) * 2)) = count + 1;
	}

	u16 stride = r3_array_field(STRIDE_FIELD, in);
	if (!r3_write_memory(stride, value, (addr)((u8*)in->data + (index * stride)))) {
		r3_log(ERROR_LOG, "[r3ktarr] failed array write\n");
		return 0;
	}

	return 1;
}

u8 r3_push_array(addr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log(ERROR_LOG, "[r3ktarr] value error\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktarr] invalid array pointer\n");
		return 0;
	}

	u16 max = r3_array_field(MAX_FIELD, in);
	u16 count = r3_array_field(COUNT_FIELD, in);

	if (count + 1 > max) {
		r3_log_fmt(WARN_LOG, "[r3ktarr] attempted push past array bounds: (max)%d (count)%d\n", max, count);
		return 0;
	}

	if (!r3_write_array(count, value, in)) {
		r3_log(ERROR_LOG, "[r3ktarr] array push failed\n");
		return 0;
	}

	return 1;
}

u8 r3_pull_array(addr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log(ERROR_LOG, "[r3ktarr] value error\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktarr] invalid array pointer\n");
		return 0;
	}

	u16 count = r3_array_field(COUNT_FIELD, in);
	if (count == 0) {
		r3_log_fmt(WARN_LOG, "[r3ktarr] attempted pull from empty array: (count)%d\n", count);
		return 0;
	}

	if (!r3_read_array(count - 1, value, in)) {
		r3_log(ERROR_LOG, "[r3ktarr] array pull failed\n");
		return 0;
	}

	u16 stride = r3_array_field(STRIDE_FIELD, in);
	if (!r3_set_memory(stride, 0, (addr)((u8*)in->data + ((count - 1) * stride)))) {
		r3_log(ERROR_LOG, "[r3ktarr] failed to zero array memory\n");
		return 0;
	}

	*(u16*)((u8*)in->data - (sizeof(u16) * 2)) = count - 1;

	return 1;
}


