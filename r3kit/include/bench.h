#ifndef __R3KIT_BENCH_H__
#define __R3KIT_BENCH_H__

#include <r3kit/include/io/log.h>
#include <r3kit/include/time.h>

typedef struct {
    const char* name;
    f64 start;
    f64 end;
    u32 ops;
} r3_bench_t;

static inline none r3_bench_begin(r3_bench_t* b, const char* name, u32 ops) {
    b->name = name;
    b->ops = ops;
    b->start = r3_clock_time();
}

static inline none r3_bench_end(r3_bench_t* b) {
    b->end = r3_clock_time();
    f64 ms = r3_clock_diff_ms(b->start, b->end);
    f64 per_s = (b->ops / (ms / 1000.0));
    r3_log_stdoutf(SUCCESS_LOG, "%-24s %.3f ms (%.1fM/s)\n", b->name, ms, per_s / 1e6);
}

#endif // __R3KIT_BENCH_H__
