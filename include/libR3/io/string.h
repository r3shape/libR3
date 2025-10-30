#ifndef __R3_IO_STRING_H__
#define __R3_IO_STRING_H__

#include <include/libR3/r3def.h>

typedef enum R3StringFlag {
    R3_STRING_IMMUTABLE =   ~(1 << 0),
    R3_STRING_MUTABLE =       1 << 0,
    R3_STRING_APPEND =      ~(1 << 1),
    R3_STRING_WRITE =         1 << 1,
    R3_STRING_FLAGS,
} R3StringFlag;

typedef struct R3StringHeader {
    u64 length; // length of the string in bytes
    u64 cursor; // cursor position within the string (used for offsetting API r/w/cp ops)
    u32 rOps;   // number of API read operations performerd on the string (copies count as read ops)
    u32 wOps;   // number of API write operations performed on the string (clears count as write ops)
    u64 mask;   // internal API string mask (e.g terminated, mutable, w/a modes, etc...)
} R3StringHeader;

R3_PUBLIC_API R3Result r3FlagString(R3StringFlag flag, char* string);
R3_PUBLIC_API R3Result r3StringHeader(R3StringHeader* header, char* string);

R3_PUBLIC_API char* r3NewString(u64 length);
R3_PUBLIC_API R3Result r3DelString(char* string);

R3_PUBLIC_API R3Result r3SeekString(u64 bytes, char* string);
R3_PUBLIC_API R3Result r3RewindString(u64 bytes, char* string);

R3_PUBLIC_API R3Result r3ClearString(char* string);
R3_PUBLIC_API R3Result r3CopyString(u64 bytes, char* source, char* dest);
R3_PUBLIC_API R3Result r3ReadString(u64 length, char* source, char* dest);
R3_PUBLIC_API R3Result r3WriteString(u64 length, char* source, char* dest);

#endif // __R3_IO_STRING_H__
