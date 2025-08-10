#include <r3kit/include/mem/alloc.h>
#include <r3kit/include/ds/arr.h>
#include <r3kit/include/io/log.h>
#include <string.h>

typedef struct Key_Value {
	const char* key;
	ptr value;
} Key_Value;


static inline ptr r3_get_header(u8 fields, u16 stride, ptr data) {
    return (ptr)((u8*)data - (stride * fields));
}

static inline u16 r3_get_field(u8 fields, u8 field, u16 stride, ptr data) {
    return ((u16*)r3_get_header(fields, stride, data))[field];
}

static inline none r3_set_field(u8 fields, u8 field, u16 stride, u16 value, ptr data) {
    *(u16*)((u8*)data - (stride * (fields - field))) = value;
}


u32 _fnv1a(const char* key) {
	if (!key) return I32_MAX;
	u32 hash = 2166136261u; 	// FNV-1a offset basis
	while (*key) {
		hash ^= (char)*key++;	// XOR with current character
		hash *= 16777619;		// multiply by FNV-1a prime
	}; return hash;
}

u16 _probe_write_index(const char* key, Array* in) {
	u16 max = r3_arr_field(ARRAY_MAX_FIELD, in);
	u32 index = _fnv1a(key) % max;
	u32 start = index;
	FOR_I(0, max, 1) {
		Key_Value* key_value = &((Key_Value*)in->data)[index];
		if (key_value->key != NULL && strcmp(key_value->key, key) == 0) {
			r3_log_stdoutf(WARN_LOG, "[arr] probed overwrite index: (key)%s (index)%d\n", key_value->key, index);
			return (u16)index;
		} else if (key_value->value == NULL) {
			r3_log_stdoutf(SUCCESS_LOG, "[arr] probed write index: (key)%s (index)%d\n", key, index);
			return (u16)index;
		} else {
			index = (index + 1) % max;
			if (index == start) {
				r3_log_stdoutf(ERROR_LOG, "[arr] failed to probe write index: (key)%s\n", key);
				return I16_MAX;
			}
		}
	}
	return (u16)index;
}

u16 _probe_read_index(const char* key, Array* in) {
	u16 max = r3_arr_field(ARRAY_MAX_FIELD, in);
	u32 index = _fnv1a(key) % max;
	u16 start = index;
	FOR_I(0, max, 1) {
		Key_Value* key_value = &((Key_Value*)in->data)[index];
		if (key_value->key != NULL && strcmp(key_value->key, key) == 0) {
			r3_log_stdoutf(SUCCESS_LOG, "[arr] probed read index: (key)%s (index)%d\n", key_value->key, index);
			return (u16)index;
		} else {
			index = (index + 1) % max;
			if (index == start) {
				r3_log_stdoutf(ERROR_LOG, "[arr] failed to probe read index: (key)%s\n", key);
				return I16_MAX;
			}
		}
	}
	return (u16)index;
}


u8 r3_arr_hashed_alloc(u16 max, u16 stride, Array* out) {
	u8 size_err; u8 ptr_err;
	if ((size_err = (max >= I16_MAX)) || (ptr_err = (!out || out->data != NULL))) {
		if (size_err) r3_log_stdoutf(ERROR_LOG, "[arr] invalid array max: %d\n", max);
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[arr] invalid array pointer\n");
		return 0;
	}

	ptr raw = r3_mem_alloc(((sizeof(u16) * 4) + (sizeof(Key_Value) * max)), 8);
	if (!raw) {
		r3_log_stdout(ERROR_LOG, "[arr] array allocation failed");
		return 0;
	}

	u16 count = 0;
	u16 size = ((sizeof(u16) * 4) + (sizeof(Key_Value) * max));

	out->data = (ptr)((u8*)raw + (sizeof(u16) * 4));
	if (!r3_mem_set(size, 0, out->data)) {
		r3_log_stdout(WARN_LOG, "[arr] failed to zero array memory");
	}

	FOR_I(0, max, 1) {
		Key_Value* key_value = (Key_Value*)((u8*)out->data + (sizeof(Key_Value) * i));
		key_value->key = NULL;
		key_value->value = NULL;
	}

	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_MAX_FIELD, sizeof(u16), max, out->data);
	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_SIZE_FIELD, sizeof(u16), size, out->data);
	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_COUNT_FIELD, sizeof(u16), count, out->data);
	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_STRIDE_FIELD, sizeof(u16), stride, out->data);

	return 1;
}

u8 r3_arr_hashed_dealloc(Array* in) {
	FOR_I(0, r3_arr_field(ARRAY_MAX_FIELD, in), 1) {
		Key_Value* key_value = (Key_Value*)((u8*)in->data + (sizeof(Key_Value) * i));
		if (key_value->value == NULL) { continue; } else {
			r3_log_stdoutf(SUCCESS_LOG, "[arr] deallocating key: (key)%s (index)%d\n", key_value->key, i);
			r3_mem_dealloc(key_value->value);
		}
	}
	
	if (!r3_arr_dealloc(in)) {
		return 0;
	} else { return 1; }
}


u8 r3_arr_hashed_pull(const char* key, ptr value, Array* in) {
	u8 key_err; u8 value_err; u8 ptr_err;
	if ((key_err = (key == 0)) || (value_err = (value == 0)) || (ptr_err = (in == 0))) {
		if (key_err) {
			r3_log_stdout(ERROR_LOG, "[arr] key error\n");
			return 0;
		} if (value_err) {
			r3_log_stdout(ERROR_LOG, "[arr] value error\n");
			return 0;
		} if (ptr_err) {
			r3_log_stdout(ERROR_LOG, "[arr] invalid array ptr\n");
			return 0;
		}		
	}

	u32 index = _probe_read_index(key, in);
	if (index > r3_arr_field(ARRAY_MAX_FIELD, in)) {
		r3_log_stdoutf(ERROR_LOG, "[arr] failed to probe pull hashed index: (key)%s (index)%d\n", key, index);
		return 0;
	} else { r3_log_stdoutf(INFO_LOG, "[arr] probed pull hashed index: (key)%s (index)%d\n", key, index); }
	
	Key_Value* key_value = &((Key_Value*)in->data)[index];
	u16 stride = r3_arr_field(ARRAY_STRIDE_FIELD, in);
	if (key_value->value == NULL) {
		r3_log_stdoutf(ERROR_LOG, "[arr] removing unexpected null key: (key)%s (index)%d\n", key, index);
		key_value->value = NULL;
		key_value->key = NULL;
		return 0;
	} if (!r3_mem_read(stride, value, key_value->value)) {
		r3_log_stdoutf(ERROR_LOG, "[arr] failed to read memory for key: (key)%s (index)%d\n", key, index);
		return 0;
	}

	r3_mem_dealloc(key_value->value);
	key_value->key = NULL;
	key_value->value = NULL;
	
	// decrement count on kvp dealloc
	u16 count = r3_arr_field(ARRAY_COUNT_FIELD, in);
	r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_COUNT_FIELD, sizeof(u16), count - 1, in->data);

	r3_log_stdoutf(INFO_LOG, "[arr] pulled key: (key)%s\n", key);

	return 1;
}

u8 r3_arr_hashed_read(const char* key, ptr value, Array* in) {
	u8 key_err; u8 value_err; u8 ptr_err;
	if ((key_err = (key == 0)) || (value_err = (value == 0)) || (ptr_err = (in == 0))) {
		if (key_err) {
			r3_log_stdout(ERROR_LOG, "[arr] key error\n");
			return 0;
		} if (value_err) {
			r3_log_stdout(ERROR_LOG, "[arr] value error\n");
			return 0;
		} if (ptr_err) {
			r3_log_stdout(ERROR_LOG, "[arr] invalid array ptr\n");
			return 0;
		}		
	}

	u32 index = _probe_read_index(key, in);
	if (index > r3_arr_field(ARRAY_MAX_FIELD, in)) {
		r3_log_stdoutf(ERROR_LOG, "[arr] failed to probe read hashed index: (key)%s (index)%d\n", key, index);
		return 0;
	} else { r3_log_stdoutf(INFO_LOG, "[arr] probed read hashed index: (key)%s (index)%d\n", key, index); }
	
	Key_Value* key_value = &((Key_Value*)in->data)[index];
	u16 stride = r3_arr_field(ARRAY_STRIDE_FIELD, in);
	if (key_value->value == NULL) {
		r3_log_stdoutf(ERROR_LOG, "[arr] removing unexpected null key: (key)%s (index)%d\n", key, index);
		key_value->value = NULL;
		key_value->key = NULL;
		return 0;
	} if (!r3_mem_read(stride, value, key_value->value)) {
		r3_log_stdoutf(ERROR_LOG, "[arr] failed to read memory for key: (key)%s (index)%d\n", key, index);
		return 0;
	}

	return 1;
}


// TODO: implement 70% load balance
u8 r3_arr_hashed_write(const char* key, ptr value, Array* in) {
	u8 key_err; u8 value_err; u8 ptr_err;
	if ((key_err = (key == 0)) || (value_err = (value == 0)) || (ptr_err = (in == 0))) {
		if (key_err) {
			r3_log_stdout(ERROR_LOG, "[arr] key error\n");
			return 0;
		} if (value_err) {
			r3_log_stdout(ERROR_LOG, "[arr] value error\n");
			return 0;
		} if (ptr_err) {
			r3_log_stdout(ERROR_LOG, "[arr] invalid array ptr\n");
			return 0;
		}
	}

	u32 index = _probe_write_index(key, in);
	if (index > r3_arr_field(ARRAY_MAX_FIELD, in)) {
		r3_log_stdoutf(ERROR_LOG, "[arr] failed to probe write hashed index: (key)%s (index)%d\n", key, index);
		return 0;
	} else { r3_log_stdoutf(INFO_LOG, "[arr] probed write hashed index: (key)%s (index)%d\n", key, index); }
	
	Key_Value* key_value = &((Key_Value*)in->data)[index];
	u16 stride = r3_arr_field(ARRAY_STRIDE_FIELD, in);
	if (key_value->value == NULL) {
		r3_log_stdoutf(INFO_LOG, "[arr] allocating memory for key: (key)%s\n", key);
		key_value->value = r3_mem_alloc(stride, 8);
		if (key_value->value == NULL) {
			r3_log_stdoutf(ERROR_LOG, "[arr] failed to allocate memory for key: (key)%s (index)%d\n", key, index);
			return 0;
		}
		
		// increment count if key value is new
		u16 count = r3_arr_field(ARRAY_COUNT_FIELD, in);
		r3_set_field(ARRAY_HEADER_FIELDS, ARRAY_COUNT_FIELD, sizeof(u16), count + 1, in->data);
	} if (!r3_mem_write(stride, value, key_value->value)) {
		r3_log_stdoutf(ERROR_LOG, "[arr] failed to write value for key: (key)%s (index)%d\n", key, index);
		r3_mem_dealloc(key_value->value);
		key_value->value = NULL;
		key_value->key = NULL;
		return 0;
	}
	key_value->key = key;

	return 1;
}

u8 r3_arr_hashed_assign(const char* key, ptr value, Array* in) {
	u8 key_err; u8 value_err; u8 ptr_err;
	if ((key_err = (key == 0)) || (value_err = (value == 0)) || (ptr_err = (in == 0))) {
		if (key_err) {
			r3_log_stdout(ERROR_LOG, "[arr] key error\n");
			return 0;
		} if (value_err) {
			r3_log_stdout(ERROR_LOG, "[arr] value error\n");
			return 0;
		} if (ptr_err) {
			r3_log_stdout(ERROR_LOG, "[arr] invalid array ptr\n");
			return 0;
		}
	}

	u32 index = _probe_write_index(key, in);
	if (index > r3_arr_field(ARRAY_MAX_FIELD, in)) {
		r3_log_stdoutf(ERROR_LOG, "[arr] failed to probe assign hashed index: (key)%s (index)%d\n", key, index);
		return 0;
	} else { r3_log_stdoutf(INFO_LOG, "[arr] probed assign hashed index: (key)%s (index)%d\n", key, index); }
	
	Key_Value* key_value = &((Key_Value*)in->data)[index];
	if (key_value->value == NULL) {
		r3_log_stdoutf(ERROR_LOG, "[arr] removing unexpected null key: (key)%s (index)%d\n", key, index);
		key_value->value = NULL;
		key_value->key = NULL;
		return 0;
	} else {
		*(uptr*)key_value->value = *(uptr*)value;
		r3_log_stdoutf(INFO_LOG, "[arr] assigned key: (key)%s\n", key);
	}

	return 1;
}
