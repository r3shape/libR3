#ifndef __R3_DEF_H__
#define __R3_DEF_H__

typedef void* ptr;
typedef void none;

typedef float   f32;
typedef double  f64;

typedef signed char           i8;
typedef signed short          i16;
typedef signed int            i32;
typedef signed long long      i64;

typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
typedef unsigned long long      u64;

typedef enum R3Result {
    R3_RESULT_FATAL =   -1,
    R3_RESULT_ERROR =    0,
    R3_RESULT_SUCCESS =  1
} R3Result;

#ifdef KB
    #undef KB
#endif
#ifdef MB
    #undef MB
#endif
#ifdef GB
    #undef GB
#endif
#define KB 1000
#define MB (KB * 1000)
#define GB (MB * 1000)

#ifdef KiB
    #undef KiB
#endif
#ifdef MiB
    #undef MiB
#endif
#ifdef GiB
    #undef GiB
#endif
#define KiB 1024
#define MiB (KiB * 1024)
#define GiB (MiB * 1024)

#define I8_MAX  ((1 << 8) - 1)
#define I16_MAX ((1 << 16) - 1)
#define I32_MAX ((1ULL << 32) - 1)
#define I64_MAX ((1ULL << 63) - 1)

#define PI 3.14159265358979323846

#define FPTR(r, n, ...) r (*n)(__VA_ARGS__)

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define MAX3(a, b, c) MAX((a), MAX((b), (c)))
#define MIN3(a, b, c) MIN((a), MIN((b), (c)))

#define MAX4(a, b, c, d) MAX((a), MAX3((b), (c), (d)))
#define MIN4(a, b, c, d) MIN((a), MIN3((b), (c), (d)))

#define SWAP(t, a, b) { t tmp = a; (a) = (b); (b) = tmp; }

#define CLAMP(v, l, h) (((v) > (h)) ? (h) : ((v) < (l)) ? (l) : (v))

#define FOR_I(start, stop, step) for (u32 i = start; i < stop; i += step)
#define FOR_J(start, stop, step) for (u32 j = start; j < stop; j += step)
#define FOR_K(start, stop, step) for (u32 k = start; k < stop; k += step)

#define FOR(type, iter, start, stop, step) for (type iter = start; iter < stop; iter += step)

#ifndef NULL
    #ifndef _WIN64
        #define NULL 0
        #define R3_ALIGN 4
    #else
        #define NULL 0LL
        #define R3_ALIGN 8
    #endif  /* _WIN64 */
    #else
        #define NULL ((void *)0)
#endif

#ifdef R3_BUILD_DLL
    #ifdef _MSC_VER
        #define R3_PUBLIC_API __declspec(dllexport)
    #elif #defined (__GNUC__) || defined (__clang__)
        #define R3_PUBLIC_API __attribute__((visibility("default")))
    #else
        #define R3_PUBLIC_API
    #endif
#else
    #ifdef _MSC_VER
        #define R3_PUBLIC_API __declspec(dllimport)
    #elif defined(__GNUC__) || defined(__clang__)
        #define R3_PUBLIC_API __attribute__((visibility("default")))
    #else
        #define R3_PUBLIC_API
    #endif
#endif

#endif // __R3_DEF_H__