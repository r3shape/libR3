#ifndef __R3_IO_FILE_H__
#define __R3_IO_FILE_H__

#include <include/libR3/r3def.h>

typedef enum R3FileFlag {
    R3_FILE_IMMUTABLE =   ~(1 << 0),
    R3_FILE_MUTABLE =       1 << 0,
    R3_FILE_APPEND =      ~(1 << 1),
    R3_FILE_WRITE =         1 << 1,
    R3_FILE_FLAGS,
} R3FileFlag;

typedef struct R3FileHeader {
    u64 cursor; // cursor position within the file (used for offsetting API r/w/cp ops)
    u64 size;   // size of the file in bytes
    u32 rOps;   // number of API read operations performerd on the file (copies count as read ops)
    u32 wOps;   // number of API write operations performed on the file (clears count as write ops)
    u64 mask;   // internal API file mask (e.g terminated, mutable, w/a modes, etc...)
} R3FileHeader;

R3_PUBLIC_API R3Result r3FlagFile(R3FileFlag flag, ptr file);
R3_PUBLIC_API R3Result r3FileHeader(R3FileHeader* header, ptr file);

R3_PUBLIC_API ptr r3NewFile(u64 bytes);
R3_PUBLIC_API R3Result r3DelFile(ptr file);

R3_PUBLIC_API R3Result r3SeekFile(u64 bytes, ptr file);
R3_PUBLIC_API R3Result r3RewindFile(u64 bytes, ptr file);

R3_PUBLIC_API R3Result r3LoadFile(char* path, ptr file);
R3_PUBLIC_API R3Result r3SaveFile(u64 bytes, char* path, ptr file);

R3_PUBLIC_API u64 r3SumFileChars(char* file);
R3_PUBLIC_API u64 r3SumFileLines(char* file);
R3_PUBLIC_API char* r3GetFileLine(u64 line, char* file);
R3_PUBLIC_API char* r3GetFileWord(u64 line, u64 word, char* file);

R3_PUBLIC_API R3Result r3ClearFile(ptr File);
R3_PUBLIC_API R3Result r3CopyFile(u64 bytes, ptr source, ptr dest);
R3_PUBLIC_API R3Result r3ReadFile(u64 bytes, ptr source, ptr dest);
R3_PUBLIC_API R3Result r3WriteFile(u64 bytes, ptr source, ptr dest);

#endif // __R3_IO_FILE_H__