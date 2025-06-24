#include <include/r3kt/r3ktarr.h>
#include <include/r3kt/r3ktlog.h>
#include <stdio.h>

int main() {
	Array arr1;
	if (!r3_alloc_array(4, 2, &arr1)) r3_log(ERROR_LOG, "[r3arr test] failed dynamic array allocation\n");
	else {
		u16 max = r3_array_field(MAX_FIELD, &arr1);
		u16 stride = r3_array_field(STRIDE_FIELD, &arr1);
		r3_log_fmt(SUCCESS_LOG, "[r3arr test] passed dynamic array allocation: (max)%d (stride)%d\n", max, stride);

		// TEST SAFE WRITE
		if (!r3_write_array(0, &(u16){456}, &arr1)) {
			r3_log(ERROR_LOG, "[r3arr test] failed array write\n");
			return 1;
		} else r3_log_fmt(SUCCESS_LOG, "[r3arr test] passed array write: (value)%d\n", ((u16*)arr1.data)[0]);

		// TEST SAFE READ
		u16 value;
		if (!r3_read_array(0, &value, &arr1)) {
			r3_log(ERROR_LOG, "[r3arr test] failed array read\n");
			return 1;
		} else r3_log_fmt(SUCCESS_LOG, "[r3arr test] passed array read: (value)%d\n", value);

		// TEST SAFE PUSH
		if (!r3_push_array(&(u16){654}, &arr1)) {
			r3_log(ERROR_LOG, "[r3arr test] failed array push\n");
			return 1;
		} else r3_log_fmt(SUCCESS_LOG, "[r3arr test] passed array push: (pushed)%d\n", 654);

		u16 count = r3_array_field(COUNT_FIELD, &arr1);
		r3_log_fmt(INFO_LOG, "[r3arr test] array count: %d\n", count);
		
		// TEST SAFE PULL
		if (!r3_pull_array(&value, &arr1)) {
			r3_log(ERROR_LOG, "[r3arr test] failed array pull\n");
		} else r3_log_fmt(SUCCESS_LOG, "[r3arr test] passed array pull: (value)%d\n", value);

		count = r3_array_field(COUNT_FIELD, &arr1);
		r3_log_fmt(INFO_LOG, "[r3arr test] array count: %d\n", count);
		
		// TEST OUT-OF-BOUNDS WRITE
		if (r3_write_array(99, &value, &arr1)) {
		    r3_log(ERROR_LOG, "[r3arr test] unexpected success for OOB read\n");
		    return 1;
		}

		// TEST OUT-OF-BOUNDS READ
		if (r3_read_array(99, &value, &arr1)) {
		    r3_log(ERROR_LOG, "[r3arr test] unexpected success for OOB read\n");
		    return 1;
		}

		// TEST PUSH BEYOND MAX
		for (int i = 0; i < 5; ++i) {
		    u16 v = (i + 1) * 100;
		    if (!r3_push_array(&v, &arr1) && i >= 4) {
			r3_log_fmt(SUCCESS_LOG, "[r3arr test] failed push beyond max at index %d\n", i);
		    }
		}

		// TEST PULL UNTIL EMPTY
		count = r3_array_field(COUNT_FIELD, &arr1);
		for (int i = 0; i <= count; ++i) {
		    if (!r3_pull_array(&value, &arr1) && i >= count) {
			r3_log(SUCCESS_LOG, "[r3arr test] failed pull from empty array\n");
		    }
		}

		if(!r3_dealloc_array(&arr1)) r3_log(ERROR_LOG, "[r3arr test] failed dynamic array deallocation\n");
		else r3_log_fmt(SUCCESS_LOG, "[r3arr test] passed dynamic array deallocation\n");
	}

	return 0;
}

