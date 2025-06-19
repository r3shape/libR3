#ifndef __R3DEF_H__
#define __R3DEF_H__

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

#ifdef R3STD_BUILD_SOURCE
    #ifdef _MSC_VER
        #define R3STD_API __declspec(dllexport)
    #elif #defined (__GNUC__) || defined (__clang__)
        #define R3STD_API __attribute__((visibility("default")))
    #else
        #define R3STD_API
    #endif
#else
    #ifdef _MSC_VER
        #define R3STD_API __declspec(dllimport)
    #elif defined(__GNUC__) || defined(__clang__)
        #define R3STD_API __attribute__((visibility("default")))
    #else
        #define R3STD_API
    #endif
#endif

#endif // __R3DEF_H__
