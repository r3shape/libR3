#include <r3kit/include/time.h>

#ifdef _WIN64
#include <windows.h>

R3_API f64 r3_clock_time(void) {
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

R3_API f64 r3_clock_diff_ms(f64 start, f64 end) {
    return (end - start) * 1000.0;
}

R3_API void r3_clock_sleep_ms(u32 ms) {
    Sleep(ms);
}
#endif // _WIN64
