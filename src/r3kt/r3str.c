#include <include/r3kt/r3ktmem.h>
#include <include/r3kt/r3ktlog.h>
#include <include/r3kt/r3ktstr.h>


u8 r3_dealloc_string(String* in) {
	if (!in) {
		r3_log(ERROR_LOG, "[r3ktstr] invalid string pointer\n");
		return 0;
	}
	
	addr raw = (addr)((u8*)in->data - (sizeof(u16) * 3));
	r3_dealloc(raw);

	return 1;
}

u8 r3_alloc_string(u16 max, String* out) {
	u8 size_err; u8 ptr_err; u8 alloc_err;
	if ((size_err = (!max || max >= I16_MAX)) || (ptr_err = (!out))) {
		if (size_err) r3_log_fmt(ERROR_LOG, "[r3ktstr] invalid string max: %d\n", max);
		else r3_log(ERROR_LOG, "[r3ktstr] invalid string pointer\n");
		return 0;
	}

	addr raw = r3_alloc((sizeof(u16) * 3) + ((max + 1) * sizeof(u8)), 8);
	if ((alloc_err = (!raw))) {
		r3_log(ERROR_LOG, "[r3ktstr] string allocation failed");
		return 0;
	}

	u16 count = 0;
	u16 size = max * sizeof(u8);
	*(u16*)((u8*) raw) = max;
	*(u16*)((u8*) raw + sizeof(u16)) = size;
	*(u16*)((u8*) raw + (sizeof(u16) * 2)) = count;

	out->data = (addr)((u8*)raw + (sizeof(u16) * 3));

	*((u8*)out->data + max) = '\0';

	return 1;
}


u16 r3_string_field(u8 field, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (field < 0 || field >= STRING_HEADER_FIELDS)) || ((ptr_err = (!in)))) {
		if (value_err) printf("[r3ktstr] invalid string header field: %d\n", field);
		if (ptr_err) printf("[r3ktstr] invalid string pointer\n");
		return 0;
	}
	
	switch(field) {
		case MAX_FIELD:
			return *((u16*)((u8*)in->data - (sizeof(u16) * 3))) - 1;
		case SIZE_FIELD:
			return *((u16*)((u8*)in->data - (sizeof(u16) * 2)));
		case COUNT_FIELD:
			return *((u16*)((u8*)in->data - sizeof(u16)));
		default: return 0;
	}
}

u8 r3_combine_string(String* dest, String* src) {
	u8 dest_err; u8 src_err;
	if ((dest_err = (!dest || !dest->data)) || ((src_err = (!src || !src->data)))) {
		if (dest_err) r3_log(ERROR_LOG, "[r3ktstr] invalid destination string pointer\n");
		if (src_err) r3_log(ERROR_LOG, "[r3ktstr] invalid source string pointer\n");
		return 0;
	}

	u16 dest_max = r3_string_field(MAX_FIELD, dest);
	u16 src_count = r3_string_field(COUNT_FIELD, src);
	u16 dest_count = r3_string_field(COUNT_FIELD, dest);
	if (dest_count == dest_max || dest_count + src_count > dest_max) {
		r3_log_fmt(WARN_LOG, "[r3ktstr] attempted illegal string combination: (dest max)%d (dest count)%d (src count)%d", dest_max, dest_count, src_count);
		return 0;
	}

	if (!r3_extend_string(src_count, src->data, dest)) {
		r3_log(ERROR_LOG, "[r3ktstr] failed string combination\n");
		return 0;
	}

	return 1;
}

u8 r3_read_string(u16 count, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log(ERROR_LOG, "[r3ktstr] value error\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktstr] invalid string pointer\n");
		return 0;
	}
	
	u16 max = r3_string_field(MAX_FIELD, in);
	if (count > max) {
		r3_log_fmt(WARN_LOG, "[r3ktstr] attempted read outside string bounds: (max)%d (count)%d\n", max, count);
		return 0;
	}
	
	if (!r3_read_memory(sizeof(u8) * count, value, in->data)) {
		r3_log(ERROR_LOG, "[r3ktstr] failed string read\n");
		return 0;
	}

	return 1;
}

u8 r3_slice_string(u16 start, u16 end, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log(ERROR_LOG, "[r3ktstr] value error\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktstr] invalid string pointer\n");
		return 0;
	}

	if (start > end) {
	    r3_log_fmt(WARN_LOG, "[r3ktstr] attempted backward slice: (start)%d (end)%d\n", start, end);
	    return 0;
	}

	u16 max = r3_string_field(MAX_FIELD, in);
	if (start > max || end > max) {
		r3_log_fmt(WARN_LOG, "[r3ktstr] attempted slice outside string bounds: (max)%d (start)%d (end)%d\n", max, start, end);
		return 0;
	}
	
	u16 count = end - start;
	if (!r3_read_memory(sizeof(u8) * count, value, (addr)((u8*)in->data + (sizeof(u8) * start)))) {
		r3_log(ERROR_LOG, "[r3ktstr] failed string slice\n");
		return 0;
	}

	return 1;
}

u8 r3_write_string(u16 count, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log(ERROR_LOG, "[r3ktstr] value error\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktstr] invalid string pointer\n");
		return 0;
	}
	
	u16 src_max = r3_string_field(MAX_FIELD, in);
	u16 src_count = r3_string_field(COUNT_FIELD, in);
	if (count > src_max || src_count + count > src_max) {
		r3_log_fmt(WARN_LOG, "[r3ktstr] attempted write outside string bounds: (max)%d (count)%d\n", src_max, count);
		return 0;
	}
	
	if (!r3_write_memory(sizeof(u8) * count, value, in->data)) {
		r3_log(ERROR_LOG, "[r3ktstr] failed string write\n");
		return 0;
	}

	*(u16*)((u8*)in->data - sizeof(u16)) = count;

	return 1;
}

u8 r3_extend_string(u16 count, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log(ERROR_LOG, "[r3ktstr] value error\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktstr] invalid string pointer\n");
		return 0;
	}
	
	u16 src_max = r3_string_field(MAX_FIELD, in);
	u16 src_count = r3_string_field(COUNT_FIELD, in);
	if (count > src_max || src_count + count > src_max) {
		r3_log_fmt(WARN_LOG, "[r3ktstr] attempted write outside string bounds: (max)%d (count)%d\n", src_max, count);
		return 0;
	}
	
	if (!r3_write_memory(sizeof(u8) * count, value, (addr)((u8*)in->data + (sizeof(u8) * src_count)))) {
		r3_log(ERROR_LOG, "[r3ktstr] failed string write\n");
		return 0;
	}

	*(u16*)((u8*)in->data - sizeof(u16)) = src_count + count;

	return 1;
}

u8 r3_copy_string(u16 count, String* dest, String* src) {
	u8 dest_err; u8 src_err;
	if ((dest_err = (!dest || !dest->data)) || ((src_err = (!src || !src->data)))) {
		if (dest_err) r3_log(ERROR_LOG, "[r3ktstr] invalid destination string pointer\n");
		if (src_err) r3_log(ERROR_LOG, "[r3ktstr] invalid source string pointer\n");
		return 0;
	}

	u16 src_max = r3_string_field(MAX_FIELD, src);
	u16 dest_max = r3_string_field(MAX_FIELD, dest);
	if (dest_max < src_max) {
		r3_log_fmt(WARN_LOG, "[r3ktstr] attempted illegal string copy: (dest max)%d (src max)%d", dest_max, src_max);
		return 0;
	}

	if (!r3_write_memory(sizeof(u8) * count, src->data, dest->data)) {
		r3_log(ERROR_LOG, "[r3ktstr] failed string write\n");
		return 0;
	}

	*(u16*)((u8*)dest->data - sizeof(u16)) = count;

	return 1;
}

u8 r3_read_string_at(u16 offset, u16 count, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log(ERROR_LOG, "[r3ktstr] value error\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktstr] invalid string pointer\n");
		return 0;
	}
	
	u16 max = r3_string_field(MAX_FIELD, in);
	if (offset > max || count > max) {
		r3_log_fmt(WARN_LOG, "[r3ktstr] attempted read outside string bounds: (max)%d (offset)%d (count)%d\n", max, offset, count);
		return 0;
	}
	
	if (!r3_read_memory(sizeof(u8) * count, value, (addr)((u8*)in->data + (sizeof(u8) * offset)))) {
		r3_log(ERROR_LOG, "[r3ktstr] failed string read\n");
		return 0;
	}

	return 1;
}

u8 r3_write_string_at(u16 offset, u16 count, char* value, String* in) {
	u8 value_err; u8 ptr_err;
	if ((value_err = (!value)) || (ptr_err = (!in || !in->data))) {
		if (value_err) r3_log(ERROR_LOG, "[r3ktstr] value error\n");
		if (ptr_err) r3_log(ERROR_LOG, "[r3ktstr] invalid string pointer\n");
		return 0;
	}
	
	u16 src_max = r3_string_field(MAX_FIELD, in);
	u16 src_count = r3_string_field(COUNT_FIELD, in);
	if (offset > src_max || count > src_max || src_count + count > src_max) {
		r3_log_fmt(WARN_LOG, "[r3ktstr] attempted write outside string bounds: (max)%d (offset)%d (count)%d\n", src_max, offset, count);
		return 0;
	}
	
	if (!r3_write_memory(sizeof(u8) * count, value, (addr)((u8*)in->data + (sizeof(u8) * offset)))) {
		r3_log(ERROR_LOG, "[r3ktstr] failed string write\n");
		return 0;
	}


	*(u16*)((u8*)in->data - sizeof(u16)) = src_count + count;

	return 1;
}

u8 r3_copy_string_at(u16 offset, u16 count, String* dest, String* src) {
	u8 dest_err; u8 src_err;
	if ((dest_err = (!dest || !dest->data)) || ((src_err = (!src || !src->data)))) {
		if (dest_err) r3_log(ERROR_LOG, "[r3ktstr] invalid destination string pointer\n");
		if (src_err) r3_log(ERROR_LOG, "[r3ktstr] invalid source string pointer\n");
		return 0;
	}

	u16 src_max = r3_string_field(MAX_FIELD, src);
	u16 dest_max = r3_string_field(MAX_FIELD, dest);
	u16 src_count = r3_string_field(COUNT_FIELD, src);
	if (offset > src_max || count > src_max || count > dest_max) {
		r3_log_fmt(WARN_LOG, "[r3ktstr] attempted illegal string copy: (dest max)%d (offset)%d (src max)%d (src count)%d", dest_max, offset, src_max, src_count);
		return 0;
	}

	if (!r3_write_memory(sizeof(u8) * count, (addr)((u8*)src->data + (sizeof(u8) * offset)), dest->data)) {
		r3_log(ERROR_LOG, "[r3ktstr] failed string write\n");
		return 0;
	}

	*(u16*)((u8*)dest->data - sizeof(u16)) = src_count - offset;

	return 1;
}

