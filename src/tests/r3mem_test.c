#include <include/r3std/r3mem.h>
#include <stdio.h>

int main() {
	u64 size = 1024;
	addr p = r3_alloc(size, 8);
	if (p) {
		printf("allocated memory: %llu bytes\n", size);
	} else {
		printf("failed to allocate memory: %llu bytes\n", size);
		return 1;
	}

	if ((p = r3_realloc(size * 2, 8, p))) {
		printf("reallocated memory: %llu bytes\n", size*2);
	} else {
		printf("failed to reallocate memory: %llu bytes\n", size);
		return 1;
	}

	r3_dealloc(p);

	printf("r3mem test passed!\n");
	return 0;
}

