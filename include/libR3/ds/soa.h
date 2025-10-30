#ifndef __R3_DS_STRUCT_H__
#define __R3_DS_STRUCT_H__

#include <include/libR3/r3def.h>

typedef struct R3ArrayDesc { u16 stride; } R3ArrayDesc;

typedef struct R3SOADesc {
    R3ArrayDesc* fieldv;
    u32 fields;
    u32 fslots;
} R3SOADesc;

typedef struct R3SOA { ptr fields; } R3SOA;

R3_PUBLIC_API R3Result r3DelSOA(R3SOA* soa);
R3_PUBLIC_API R3Result r3NewSOA(R3SOADesc desc, R3SOA* soa);

R3_PUBLIC_API R3Result r3RemSOA(u32 field, u32 fslot, R3SOA* soa);
R3_PUBLIC_API R3Result r3GetSOA(u32 field, u32 fslot, ptr value, R3SOA* soa);
R3_PUBLIC_API R3Result r3SetSOA(u32 field, u32 fslot, ptr value, R3SOA* soa);

#endif // __R3_DS_STRUCT_H__