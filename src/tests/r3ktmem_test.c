#include <include/r3kt/r3ktmem.h>
#include <include/r3kt/r3ktlog.h>
#include <string.h>
#include <stdio.h>

int main() {
	u64 init_size = 1024;
	u64 new_size = 2048;

	// TEST ALLOCATION
	addr p = r3_alloc(init_size, 8);
	if (!p) {
		r3_log_fmt(ERROR_LOG, "[r3ktmem test] failed allocation: %llu bytes\n", init_size);
		return 1;
	}
	r3_log_fmt(SUCCESS_LOG, "[r3ktmem test] passed allocation: %llu bytes\n", init_size);

	// TEST WRITE
	unsigned char write_data[1024];
	for (size_t i = 0; i < sizeof(write_data); ++i) {
		write_data[i] = (unsigned char)0xAB;
	}
	if (!r3_write_memory(sizeof(write_data), write_data, p)) {
		r3_log(ERROR_LOG, "[r3ktmem test] failed write\n");
		return 1;
	}
	r3_log(SUCCESS_LOG, "[r3ktmem test] passed write\n");

	// TEST READ
	unsigned char read_data[1024];
	if (!r3_read_memory(sizeof(read_data), read_data, p)) {
		r3_log(ERROR_LOG, "[r3ktmem test] failed read\n");
		return 1;
	}
	for (size_t i = 0; i < sizeof(read_data); ++i) {
		if (read_data[i] != 0xAB) {
			r3_log_fmt(ERROR_LOG, "[r3ktmem test] data corrupt at offset %zu\n", i);
			return 1;
		}
	}
	r3_log(SUCCESS_LOG, "[r3ktmem test] passed read\n");

	// TEST REALLOC
	addr p2 = r3_realloc(new_size, 8, p);
	if (!p2) {
		r3_log_fmt(ERROR_LOG, "[r3ktmem test] failed realloc: %llu bytes\n", new_size);
		return 1;
	}
	p = p2;
	r3_log_fmt(SUCCESS_LOG, "[r3ktmem test] passed realloc: %llu bytes\n", new_size);

	// VERIFY DATA AFTER REALLOC
	unsigned char read_data2[1024];
	if (!r3_read_memory(sizeof(read_data2), read_data2, p)) {
		r3_log(ERROR_LOG, "[r3ktmem test] failed read after realloc\n");
		return 1;
	}
	for (size_t i = 0; i < sizeof(read_data2); ++i) {
		if (read_data2[i] != 0xAB) {
			r3_log_fmt(ERROR_LOG, "[r3ktmem test] data corrupt after realloc at offset %zu\n", i);
			return 1;
		}
	}
	r3_log(SUCCESS_LOG, "[r3ktmem test] data intact after realloc\n");

	// TEST SET
	u8 set_value = 'X';
	if (!r3_set_memory(new_size, set_value, p)) {
		r3_log_fmt(ERROR_LOG, "[r3ktmem test] failed to set memory: (value)%c\n", set_value);
		return 1;
	} else r3_log_fmt(SUCCESS_LOG, "[r3ktmem test] passed set memory: (value)%c\n", set_value);

	// VERIFY DATA AFTER SET
	unsigned char read_data3[2048];
	if (!r3_read_memory(sizeof(read_data3), read_data3, p)) {
		r3_log(ERROR_LOG, "[r3ktmem test] failed read after set\n");
		return 1;
	}
	for (size_t i = 0; i < sizeof(read_data3); ++i) {
		if (read_data3[i] != set_value) {
			r3_log_fmt(ERROR_LOG, "[r3ktmem test] data corrupt after set at offset %zu\n", i);
			return 1;
		}
	}
	r3_log(SUCCESS_LOG, "[r3ktmem test] data intact after set\n");


	// TEST DEALLOC
	r3_dealloc(p);
	r3_log(SUCCESS_LOG, "[r3ktmem test] passed deallocation\n");

	return 0;
}


