#include <r3kit/include/mem/alloc.h>
#include <r3kit/include/io/log.h>
#include <r3kit/include/ds/arr.h>

u64 r3_arr_max(Array* in) { return r3_arr_header(in).max; }
u64 r3_arr_count(Array* in) { return r3_arr_header(in).count; }
u64 r3_arr_stride(Array* in) { return r3_arr_header(in).stride; }
ArrayHeader r3_arr_header(Array* in) { return *(ArrayHeader*)((u8*)in->data - sizeof(ArrayHeader)); }


u8 r3_arr_alloc(u64 max, u64 stride, Array* out) {
	u8 size_err; u8 ptr_err;
	if ((size_err = (max >= I64_MAX)) || (ptr_err = (!out || out->data != NULL))) {
		if (size_err) r3_log_stdoutf(ERROR_LOG, "[arr] invalid array max: %d\n", max);
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

	ArrayHeader* header = r3_mem_alloc(sizeof(ArrayHeader) + (max * stride), 8);
	if (!header) {
		r3_log_stdout(ERROR_LOG, "[arr] array allocation failed");
		return 0;
	}

	u64 count = 0;
	u64 size = max * stride;

	out->data = (ptr)((u8*)header + sizeof(ArrayHeader));
	if (!r3_mem_set(size, 0, out->data)) {
		r3_log_stdout(WARN_LOG, "[arr] failed to zero array memory");
	}

	header->max = max;
	header->size = size;
	header->count = count;
	header->stride = stride;

	return 1;
}

u8 r3_arr_realloc(u64 max, Array* in) {
    u8 size_err; u8 ptr_err;
	if ((size_err = (!max)) || (ptr_err = (!in || !in->data))) {
		if (size_err) r3_log_stdout(ERROR_LOG, "[arr] size error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

    u64 stride = R3_HEADER(ArrayHeader, in)->stride;
	
    ptr raw = (ptr)((u8*)in->data - sizeof(ArrayHeader));
    raw = r3_mem_realloc(sizeof(ArrayHeader) + (max * stride), 8, raw);
    if (!raw) {
        r3_log_stdoutf(ERROR_LOG, "[arr] failed to realloc array: (max)%d\n", max);
        return 1;
    }

    ((u64*)raw)[ARRAY_MAX_FIELD] = max;
	in->data = (ptr)((u8*)raw + sizeof(ArrayHeader));

    return 1;
}

u8 r3_arr_dealloc(Array* in) {
	if (!in || !in->data) {
		r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

	ptr raw = (ptr)((u8*)in->data - sizeof(ArrayHeader));
	r3_mem_dealloc(raw);

	return 1;
}


u8 r3_arr_shift_left(u64 index, u64 shift, Array *in) {
    u64 count = R3_HEADER(ArrayHeader, in)->count;
    u64 stride = R3_HEADER(ArrayHeader, in)->stride;

    if (!in || !in->data || shift == 0 || index >= count || shift > index) {
		r3_log_stdout(ERROR_LOG, "[arr] failed to shift array -- invalid params\n");
        return 0;
    }
    
    if (!r3_mem_move(stride * (count - index), (ptr)((u8*)in->data + (stride * index)), (ptr)((u8*)in->data + (stride * (index - shift))))) {
		r3_log_stdout(ERROR_LOG, "[arr] failed to shift array\n");
		return 0;
	}

    if (!r3_mem_set(stride * shift, 0, (ptr)((u8*)in->data + (stride * (count - shift))))) {
		r3_log_stdout(ERROR_LOG, "[arr] failed to zero array after shift\n");
		return 0;
	}
    
    R3_HEADER(ArrayHeader, in)->count -= shift;

    return 1;
}

u8 r3_arr_shift_right(u64 index, u64 shift, Array *in) {
    u64 stride = R3_HEADER(ArrayHeader, in)->stride;
    u64 count = R3_HEADER(ArrayHeader, in)->count;
    u64 max = R3_HEADER(ArrayHeader, in)->max;
    
    if (!in || !in->data || shift == 0 || index >= count || shift >= max) {
        r3_log_stdout(ERROR_LOG, "[arr] failed to shift array -- invalid params\n");
        return 0;
    }

	// guard against OOB shift
    if ((max - count) >= shift) {
        if (!r3_mem_move(stride * (count - index), (ptr)((u8*)in->data + (stride * index)), (ptr)((u8*)in->data + (stride * (index + shift))))) {
        	r3_log_stdout(ERROR_LOG, "[arr] failed to right shift array\n");
        	return 0;
        }
    } else { // excess elements overwritten/'dropped'
		if (!r3_mem_move(stride * ((count - index) - (shift - (max - count))), (ptr)((u8*)in->data + (stride * index)), (ptr)((u8*)in->data + (stride * (index + shift))))) {
			r3_log_stdout(ERROR_LOG, "[arr] failed to right shift array\n");
        	return 0;
        }
    }
    
    if (!r3_mem_set(stride * shift, 0, (ptr)((u8*)in->data + (stride * index)))) {
		r3_log_stdout(ERROR_LOG, "[arr] failed to zero array after right shift\n");
		return 0;
	}
	
	R3_HEADER(ArrayHeader, in)->count = MIN(max, count  + shift);

    return 1;
}


u8 r3_arr_push(ptr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[arr] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

	u64 max = R3_HEADER(ArrayHeader, in)->max;
	u64 count = R3_HEADER(ArrayHeader, in)->count;

	if (count + 1 > max) {
		if (!r3_arr_realloc(max * 2, in)) {
			r3_log_stdout(ERROR_LOG, "[arr] array push failed on realloc\n");
		}
		
		max = R3_HEADER(ArrayHeader, in)->max;
		r3_log_stdoutf(WARN_LOG, "[arr] reallocated (max * 2) after attempted push past array bounds: (max)%d\n", max);
	}

	if (!r3_arr_write(count, value, in)) {
		r3_log_stdout(ERROR_LOG, "[arr] array push failed\n");
		return 0;
	}

	return 1;
}

u8 r3_arr_pop(ptr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[arr] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

	u64 count = R3_HEADER(ArrayHeader, in)->count;
	if (count == 0) {
		r3_log_stdoutf(WARN_LOG, "[arr] attempted pull from empty array: (count)%d\n", count);
		return 0;
	}

	if (!r3_arr_read(count - 1, value, in)) {
		r3_log_stdout(ERROR_LOG, "[arr] array pull failed\n");
		return 0;
	}

	u64 stride = R3_HEADER(ArrayHeader, in)->stride;
	if (!r3_mem_set(stride, 0, (ptr)((u8*)in->data + ((count - 1) * stride)))) {
		r3_log_stdout(ERROR_LOG, "[arr] failed to zero array memory\n");
		return 0;
	}

	R3_HEADER(ArrayHeader, in)->count--;

	return 1;
}

u8 r3_arr_pull(u64 index, ptr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[arr] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

	u64 count = R3_HEADER(ArrayHeader, in)->count;
	if (count == 0) {
		r3_log_stdoutf(WARN_LOG, "[arr] attempted pull from empty array: (count)%d\n", count);
		return 0;
	}

	u64 max = R3_HEADER(ArrayHeader, in)->max;
	if (index >= max) {
		r3_log_stdoutf(WARN_LOG, "[arr] attempted pull past array bounds: (max)%d (index)%d\n", max, index);
		return 0;
	}

	if (!r3_arr_read(index, value, in)) {
		r3_log_stdout(ERROR_LOG, "[arr] array pull failed\n");
		return 0;
	}

	// left shift
	u64 stride = R3_HEADER(ArrayHeader, in)->stride;
	r3_mem_move(stride * (count - (count - (index + 1))), (ptr)((u8*)in->data + (stride * (index + 1))), (ptr)((u8*)in->data + (stride * index)));
    
	if (!r3_mem_set(stride, 0, (ptr)((u8*)in->data + (stride * (count - 1))))) {
		r3_log_stdout(ERROR_LOG, "[arr] failed to zero array\n");
		return 0;
	}

	R3_HEADER(ArrayHeader, in)->count--;

	return 1;
}



u8 r3_arr_read(u64 index, ptr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdoutf(ERROR_LOG, "[arr] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}
	
	u64 max = R3_HEADER(ArrayHeader, in)->max;
	if (index >= max) {
		r3_log_stdoutf(WARN_LOG, "[arr] attempted read past array bounds: (max)%d (index)%d\n", max, index);
		return 0;
	}
	
	u64 stride = R3_HEADER(ArrayHeader, in)->stride;
	if (!r3_mem_read(stride, value, (ptr)((u8*)in->data + (index * stride)))) {
		r3_log_stdout(ERROR_LOG, "[arr] failed array write\n");
		return 0;
	}

	return 1;
}

u8 r3_arr_assign(u64 index, ptr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[arr] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}
	
	u64 max = R3_HEADER(ArrayHeader, in)->max;
	if (index >= max) {
		r3_log_stdoutf(WARN_LOG, "[arr] attempted write past array bounds: (max)%d (index)%d\n", max, index);
		return 0;
	}
	
	u64 count = R3_HEADER(ArrayHeader, in)->count;
	if (index >= count) {
		R3_HEADER(ArrayHeader, in)->count++;
	}
	
	u64 stride = R3_HEADER(ArrayHeader, in)->stride;
	*(uptr*)((u8*)in->data + (index * stride)) = (uptr)value;
	
	return 1;
}

u8 r3_arr_write(u64 index, ptr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[arr] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}
	
	u64 max = R3_HEADER(ArrayHeader, in)->max;
	if (index >= max) {
		r3_arr_realloc(index + 8, in);

		max = R3_HEADER(ArrayHeader, in)->max;
		r3_log_stdoutf(WARN_LOG, "[arr] reallocated (index + 8) after attempted write past array bounds: (max)%d (index)%d\n", max, index);
	}
	
	u64 count = R3_HEADER(ArrayHeader, in)->count;
	if (index >= count) {
		R3_HEADER(ArrayHeader, in)->count++;
	}
	
	u64 stride = R3_HEADER(ArrayHeader, in)->stride;
	if (!r3_mem_write(stride, value, (ptr)((u8*)in->data + (index * stride)))) {
		r3_log_stdout(ERROR_LOG, "[arr] failed array write\n");
		return 0;
	}
	
	return 1;
}
