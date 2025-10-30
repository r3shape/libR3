#ifndef __ECX_H__
#define __ECX_H__

#include <include/libR3/ds/array.h>

#define ECX_MEMORY          2ULL * GiB
#define ECX_FIELD_MAX       0xFF
#define ECX_ENTITY_MAX      0xFFFFFFFF
#define ECX_COMPONENT_MAX   0x40

typedef u16 ECXQuery;
typedef u64 ECXEntity;
typedef u16 ECXComponent;

typedef struct ECXFieldDesc {
    u16 stride;     // element stride
    char* hash;       // TODO: string-name hash for hash array lookups
} ECXFieldDesc;

typedef struct ECXComponentDesc {
    ECXFieldDesc* fieldv;   // field descriptors
    char* hash;             // TODO: string-name hash for hash array lookups
    u8 fields;              // number of fields
    u64 mask;               // identifier bitmask
    u32 max;                // field max
} ECXComponentDesc;

typedef struct ECXQueryDesc {
    u64 any;    // components that MAY be bound
    u64 all;    // components that MUST be bound
    u64 none;   // components that MAY NOT be bound
} ECXQueryDesc;

typedef struct ECXView {
    ptr* fieldSet;
    u16 fieldCount;
} ECXView;

typedef struct ECXComposition {
    ECXView viewSet[ECX_COMPONENT_MAX];
    u8 viewCount;
} ECXComposition;

typedef none (*ECXSystem)(u32 index, ptr user, ECXComposition* comp);

R3_PUBLIC_API ECXEntity ecxNewEntity(none);
R3_PUBLIC_API u8 ecxDelEntity(ECXEntity);

R3_PUBLIC_API ECXComponent ecxNewComponent(ECXComponentDesc comp);
R3_PUBLIC_API u8 ecxDelComponent(ECXComponent comp);

R3_PUBLIC_API ECXQuery ecxQuery(ECXQueryDesc desc);
R3_PUBLIC_API ECXComposition ecxCompose(ECXQuery config);
R3_PUBLIC_API u8 ecxDecompose(ECXQuery config);

R3_PUBLIC_API none ecxIter(ECXQuery config, ECXSystem sys, ptr user);

R3_PUBLIC_API u8 ecxBind(ECXEntity ent, ECXComponent comp);
R3_PUBLIC_API u8 ecxUnbind(ECXEntity ent, ECXComponent comp);

R3_PUBLIC_API ptr ecxGetFieldArray(u8 field, ECXComponent comp);
R3_PUBLIC_API u8 ecxGetField(u8 field, ptr out, ECXEntity ent, ECXComponent comp);

R3_PUBLIC_API u8 ecxSetFieldArray(u8 field, u8 in, ECXComponent comp);
R3_PUBLIC_API u8 ecxSetField(u8 field, ptr in, ECXEntity ent, ECXComponent comp);

R3_PUBLIC_API u8 ecxInit(u64 entityMax);
R3_PUBLIC_API u8 ecxExit(none);

#endif // __ECX_H__