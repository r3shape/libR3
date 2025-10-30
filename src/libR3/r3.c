/* libR3 ubuild */

/* MATH API INCLUDES */
#include "math/math.c"

/* MEM API INCLUDES */
#include "mem/alloc.c"
#include "mem/mem.c"

/* IO API INCLUDES */
#include "io/string.c"
#include "io/file.c"
#include "io/log.c"

/* DS API INCLUDES */
#include "ds/harray.c"
#include "ds/array.c"
#include "ds/tree.c"
#include "ds/soa.c"

/* BENCHMARK API IMPL */
#ifdef _WIN64               // WINDOWS 64-bit
#include <windows.h>

f64 r3ClockTime(void) {
    static LARGE_INTEGER freq;
    static BOOL initialized = FALSE;
    
    if (!initialized) {
        QueryPerformanceFrequency(&freq);
        initialized = TRUE;
    }
    
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    return (f64)now.QuadPart / (f64)freq.QuadPart;
}

f64 r3ClockDiffMs(f64 start, f64 end) { return (end - start) * 1000.0; }

void r3ClockSleepMs(u32 ms) { Sleep(ms); }
#endif // _WIN64
