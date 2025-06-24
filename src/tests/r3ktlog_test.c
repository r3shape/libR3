#include <include/r3kt/r3ktlog.h>
#include <stdio.h>

int main() {
	r3_log(INFO_LOG, "Info Message\n");
	r3_log(WARN_LOG, "Warn Message\n");
	r3_log(TRACE_LOG, "Trace Message\n");
	r3_log(ERROR_LOG, "Error Message\n");
	r3_log(SUCCESS_LOG, "Success Message\n");
	
	r3_log_fmt(INFO_LOG, "Info Message: d%dd%ds\n", 34, 0);
	r3_log_fmt(WARN_LOG, "Warn Message: d%dd%ds\n", 34, 0);
	r3_log_fmt(TRACE_LOG, "Trace Message: d%dd%ds\n", 43, 0);
	r3_log_fmt(ERROR_LOG, "Error Message: d%dd%ds\n", 34, 0);
	r3_log_fmt(SUCCESS_LOG, "Success Message: d%dd%ds\n", 34, 0);

	printf("r3ktlog test passed!\n");
	return 0;
}

