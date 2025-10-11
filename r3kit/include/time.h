#ifndef __R3_TIME_H__
#define __R3_TIME_H__

#include <r3kit/include/defines.h>

R3_API f64 r3_clock_time(void);
R3_API void r3_clock_sleep_ms(u32 ms);
R3_API f64 r3_clock_diff_ms(f64 start, f64 end);

#endif  // __R3_TIME_H__