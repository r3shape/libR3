#include <include/libR3/ds/array.h>
#include <include/libR3/mem/mem.h>
#include <include/libR3/io/log.h>
#include <include/libR3/ds/soa.h>

typedef struct R3SOAMeta { u16* fstride; u64* foffset; u32 fields; u32 fslots; } R3SOAMeta;

R3Result r3DelSOA(R3SOA* soa) {
    if (!soa || !soa->fields) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `DelSOA` -- invalid `SOA` pointer: (soa)%p (fields)%p\n", soa, soa->fields);
        return R3_RESULT_ERROR;
    }

    ptr rfields = (ptr)((u8*)soa->fields - sizeof(R3SOAMeta));
    r3FreeMemory(rfields);

    return R3_RESULT_SUCCESS;
}

R3Result r3NewSOA(R3SOADesc desc, R3SOA* soa) {
    if (!desc.fslots || !desc.fields || !desc.fieldv) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewSOA` -- invalid `SOADesc` fields: (fslots)%d (fields)%d (fieldv)%p\n", desc.fslots, desc.fields, desc.fieldv);
        return R3_RESULT_ERROR;
    } if (!soa || soa->fields != NULL) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `NewSOA` -- invalid `SOA` pointer: (soa)%p (fields)%p\n", soa, soa->fields);
        return R3_RESULT_ERROR;
    }

    u64 size = 0;
    FOR(u32, f, 0, desc.fields, 1) size += (desc.fslots * desc.fieldv[f].stride);
    r3LogStdOutF(R3_LOG_DEV, "`NewSOA` -- `SOA`: (size)%llu (fields)%d\n", size, desc.fields);

    ptr rfields = r3AllocMemory(sizeof(R3SOAMeta) + size);
    if (!rfields) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewSOA` -- soa field allocation failed\n");
        return R3_RESULT_ERROR;
    }

    ((R3SOAMeta*)rfields)->fields = desc.fields;
    ((R3SOAMeta*)rfields)->fslots = desc.fslots;
    
    ((R3SOAMeta*)rfields)->fstride = r3NewArray(desc.fslots, sizeof(u16));
    ((R3SOAMeta*)rfields)->foffset = r3NewArray(desc.fslots, sizeof(u64));
    if (!((R3SOAMeta*)rfields)->fstride || !((R3SOAMeta*)rfields)->foffset) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewSOA` -- soa fstride/foffset array allocation failed\n");
        return R3_RESULT_ERROR;
    }

    // store soa field offsets/strides
    FOR(u32, f, 0, desc.fields, 1) {
        ((R3SOAMeta*)rfields)->foffset[f] = (desc.fslots * desc.fieldv[f].stride);
        ((R3SOAMeta*)rfields)->fstride[f] = desc.fieldv[f].stride;
    }

    soa->fields = (ptr)((u8*)rfields + sizeof(R3SOAMeta));

    return R3_RESULT_SUCCESS;
}

R3Result r3GetSOA(u32 field, u32 fslot, ptr value, R3SOA* soa) {
    if (!value || !soa || !soa->fields) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `GetSOA` -- invalid value/`SOA` pointer: (value)%p (soa)%p (fields)%p\n", value, soa, soa->fields);
        return R3_RESULT_ERROR;
    }

    R3SOAMeta* m = (R3SOAMeta*)((u8*)soa->fields - sizeof(R3SOAMeta));
    if (field > m->fields || fslot > m->fslots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `GetSOA` -- invalid field/fslot: (field)%d (fslot)%d\n", field, fslot);
        return R3_RESULT_ERROR;
    }

    u16 stride = m->fstride[field];
    u64 offset = m->foffset[field];
    ptr readat = (ptr)((u8*)soa->fields + (offset + (stride * fslot)));
    if (r3ReadMemory(stride, readat, value) != R3_RESULT_SUCCESS) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `GetSOA` -- soa memory read failed: %p\n", readat);
        return R3_RESULT_ERROR;
    }

    return R3_RESULT_SUCCESS;
}

R3Result r3SetSOA(u32 field, u32 fslot, ptr value, R3SOA* soa) {
    if (!value || !soa || !soa->fields) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `SetSOA` -- invalid value/`SOA` pointer: (value)%p (soa)%p (fields)%p\n", value, soa, soa->fields);
        return R3_RESULT_ERROR;
    }

    R3SOAMeta* m = (R3SOAMeta*)((u8*)soa->fields - sizeof(R3SOAMeta));
    if (field > m->fields || fslot > m->fslots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `SetSOA` -- invalid field/fslot: (field)%d (fslot)%d\n", field, fslot);
        return R3_RESULT_ERROR;
    }

    u16 stride = m->fstride[field];
    u64 offset = m->foffset[field];
    ptr writeat = (ptr)((u8*)soa->fields + (offset + (stride * fslot)));
    if (r3WriteMemory(stride, value, writeat) != R3_RESULT_SUCCESS) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `SetSOA` -- soa memory write failed: %p\n", writeat);
        return R3_RESULT_ERROR;
    }

    return R3_RESULT_SUCCESS;
}

R3Result r3RemSOA(u32 field, u32 fslot, R3SOA* soa) {
    if (!soa || !soa->fields) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `RemSOA` -- invalid `SOA` pointer: (soa)%p (fields)%p\n", soa, soa->fields);
        return R3_RESULT_ERROR;
    }

    R3SOAMeta* m = (R3SOAMeta*)((u8*)soa->fields - sizeof(R3SOAMeta));
    if (field > m->fields || fslot > m->fslots) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `RemSOA` -- invalid field/fslot: (field)%d (fslot)%d\n", field, fslot);
        return R3_RESULT_ERROR;
    }

    u16 stride = m->fstride[field];
    u64 offset = m->foffset[field];
    ptr setat = (ptr)((u8*)soa->fields + (offset + (stride * fslot)));
    if (r3SetMemory(stride, 0, setat) != R3_RESULT_SUCCESS) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `SetSOA` -- soa memory set failed: %p\n", setat);
        return R3_RESULT_ERROR;
    }

    return R3_RESULT_SUCCESS;
}
