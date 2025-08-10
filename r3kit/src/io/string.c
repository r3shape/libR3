#include <r3kit/include/mem/alloc.h>
#include <r3kit/include/io/log.h>
#include <r3kit/include/io/str.h>

static inline ptr r3_get_header(u8 fields, u16 stride, ptr data) {
    return (ptr)((u8*)data - (stride * fields));
}

static inline u16 r3_get_field(u8 fields, u8 field, u16 stride, ptr data) {
    return ((u16*)r3_get_header(fields, stride, data))[field];
}

static inline none r3_set_field(u8 fields, u8 field, u16 stride, u16 value, ptr data) {
    *(u16*)((u8*)data - (stride * (fields - field))) = value;
}


u8 r3_str_dealloc(String* in) {
	if (!in || !in->data) {
		r3_log_stdout(ERROR_LOG, "[str] invalid string pointer\n");
		return 0;
	}
	
	ptr raw = (ptr)((u8*)in->data - (sizeof(u16) * STRING_HEADER_FIELDS));
	r3_mem_dealloc(raw);

	return 1;
}

u8 r3_str_alloc(u16 max, String* out) {
	u8 size_err; u8 ptr_err; u8 alloc_err;
	if ((size_err = (!max || max >= I16_MAX)) || (ptr_err = (!out || out->data != NULL))) {
		if (size_err) r3_log_stdoutf(ERROR_LOG, "[str] invalid string max: %d\n", max);
		else r3_log_stdout(ERROR_LOG, "[str] invalid string pointer\n");
		return 0;
	}
	
	ptr raw = r3_mem_alloc((sizeof(u16) * STRING_HEADER_FIELDS) + ((max + 1) * sizeof(u8)), 8);
	if ((alloc_err = (!raw))) {
		r3_log_stdout(ERROR_LOG, "[str] string allocation failed");
		return 0;
	}

	u16 count = 0;
	u16 size = max * sizeof(u8);

	out->data = (ptr)((u8*)raw + (sizeof(u16) * STRING_HEADER_FIELDS));
	*((u8*)out->data + max) = '\0';

	r3_set_field(STRING_HEADER_FIELDS, MAX_FIELD, sizeof(u16), max, out->data);
	r3_set_field(STRING_HEADER_FIELDS, SIZE_FIELD, sizeof(u16), size, out->data);
	r3_set_field(STRING_HEADER_FIELDS, COUNT_FIELD, sizeof(u16), count, out->data);

	return 1;
}

u16 r3_str_field(u8 field, String* in) {
	return r3_get_field(STRING_HEADER_FIELDS, field, sizeof(u16), in->data);
}


u8 r3_str_combine(String* dest, String* src) {
	u8 dest_err; u8 src_err;
	if ((dest_err = (!dest || !dest->data)) || ((src_err = (!src || !src->data)))) {
		if (dest_err) r3_log_stdout(ERROR_LOG, "[str] invalid destination string pointer\n");
		if (src_err) r3_log_stdout(ERROR_LOG, "[str] invalid source string pointer\n");
		return 0;
	}

	u16 dest_max = r3_str_field(MAX_FIELD, dest);
	u16 src_count = r3_str_field(COUNT_FIELD, src);
	u16 dest_count = r3_str_field(COUNT_FIELD, dest);
	if (dest_count == dest_max || dest_count + src_count > dest_max) {
		r3_log_stdoutf(WARN_LOG, "[str] attempted illegal string combination: (dest max)%d (dest count)%d (src count)%d", dest_max, dest_count, src_count);
		return 0;
	}

	if (!r3_str_extend(src_count, src->data, dest)) {
		r3_log_stdout(ERROR_LOG, "[str] failed string combination\n");
		return 0;
	}

	return 1;
}

u8 r3_str_extend(u16 count, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[str] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[str] invalid string pointer\n");
		return 0;
	}
	
	u16 src_max = r3_str_field(MAX_FIELD, in);
	u16 src_count = r3_str_field(COUNT_FIELD, in);
	if (count > src_max || src_count + count > src_max) {
		r3_log_stdoutf(WARN_LOG, "[str] attempted write outside string bounds: (max)%d (count)%d\n", src_max, count);
		return 0;
	}
	
	if (!r3_mem_write(sizeof(u8) * count, value, (ptr)((u8*)in->data + (sizeof(u8) * src_count)))) {
		r3_log_stdout(ERROR_LOG, "[str] failed string write\n");
		return 0;
	}

	r3_set_field(STRING_HEADER_FIELDS, COUNT_FIELD, sizeof(u16), src_count + count, in->data);

	return 1;
}

u8 r3_str_copy(u16 count, String* dest, String* src) {
	u8 dest_err; u8 src_err;
	if ((dest_err = (!dest || !dest->data)) || ((src_err = (!src || !src->data)))) {
		if (dest_err) r3_log_stdout(ERROR_LOG, "[str] invalid destination string pointer\n");
		if (src_err) r3_log_stdout(ERROR_LOG, "[str] invalid source string pointer\n");
		return 0;
	}

	u16 src_max = r3_str_field(MAX_FIELD, src);
	u16 dest_max = r3_str_field(MAX_FIELD, dest);
	if (dest_max < src_max) {
		r3_log_stdoutf(WARN_LOG, "[str] attempted illegal string copy: (dest max)%d (src max)%d", dest_max, src_max);
		return 0;
	}

	if (!r3_mem_write(sizeof(u8) * count, src->data, dest->data)) {
		r3_log_stdout(ERROR_LOG, "[str] failed string write\n");
		return 0;
	}

	r3_set_field(STRING_HEADER_FIELDS, COUNT_FIELD, sizeof(u16), count, dest->data);

	return 1;
}

u8 r3_str_slice(u16 start, u16 end, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[str] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[str] invalid string pointer\n");
		return 0;
	}

	if (start > end) {
	    r3_log_stdoutf(WARN_LOG, "[str] attempted backward slice: (start)%d (end)%d\n", start, end);
	    return 0;
	}

	u16 max = r3_str_field(MAX_FIELD, in);
	if (start > max || end > max) {
		r3_log_stdoutf(WARN_LOG, "[str] attempted slice outside string bounds: (max)%d (start)%d (end)%d\n", max, start, end);
		return 0;
	}
	
	u16 count = end - start;
	if (!r3_mem_read(sizeof(u8) * count, value, (ptr)((u8*)in->data + (sizeof(u8) * start)))) {
		r3_log_stdout(ERROR_LOG, "[str] failed string slice\n");
		return 0;
	}

	return 1;
}


u8 r3_str_read(u16 count, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[str] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[str] invalid string pointer\n");
		return 0;
	}
	
	u16 max = r3_str_field(MAX_FIELD, in);
	if (count > max) {
		r3_log_stdoutf(WARN_LOG, "[str] attempted read outside string bounds: (max)%d (count)%d\n", max, count);
		return 0;
	}
	
	if (!r3_mem_read(sizeof(u8) * count, value, in->data)) {
		r3_log_stdout(ERROR_LOG, "[str] failed string read\n");
		return 0;
	}

	// TODO: null-terminate the read buffer
	return 1;
}

u8 r3_str_write(u16 count, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[str] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[str] invalid string pointer\n");
		return 0;
	}
	
	u16 src_max = r3_str_field(MAX_FIELD, in);
	u16 src_count = r3_str_field(COUNT_FIELD, in);
	if (count > src_max || src_count + count > src_max) {
		r3_log_stdoutf(WARN_LOG, "[str] attempted write outside string bounds: (max)%d (count)%d\n", src_max, count);
		return 0;
	}
	
	if (!r3_mem_write(sizeof(u8) * count, value, in->data)) {
		r3_log_stdout(ERROR_LOG, "[str] failed string write\n");
		return 0;
	}

	r3_set_field(STRING_HEADER_FIELDS, COUNT_FIELD, sizeof(u16), src_count + count, in->data);

	return 1;
}


u8 r3_str_read_at(u16 offset, u16 count, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[str] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[str] invalid string pointer\n");
		return 0;
	}
	
	u16 max = r3_str_field(MAX_FIELD, in);
	if (offset > max || count > max) {
		r3_log_stdoutf(WARN_LOG, "[str] attempted read outside string bounds: (max)%d (offset)%d (count)%d\n", max, offset, count);
		return 0;
	}
	
	if (!r3_mem_read(sizeof(u8) * count, value, (ptr)((u8*)in->data + (sizeof(u8) * offset)))) {
		r3_log_stdout(ERROR_LOG, "[str] failed string read\n");
		return 0;
	}

	return 1;
}

u8 r3_str_write_at(u16 offset, u16 count, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log_stdout(ERROR_LOG, "[str] value error\n");
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[str] invalid string pointer\n");
		return 0;
	}
	
	u16 src_max = r3_str_field(MAX_FIELD, in);
	u16 src_count = r3_str_field(COUNT_FIELD, in);
	if (offset > src_max || count > src_max || src_count + count > src_max) {
		r3_log_stdoutf(WARN_LOG, "[str] attempted write outside string bounds: (max)%d (offset)%d (count)%d\n", src_max, offset, count);
		return 0;
	}
	
	if (!r3_mem_write(sizeof(u8) * count, value, (ptr)((u8*)in->data + (sizeof(u8) * offset)))) {
		r3_log_stdout(ERROR_LOG, "[str] failed string write\n");
		return 0;
	}

	r3_set_field(STRING_HEADER_FIELDS, COUNT_FIELD, sizeof(u16), src_count + count, in->data);

	return 1;
}

u8 r3_str_copy_at(u16 offset, u16 count, String* dest, String* src) {
	u8 dest_err; u8 src_err;
	if ((dest_err = (!dest || !dest->data)) || ((src_err = (!src || !src->data)))) {
		if (dest_err) r3_log_stdout(ERROR_LOG, "[str] invalid destination string pointer\n");
		if (src_err) r3_log_stdout(ERROR_LOG, "[str] invalid source string pointer\n");
		return 0;
	}

	u16 src_max = r3_str_field(MAX_FIELD, src);
	u16 dest_max = r3_str_field(MAX_FIELD, dest);
	u16 src_count = r3_str_field(COUNT_FIELD, src);
	if (offset > src_max || count > src_max || count > dest_max) {
		r3_log_stdoutf(WARN_LOG, "[str] attempted illegal string copy: (dest max)%d (offset)%d (src max)%d (src count)%d", dest_max, offset, src_max, src_count);
		return 0;
	}

	if (!r3_mem_write(sizeof(u8) * count, (ptr)((u8*)src->data + (sizeof(u8) * offset)), dest->data)) {
		r3_log_stdout(ERROR_LOG, "[str] failed string write\n");
		return 0;
	}

	r3_set_field(STRING_HEADER_FIELDS, COUNT_FIELD, sizeof(u16), src_count - offset, dest->data);

	return 1;
}
