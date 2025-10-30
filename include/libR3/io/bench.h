#ifndef __R3KIT_BENCH_H__
#define __R3KIT_BENCH_H__

#include <include/libR3/io/log.h>

typedef struct {
    const char* name;
    f64 start;
    f64 end;
    u32 ops;
} R3Bench;

R3_PUBLIC_API f64 r3ClockTime(void);
R3_PUBLIC_API void r3ClockSleepMs(u32 ms);
R3_PUBLIC_API f64 r3ClockDiffMs(f64 start, f64 end);

static inline none r3BeginBench(R3Bench* b, const char* name, u32 ops) {
    b->name = name;
    b->ops = ops;
    b->start = r3ClockTime();
}

static inline none r3EndBench(R3Bench* b) {
    b->end = r3ClockTime();
    f64 ms = r3ClockDiffMs(b->start, b->end);
    f64 per_s = (b->ops / (ms / 1000.0));
    
    r3LogStdOutF(R3_LOG_OK, "%-24s %.3f ms (%.1fM/s)\n", b->name, ms, per_s / 1e6);
}

#endif // __R3KIT_BENCH_H__
