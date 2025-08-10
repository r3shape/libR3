#include <r3kit/include/mem/alloc.h>
#include <r3kit/include/io/log.h>
#include <r3kit/include/io/buf.h>
#include <stdio.h>


static inline ptr r3_get_header(u8 fields, u16 stride, ptr data) {
    return (ptr)((u8*)data - (stride * fields));
}

static inline u16 r3_get_field(u8 fields, u8 field, u16 stride, ptr data) {
    return ((u16*)r3_get_header(fields, stride, data))[field];
}

static inline none r3_set_field(u8 fields, u8 field, u16 stride, u16 value, ptr data) {
    *(u16*)((u8*)data - (stride * (fields - field))) = value;
}

u8 r3_buf_alloc(u16 size, Buffer* out) {
	u8 size_err; u8 ptr_err;
	if ((size_err = (size >= I16_MAX)) || (ptr_err = (!out || out->data != NULL))) {
		if (size_err) r3_log_stdoutf(ERROR_LOG, "[buf] invalid buffer size: %d\n", size);
		if (ptr_err) r3_log_stdout(ERROR_LOG, "[buf] invalid buffer pointer\n");
		return 0;
	}

	ptr raw = r3_mem_alloc((sizeof(u16) * BUFFER_HEADER_FIELDS) + size + 1, 8);
	if (!raw) {
		r3_log_stdout(ERROR_LOG, "[buf] buffer allocation failed");
		return 0;
	}

	out->data = (ptr)((u8*)raw + (sizeof(u16) * BUFFER_HEADER_FIELDS));
	if (!r3_mem_set(size, 0, out->data)) {
		r3_log_stdout(WARN_LOG, "[buf] failed to zero buffer memory");
	}

	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_SIZE_FIELD, sizeof(u16), size, out->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_READ_FIELD, sizeof(u16), 0, out->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_WROTE_FIELD, sizeof(u16), 0, out->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_READS_FIELD, sizeof(u16), 0, out->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_SAVES_FIELD, sizeof(u16), 0, out->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_SAVED_FIELD, sizeof(u16), 0, out->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_LOADS_FIELD, sizeof(u16), 0, out->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_LOADED_FIELD, sizeof(u16), 0, out->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_WRITES_FIELD, sizeof(u16), 0, out->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_CURSOR_FIELD, sizeof(u16), 0, out->data);

	return 1;
}

u16 r3_buf_field(u8 field, Buffer* in) {
	return r3_get_field(BUFFER_HEADER_FIELDS, field, sizeof(u16), in->data);
}

u8 r3_buf_dealloc(Buffer* in) {
	if (!in || !in->data) {
		r3_log_stdout(ERROR_LOG, "[buf] invalid buffer pointer\n");
		return 0;
	}

	ptr raw = (ptr)((u8*)in->data - (sizeof(u16) * BUFFER_HEADER_FIELDS));
	r3_mem_dealloc(raw);

	return 1;
}


u8 r3_buf_rewind(Buffer* in) {
	if (!in || !in->data) {
		r3_log_stdout(ERROR_LOG, "[buf] invalid buffer pointer\n");
		return 0;
	}

	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_CURSOR_FIELD, sizeof(u16), 0, in->data);

	return 1;
}

u8 r3_buf_seek(u16 offset, Buffer* in) {
	if (!in || !in->data) {
		r3_log_stdout(ERROR_LOG, "[buf] invalid buffer pointer\n");
		return 0;
	}

	u16 size = r3_buf_field(BUFFER_SIZE_FIELD, in);
	if (offset > size || offset > I16_MAX) {
		r3_log_stdoutf(ERROR_LOG, "[buf] attempted buffer seek out of bounds: (offset)%d (size)%d\n", offset, size);
		return 0;
	}

	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_CURSOR_FIELD, sizeof(u16), offset, in->data);

	return 1;
}


u8 r3_buf_load(char* path, Buffer* in) {
	if (!path) {
		r3_log_stdout(ERROR_LOG, "[buf] invalid buffer load path\n");
		return 0;
	}
	
	if (!in || !in->data) {
		r3_log_stdout(ERROR_LOG, "[buf] invalid buffer pointer\n");
		return 0;
	}

	FILE* file = NULL;
	if (!(file = (fopen((const char*)path, "rb")))) {
		r3_log_stdoutf(ERROR_LOG, "[buf] failed to load buffer content: (path)%s\n", path);
		return 0;
	}

	u16 file_size;
	u16 size = r3_buf_field(BUFFER_SIZE_FIELD, in);
	u16 cursor = r3_buf_field(BUFFER_CURSOR_FIELD, in);

	if (cursor >= size) {
		r3_log_stdoutf(WARN_LOG, "[buf] attempted write at buffer end: (cursor)%d (buffer size)%d\n", cursor, size);
		fclose(file);
		return 0;
	}

	if (fseek(file, 0, SEEK_END)) {
		r3_log_stdoutf(ERROR_LOG, "[buf] failed to access buffer content: (path)%s\n", path);
		fclose(file);
		return 0;
	}

	file_size = (u16)ftell(file);
	u16 buf_free = size - cursor;
	if (file_size > buf_free) {
		r3_log_stdoutf(WARN_LOG, "[buf] buffer overflow: (buffer size)%d (content size)%d\n", buf_free, file_size);
		return 0;
	}
	
	if (fseek(file, 0, SEEK_SET)) {
		r3_log_stdoutf(ERROR_LOG, "[buf] failed to access buffer content: (path)%s\n", path);
		fclose(file);
		return 0;
	}

	u16 read_bytes = fread((u8*)in->data + cursor, 1, file_size, file);
	if (read_bytes != file_size) {
		r3_log_stdoutf(ERROR_LOG, "[buf] failed to read full buffer: (read)%zu (expected)%d (path)%s\n", read_bytes, file_size, path);
		fclose(file);
		return 0;
	}

	
	fclose(file);

	u16 wrote = r3_buf_field(BUFFER_WROTE_FIELD, in);
	u16 writes = r3_buf_field(BUFFER_WRITES_FIELD, in);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_WROTE_FIELD, sizeof(u16), wrote + file_size, in->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_WRITES_FIELD, sizeof(u16), writes + 1, in->data);

	u16 loads = r3_buf_field(BUFFER_LOADS_FIELD, in);
	u16 loaded = r3_buf_field(BUFFER_LOADED_FIELD, in);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_LOADS_FIELD, sizeof(u16), loads + 1, in->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_LOADED_FIELD, sizeof(u16), loaded + file_size, in->data);

	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_CURSOR_FIELD, sizeof(u16), cursor + file_size, in->data);

	return 1;
}

R3_API u8 r3_buf_save(char* path, Buffer* in) {
	if (!path) {
		r3_log_stdout(ERROR_LOG, "[buf] invalid buffer save path\n");
		return 0;
	}

	if (!in || !in->data) {
		r3_log_stdout(ERROR_LOG, "[buf] invalid buffer pointer\n");
		return 0;
	}

	FILE* file = NULL;
	if (!(file = (fopen((const char*)path, "wb")))) {
		r3_log_stdoutf(ERROR_LOG, "[buf] failed to load buffer content: (path)%s\n", path);
		return 0;
	}

	u16 size = r3_buf_field(BUFFER_SIZE_FIELD, in);
	
	u32 saved_size = fwrite(in->data, 1, size, file);
	if (saved_size != size) {
		r3_log_stdoutf(WARN_LOG, "[buf] failed to write full buffer: (wrote)%zu (expected)%d (path)%s\n", saved_size, size, path);
		fclose(file);
		return 0;
	}

	fclose(file);

	u16 read = r3_buf_field(BUFFER_READ_FIELD, in);
	u16 reads = r3_buf_field(BUFFER_READS_FIELD, in);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_READ_FIELD, sizeof(u16), read + size, in->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_READS_FIELD, sizeof(u16), reads + 1, in->data);
	
	u16 saves = r3_buf_field(BUFFER_SAVES_FIELD, in);
	u16 saved = r3_buf_field(BUFFER_SAVED_FIELD, in);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_SAVES_FIELD, sizeof(u16), saves + 1, in->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_SAVED_FIELD, sizeof(u16), saved + size, in->data);

	return 1;
}

u8 r3_buf_read(u16 count, char* value, Buffer* in) {
	if (!in || !in->data) {
		r3_log_stdout(ERROR_LOG, "[buf] invalid buffer pointer\n");
		return 0;
	}

	if (!value) {
		r3_log_stdout(ERROR_LOG, "[buf] buffer value error\n");
		return 0;
	}

	u16 cursor = r3_buf_field(BUFFER_CURSOR_FIELD, in);
	
	if (!r3_mem_read(count, value, (ptr)((u8*)in->data + cursor))) {
		r3_log_stdout(ERROR_LOG, "[buf] failed to read buffer\n");
		return 0;
	}

	u16 read = r3_buf_field(BUFFER_READ_FIELD, in);
	u16 reads = r3_buf_field(BUFFER_READS_FIELD, in);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_READ_FIELD, sizeof(u16), read + count, in->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_READS_FIELD, sizeof(u16), reads + 1, in->data);

	return 1;
}

u8 r3_buf_write(u16 count, char* value, Buffer* in) {
	if (!in || !in->data) {
		r3_log_stdout(ERROR_LOG, "[buf] invalid buffer pointer\n");
		return 0;
	}

	if (!value) {
		r3_log_stdout(ERROR_LOG, "[buf] buffer value error\n");
		return 0;
	}

	u16 cursor = r3_buf_field(BUFFER_CURSOR_FIELD, in);
	
	if (!r3_mem_write(count, value, (ptr)((u8*)in->data + cursor))) {
		r3_log_stdout(ERROR_LOG, "[buf] failed to write buffer\n");
		return 0;
	}

	u16 wrote = r3_buf_field(BUFFER_WROTE_FIELD, in);
	u16 writes = r3_buf_field(BUFFER_WRITES_FIELD, in);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_WROTE_FIELD, sizeof(u16), wrote + count, in->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_WRITES_FIELD, sizeof(u16), writes + 1, in->data);

	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_CURSOR_FIELD, sizeof(u16), cursor + count, in->data);
	
	return 1;
}

u8 r3_buf_copy(u16 count, Buffer* dest, Buffer* src) {
	if (!dest || !dest->data || !src || !src->data) {
		r3_log_stdout(ERROR_LOG, "[buf] invalid buffer pointer\n");
		return 0;
	}

	u16 src_size = r3_buf_field(BUFFER_SIZE_FIELD, src);
	u16 dest_size = r3_buf_field(BUFFER_SIZE_FIELD, dest);

	if (dest_size < src_size) {
		r3_log_stdoutf(ERROR_LOG, "[buf] attempted buffer copy out of bounds: (dest size)%d (src size)%d\n", dest_size, src_size);
		return 0;
	}

	if (!r3_mem_write(count, src->data, dest->data)) {
		r3_log_stdout(ERROR_LOG, "[buf] failed to copy buffer content\n");
		return 0;
	}

	u16 wrote = r3_buf_field(BUFFER_WROTE_FIELD, src);
	u16 writes = r3_buf_field(BUFFER_WRITES_FIELD, src);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_WROTE_FIELD, sizeof(u16), wrote, dest->data);
	r3_set_field(BUFFER_HEADER_FIELDS, BUFFER_WRITES_FIELD, sizeof(u16), writes, dest->data);

	return 1;
}

