#include <r3kit/include/mem/alloc.h>
#include <r3kit/include/io/log.h>
#include <r3kit/include/ds/arr.h>

static inline ptr r3_get_header(u8 fields, u16 stride, ptr data) {
    return (ptr)((u8*)data - (stride * fields));
}

static inline u16 r3_get_field(u8 fields, u8 field, u16 stride, ptr data) {
    return ((u16*)r3_get_header(fields, stride, data))[field];
}

static inline none r3_set_field(u8 fields, u8 field, u16 stride, u16 value, ptr data) {
    *(u16*)((u8*)data - (stride * (fields - field))) = value;
}


u16 r3_arr_field(u8 field, Array* in) {
	return r3_get_field(ARRAY_HEADER_FIELDS, field, sizeof(u16), in->data);
}

u8 r3_arr_alloc(u16 max, u16 stride, Array* out) {
	u8 size_err; u8 ptr_err;
	if ((size_err = (max >= I16_MAX)) || (ptr_err = (!out || out->data != NULL))) {
		if (size_err) r3_log_stdoutf(ERROR_LOG, "[arr] invalid array max: %d\n", max);
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

	ptr raw = r3_mem_alloc((sizeof(u16) * 4) + (max * stride), 8);
	if (!raw) {
		r3_log_stdout(ERROR_LOG, "[arr] array allocation failed");
		return 0;
	}

	u16 count = 0;
	u16 size = max * stride;

	out->data = (ptr)((u8*)raw + (sizeof(u16) * 4));
	if (!r3_mem_set(size, 0, out->data)) {
		r3_log_stdout(WARN_LOG, "[arr] failed to zero array memory");
	}

	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_MAX_FIELD, sizeof(u16), max, out->data);
	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_SIZE_FIELD, sizeof(u16), size, out->data);
	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_COUNT_FIELD, sizeof(u16), count, out->data);
	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_STRIDE_FIELD, sizeof(u16), stride, out->data);

	return 1;
}

u8 r3_arr_realloc(u16 max, Array* in) {
    u8 size_err; u8 ptr_err;
	if ((size_err = (!max)) || (ptr_err = (!in || !in->data))) {
		if (size_err) r3_log_stdout(ERROR_LOG, "[arr] size error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

    u16 stride = r3_arr_field(ARRAY_STRIDE_FIELD, in);
	
    ptr raw = (ptr)((u8*)in->data - (sizeof(u16) * 4));
    raw = r3_mem_realloc((sizeof(u16) * 4) + (max * stride), 8, raw);
    if (!raw) {
        r3_log_stdoutf(ERROR_LOG, "[arr] failed to realloc array: (max)%d\n", max);
        return 1;
    }

    ((u16*)raw)[ARRAY_MAX_FIELD] = max;
	in->data = (ptr)((u8*)raw + (sizeof(u16) * 4));

    return 1;
}

u8 r3_arr_dealloc(Array* in) {
	if (!in || !in->data) {
		r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

	ptr raw = (ptr)((u8*)in->data - (sizeof(u16) * 4));
	r3_mem_dealloc(raw);

	return 1;
}


u8 r3_arr_shift_left(u16 index, u16 shift, Array *in) {
    u16 count = r3_arr_field(ARRAY_COUNT_FIELD, in);
    u16 stride = r3_arr_field(ARRAY_STRIDE_FIELD, in);

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
    
    r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_COUNT_FIELD, sizeof(u16), count - shift, in->data);

    return 1;
}

u8 r3_arr_shift_right(u16 index, u16 shift, Array *in) {
    u16 count = r3_arr_field(ARRAY_COUNT_FIELD, in);
    u16 stride = r3_arr_field(ARRAY_STRIDE_FIELD, in);
    u16 max = r3_arr_field(ARRAY_MAX_FIELD, in);
    
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
	
	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_COUNT_FIELD, sizeof(u16), MIN(max, count  + shift), in->data);

    return 1;
}


u8 r3_arr_push(ptr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[arr] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

	u16 max = r3_arr_field(ARRAY_MAX_FIELD, in);
	u16 count = r3_arr_field(ARRAY_COUNT_FIELD, in);

	if (count + 1 > max) {
		if (!r3_arr_realloc(max * 2, in)) {
			r3_log_stdout(ERROR_LOG, "[arr] array push failed on realloc\n");
		}
		
		max = r3_arr_field(ARRAY_MAX_FIELD, in);
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

	u16 count = r3_arr_field(ARRAY_COUNT_FIELD, in);
	if (count == 0) {
		r3_log_stdoutf(WARN_LOG, "[arr] attempted pull from empty array: (count)%d\n", count);
		return 0;
	}

	if (!r3_arr_read(count - 1, value, in)) {
		r3_log_stdout(ERROR_LOG, "[arr] array pull failed\n");
		return 0;
	}

	u16 stride = r3_arr_field(ARRAY_STRIDE_FIELD, in);
	if (!r3_mem_set(stride, 0, (ptr)((u8*)in->data + ((count - 1) * stride)))) {
		r3_log_stdout(ERROR_LOG, "[arr] failed to zero array memory\n");
		return 0;
	}

	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_COUNT_FIELD, sizeof(u16), count - 1, in->data);

	return 1;
}

u8 r3_arr_pull(u16 index, ptr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[arr] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

	u16 count = r3_arr_field(ARRAY_COUNT_FIELD, in);
	if (count == 0) {
		r3_log_stdoutf(WARN_LOG, "[arr] attempted pull from empty array: (count)%d\n", count);
		return 0;
	}

	u16 max = r3_arr_field(ARRAY_MAX_FIELD, in);
	if (index >= max) {
		r3_log_stdoutf(WARN_LOG, "[arr] attempted pull past array bounds: (max)%d (index)%d\n", max, index);
		return 0;
	}

	if (!r3_arr_read(index, value, in)) {
		r3_log_stdout(ERROR_LOG, "[arr] array pull failed\n");
		return 0;
	}

	// left shift
	u16 stride = r3_arr_field(ARRAY_STRIDE_FIELD, in);
	r3_mem_move(stride * (count - (count - (index + 1))), (ptr)((u8*)in->data + (stride * (index + 1))), (ptr)((u8*)in->data + (stride * index)));
    
	if (!r3_mem_set(stride, 0, (ptr)((u8*)in->data + (stride * (count - 1))))) {
		r3_log_stdout(ERROR_LOG, "[arr] failed to zero array\n");
		return 0;
	}

	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_COUNT_FIELD, sizeof(u16), count - 1, in->data);

	return 1;
}



u8 r3_arr_read(u16 index, ptr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdoutf(ERROR_LOG, "[arr] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}
	
	u16 max = r3_arr_field(ARRAY_MAX_FIELD, in);
	if (index >= max) {
		r3_log_stdoutf(WARN_LOG, "[arr] attempted read past array bounds: (max)%d (index)%d\n", max, index);
		return 0;
	}
	
	u16 stride = r3_arr_field(ARRAY_STRIDE_FIELD, in);
	if (!r3_mem_read(stride, value, (ptr)((u8*)in->data + (index * stride)))) {
		r3_log_stdout(ERROR_LOG, "[arr] failed array write\n");
		return 0;
	}

	return 1;
}

u8 r3_arr_assign(u16 index, ptr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[arr] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}
	
	u16 max = r3_arr_field(ARRAY_MAX_FIELD, in);
	if (index >= max) {
		r3_log_stdoutf(WARN_LOG, "[arr] attempted write past array bounds: (max)%d (index)%d\n", max, index);
		return 0;
	}
	
	u16 count = r3_arr_field(ARRAY_COUNT_FIELD, in);
	if (index >= count) {
		r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_COUNT_FIELD, sizeof(u16), count + 1, in->data);
	}
	
	u16 stride = r3_arr_field(ARRAY_STRIDE_FIELD, in);
	*(uptr*)((u8*)in->data + (index * stride)) = (uptr)value;
	
	return 1;
}

u8 r3_arr_write(u16 index, ptr value, Array* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[arr] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}
	
	u16 max = r3_arr_field(ARRAY_MAX_FIELD, in);
	if (index >= max) {
		r3_arr_realloc(index + 8, in);

		max = r3_arr_field(ARRAY_MAX_FIELD, in);
		r3_log_stdoutf(WARN_LOG, "[arr] reallocated (index + 8) after attempted write past array bounds: (max)%d (index)%d\n", max, index);
	}
	
	u16 count = r3_arr_field(ARRAY_COUNT_FIELD, in);
	if (index >= count) {
		r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_COUNT_FIELD, sizeof(u16), count + 1, in->data);
	}
	
	u16 stride = r3_arr_field(ARRAY_STRIDE_FIELD, in);
	if (!r3_mem_write(stride, value, (ptr)((u8*)in->data + (index * stride)))) {
		r3_log_stdout(ERROR_LOG, "[arr] failed array write\n");
		return 0;
	}
	
	return 1;
}
