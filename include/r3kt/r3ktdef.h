#ifndef __R3KTDEF_H__
#define __R3KTDEF_H__

typedef char* 			str;
typedef const char* 		cstr;

typedef void 			none;
typedef void* 			addr;

typedef float 			f32, f4b;
typedef double 			f64, f8b;

typedef signed char 		s8,  s1b;
typedef signed short 		s16, s2b;
typedef signed int 		s32, s4b;
typedef signed long long 	s64, s8b, sptr;

typedef unsigned char 		u8,  u1b;
typedef unsigned short 		u16, u2b;
typedef unsigned int 		u32, u4b;
typedef unsigned long long 	u64, u8b, uptr;

#define kb 1000
#define mb kb * 1000
#define gb mb * 1000

#define kib 1024
#define mib kib * 1024
#define gib mib * 1024

#define I8_MAX  ((1 << 8) - 1)
#define I16_MAX ((1 << 16) - 1)
#define I32_MAX ((1 << 32) - 1)
#define I64_MAX ((1 << 64) - 1)

#define r3_for_i(start, stop, count) for (u32 i = start; i < stop; i += count)

#define r3_for_j(start, stop, count) for (u32 j = start; j < stop; j += count)

#define r3_for_k(start, stop, count) for (u32 k = start; k < stop; k += count)

#ifdef R3KT_BUILD_SOURCE
    #ifdef _MSC_VER
        #define R3KT_API __declspec(dllexport)
    #elif #defined (__GNUC__) || defined (__clang__)
        #define R3KT_API __attribute__((visibility("default")))
    #else
        #define R3KT_API
    #endif
#else
    #ifdef _MSC_VER
        #define R3KT_API __declspec(dllimport)
    #elif defined(__GNUC__) || defined(__clang__)
        #define R3KT_API __attribute__((visibility("default")))
    #else
        #define R3KT_API
    #endif
#endif

#endif // __R3KTDEF_H__
