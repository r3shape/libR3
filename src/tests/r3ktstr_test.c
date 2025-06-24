#include <include/r3kt/r3ktstr.h>
#include <include/r3kt/r3ktlog.h>
#include <string.h> /* for memcmp and strlen */
#include <stdio.h>

int main() {
	r3_log(INFO_LOG, "=== r3ktstr Unit Test Begin ===\n");

	// 1. Allocate a test String
	String str;
	if (!r3_alloc_string(32, &str)) {
		r3_log(ERROR_LOG, "failed to alloc str\n");
		return 1;
	}
	r3_log(SUCCESS_LOG, "allocated str with max=32\n");

	// 2. Test write
	const char* hello = "hello";
	if (!r3_write_string(5, (char*)hello, &str)) {
		r3_log(ERROR_LOG, "failed to write 'hello'\n");
		return 1;
	}
	r3_log_fmt(SUCCESS_LOG, "wrote into string: (data)%s\n", (char*)str.data);

	// 3. Test read
	char buffer[32];
	memset(buffer, 0, sizeof(buffer));

	if (!r3_read_string((u16)strlen(hello), buffer, &str)) {
		r3_log(ERROR_LOG, "failed to read 'hello'\n");
		return 1;
	}
	if (memcmp(buffer, "hello", 5) != 0) {
		r3_log_fmt(ERROR_LOG, "read data doesn't match 'hello': (buffer)%s\n", buffer);
		return 1;
	}
	r3_log_fmt(SUCCESS_LOG, "read from string: (buffer)%s\n", buffer);

	// 4. Test extend
	const char* world = " world";
	if (!r3_extend_string((u16)strlen(world), (char*)world, &str)) {
		r3_log(ERROR_LOG, "failed to extend with ' world'\n");
		return 1;
	}
	r3_log(SUCCESS_LOG, "extended with ' world'\n");

	memset(buffer, 0, sizeof(buffer));
	if (!r3_read_string(r3_string_field(COUNT_FIELD, &str), buffer, &str)) {
		r3_log(ERROR_LOG, "failed to read after extend\n");
		return 1;
	}
	if (memcmp(buffer, "hello world", 11) != 0) {
		r3_log(ERROR_LOG, "string after extend is incorrect\n");
		return 1;
	}
	r3_log_fmt(SUCCESS_LOG, "string after extend is correct: \"%s\"\n", buffer);

	// 5. Test copy
	String str_copy;
	if (!r3_alloc_string(32, &str_copy)) {
		r3_log(ERROR_LOG, "failed to alloc str_copy\n");
		return 1;
	}
	if (!r3_copy_string(r3_string_field(COUNT_FIELD, &str), &str_copy, &str)) {
		r3_log(ERROR_LOG, "failed to copy str -> str_copy\n");
		return 1;
	}
	memset(buffer, 0, sizeof(buffer));
	if (!r3_read_string(r3_string_field(COUNT_FIELD, &str_copy), buffer, &str_copy)) {
		r3_log(ERROR_LOG, "failed to read str_copy\n");
		return 1;
	}
	if (memcmp(buffer, "hello world", 11) != 0) {
		r3_log(ERROR_LOG, "string copy is incorrect\n");
		return 1;
	}
	r3_log_fmt(SUCCESS_LOG, "string copy is correct: \"%s\"\n", buffer);

	// 6. Test slice
	memset(buffer, 0, sizeof(buffer));
	if (!r3_slice_string(0, 5, buffer, &str_copy)) {
		r3_log(ERROR_LOG, "failed to slice str_copy\n");
		return 1;
	}
	if (memcmp(buffer, "hello", 5) != 0) {
		r3_log(ERROR_LOG, "string slice is incorrect\n");
		return 1;
	}
	r3_log_fmt(SUCCESS_LOG, "string slice is correct: (buffer)%s\n", buffer);

	// 7. Test backward slice
	memset(buffer, 0, sizeof(buffer));
	if (r3_slice_string(6, 0, buffer, &str_copy)) {
		r3_log(ERROR_LOG, "undefined backward slice str_copy\n");
		return 1;
	}
	r3_log(SUCCESS_LOG, "failed backward slice\n");

	// 8. Test write_string_at
	const char* at_data = "TEST";
	if (!r3_write_string_at(6, 4, (char*)at_data, &str_copy)) {
		r3_log(ERROR_LOG, "failed to write 'TEST' at offset 6\n");
		return 1;
	}
	memset(buffer, 0, sizeof(buffer));
	if (!r3_read_string(r3_string_field(COUNT_FIELD, &str_copy), buffer, &str_copy)) {
		r3_log(ERROR_LOG, "failed to read after write_string_at\n");
		return 1;
	}
	if (memcmp(buffer, "hello TESTd", 11) != 0) {
		r3_log_fmt(ERROR_LOG, "write_string_at failed, buffer='%s'\n", buffer);
		return 1;
	}
	r3_log_fmt(SUCCESS_LOG, "write_string_at succeeded, buffer='%s'\n", buffer);

	// 9. Test read_string_at
	memset(buffer, 0, sizeof(buffer));
	if (!r3_read_string_at(6, 4, buffer, &str_copy)) {
		r3_log(ERROR_LOG, "failed to read_string_at offset 6\n");
		return 1;
	}
	if (memcmp(buffer, "TEST", 4) != 0) {
		r3_log_fmt(ERROR_LOG, "read_string_at failed, buffer='%s'\n", buffer);
		return 1;
	}
	r3_log_fmt(SUCCESS_LOG, "read_string_at succeeded, buffer='%s'\n", buffer);

	// 10. Test copy_string_at
	String str_copy2;
	if (!r3_alloc_string(32, &str_copy2)) {
		r3_log(ERROR_LOG, "failed to alloc str_copy2\n");
		return 1;
	}
	if (!r3_copy_string_at(6, 4, &str_copy2, &str_copy)) {
		r3_log(ERROR_LOG, "failed to copy_string_at offset 6\n");
		return 1;
	}
	memset(buffer, 0, sizeof(buffer));
	if (!r3_read_string(r3_string_field(COUNT_FIELD, &str_copy2), buffer, &str_copy2)) {
		r3_log(ERROR_LOG, "failed to read str_copy2 after copy_string_at\n");
		return 1;
	}
	if (memcmp(buffer, "TEST", 4) != 0) {
		r3_log_fmt(ERROR_LOG, "copy_string_at failed, buffer='%s' copy2='%s'\n", buffer, (u8*)str_copy2.data);
		return 1;
	}
	r3_log_fmt(SUCCESS_LOG, "copy_string_at succeeded, buffer='%s'\n", buffer);

	// 11. Test combine
	String str_copy3;
	if (!r3_alloc_string(64, &str_copy3)) {
		r3_log(ERROR_LOG, "failed to alloc str_copy3\n");
		return 1;
	}
	const char* prefix = "prefix_";
	if (!r3_write_string((u16)strlen(prefix), (char*)prefix, &str_copy3)) {
		r3_log(ERROR_LOG, "failed to write prefix_\n");
		return 1;
	}

	if (!r3_combine_string(&str_copy3, &str_copy)) {
		r3_log(ERROR_LOG, "failed to combine str_copy3 and str_copy\n");
		return 1;
	}
	memset(buffer, 0, sizeof(buffer));
	if (!r3_read_string(r3_string_field(COUNT_FIELD, &str_copy3), buffer, &str_copy3)) {
		r3_log(ERROR_LOG, "failed to read str_copy3 after combine\n");
		return 1;
	}
	if (memcmp(buffer, "prefix_hello TESTd", 18) != 0) {
		r3_log_fmt(ERROR_LOG, "combine failed, buffer='%s'\n", buffer);
		return 1;
	}
	r3_log_fmt(SUCCESS_LOG, "combine succeeded, buffer='%s'\n", buffer);

	// 8. Deallocate
	r3_dealloc_string(&str);
	r3_dealloc_string(&str_copy);
	r3_log(INFO_LOG, "deallocated both strings\n");

	r3_log(SUCCESS_LOG, "=== r3ktstr Unit Test PASSED ===\n");
	return 0;
}
