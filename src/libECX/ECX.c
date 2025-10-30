/*
    ECX Proof of concept using r3kit ds/arr API for better funtionality and saftey
    @zafflins - 9/3/2025
*/

#include <include/libR3/mem/alloc.h>
#include <include/libR3/mem/mem.h>
#include <include/libR3/io/log.h>
#include <include/libECX/ecx.h>

static struct ECX {
    u8 init;
    R3Allocator arena;  // all component field arrays are stored here

    // entity ID layout: 32bits | 16bits | 15bits | 1bit
    //                      ID      GEN     SALT    ALIVE
    // entity internal arrays alloced statically
    struct entity {
        u32 next;          // next entity handle
        u32 count;         // alive entity count
        u16* gen;          // array of entity generations
        u32* free;         // array of free entity handles
        u64* mask;         // array of entity component masks
    } entity;

    // component internal arrays alloced statically
    struct component {
        u8 next;                // array of next component handle
        u8 count;               // number of components
        u8* free;               // array of free component handles

        u8*  gen;              // array of component generations
        u64* mask;             // array of component masks
        u32* hash;             // array of component hashes
        ptr* field;            // array of component field buffer addresses | [position=[x1, x2, x3 | y1, y2, y3 | z1, z2, z3], color=[r1,r2,r3 | g1,g2,g3 | b1,b2,b3] ...]
        u32* fieldMax;         // array of component field maximums
        u32* fieldSize;        // array of component field sizes
        u32* fieldHash;        // array of component field hashes
        u8*  fieldCount;       // array of component field counts
        u16** fieldStride;     // array of component field strides
        u16** fieldOffset;     // array of component field offsets
    } component;

    // query internal arrays will grow dynamically up to max (alloced at min)
    struct query {
        u16 next;               // array of next query handle
        u16 count;              // number of query handles
        u8* free;               // array of free query handles
        
        u64* all;               // array of config `all` masks
        u64* any;               // array of config `any` masks
        u64* none;              // array of config `none` masks
        u32* seen;              // number of times a query was computed
        u16* config;            // config IDs offset by query | [ position+color+velocity(0, 1, 2, 3, 4, 5) | position+color(3, 1, 5) ]
        u8*  cached;            // boolean flag indicating query caching
        u32** localToGlobal;    // ltg[local] = global (query)
        u32** globalToLocal;    // gtl[global] = local (query)
    } query;

    // config internal arrays will grow dynamically up to max (alloced at min)
    struct config {
        u16 next;               // array of nect config handle
        u16 count;              // number of config handles
        u16* free;              // array of free config handles

        u64* signature;         // array of config signatures (XOR hash of component masks)  | [ position+color+velocity(0, 1, 2, 3, 4, 5) | position+color(3, 1, 5) ]
        u32** entitySet;        // array of entity IDs
        u32* entityCount;       // array of entity counts
        u8** componentSet;      // array of component IDs
        u8* componentCount;     // array of component counts
        ptr*** fieldSet;        // array of component field address arrays
        u32** localToGlobal;    // ltg[local] = global (config)
        u32** globalToLocal;    // gtl[global] = local (config)
    } config;
} ECX = {0};

// and here we have batshit ~@zafflins 9/15/25
static inline ECXEntity _packEntity(ECXEntity e, u16 g, u16 s, u8 a) {
    return ((e & 0xFFFFFFFF)   << 32)  |
           ((g & 0xFFFF)       << 16)  |
           ((s & 0x7FFF)       << 1)   |
           ((a & 0x1)          << 0)   ;
} static inline u32 _entityID(ECXEntity e) {
    return ((e >> 32) & 0xFFFFFFFF);
} static inline u16 _entityGEN(ECXEntity e) {
    return ((e >> 16) & 0xFFFF);
} static inline u16 _entitySALT(ECXEntity e) {
    return ((e >> 1) & 0x7FFF);
} static inline u8 _entityALIVE(ECXEntity e) {
    return ((e >> 0) & 0x1);
} static inline ECXComponent _packComponent(ECXComponent c, u8 g) {
    return ((c & 0xFF) << 8) | ((g & 0xFF) << 0);
} static inline u16 _componentID(ECXComponent c) {
    return ((c >> 8) & 0xFF);
} static inline u16 _componentGEN(ECXComponent c) {
    return ((c >> 0) & 0xFF);
}

// fnv1a
static inline u32 _hashv1(char* v) {
	if (v) { u32 o = 2166136261u;
        do { o ^= (char)*v++; o *= 16777619u; }
        while (*v); return o;
    } else return I32_MAX;
}

ECXEntity ecxNewEntity(none) {
    u32 id = 0;
    u8 freed = 0;
    if (r3ArrayCount(ECX.entity.free)) {
        if (!r3PopArray(ECX.entity.free, &id)) {
            r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `newEntity` -- entity internal array pop failed\n");
            return 0;
        } freed = 1;
    } else { id = ++ECX.entity.next; }
    
    u16 gen = 0;
    if (!r3GetArray(id - 1, ECX.entity.gen, &gen)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `newEntity` -- entity internal array read failed\n");
        if (freed) {
            if (!r3PushArray(&id, ECX.entity.free)) {
                r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `newEntity` -- entity internal array push failed\n");
                return 0;
            }
        } else { --ECX.entity.next; }
        return 0;
    }
    
    ECX.entity.count++;
    return _packEntity(id, gen, 1234, 1);
}

u8 ecxDelEntity(ECXEntity entity) {
    u32 id = _entityID(entity);
    u16 gen = _entityGEN(entity);
    if (!id || id > ECX.entity.next || gen != ECX.entity.gen[id - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `delEntity` -- invalid entity: (id)%d (gen)%d\n", id, gen);
        return 1;
    }
    
    gen++; // bump generation
    if (!r3PushArray(&id, ECX.entity.free)           ||
        !r3SetArray(id - 1, &gen, ECX.entity.gen)    ||
        !r3SetArray(id - 1, &(u64){0}, ECX.entity.mask)) {   // clear entity-component mask
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `delEntity` -- entity internal array push/write failed\n");
        return 0;
    }

    ECX.entity.count--;
    return 1;
}


ECXComponent ecxNewComponent(ECXComponentDesc comp) {
    // validate descriptor
    // support "tag" components with no fields by requiring field descs if a field count is passed
    if (!comp.max || comp.max > ECX_ENTITY_MAX  ||
        (comp.fields && !comp.fieldv)           ||
        !comp.mask) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `newComponent` -- invalid component descriptor\n");
        return 0;
    }

    u8 id = 0;
    u8 freed = 0;
    if (r3ArrayCount(ECX.component.free)) {
        if (!r3PopArray(ECX.component.free, &id)) {
            r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `newComponent` -- component internal array pop failed\n");
            return 0;
        } freed = 1;
    } else { id = ++ECX.component.next; }
    
    u8 result = 1;
    // allocate and assign field arrays from internal arena
    if (!r3AssignArray(id - 1, ECX.arena.alloc(sizeof(u32) * comp.fields, &ECX.arena), ECX.component.fieldHash)   ||
        !r3AssignArray(id - 1, ECX.arena.alloc(sizeof(u16) * comp.fields, &ECX.arena), ECX.component.fieldStride) ||
        !r3AssignArray(id - 1, ECX.arena.alloc(sizeof(u16) * comp.fields, &ECX.arena), ECX.component.fieldOffset)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `newComponent` -- component internal array assignment failed\n");
        result = 0;
    }

    u32* fieldHashes;
    u16* fieldStrides;
    u16* fieldOffsets;
    if (!r3GetArray(id - 1, ECX.component.fieldHash, &fieldHashes)     ||
        !r3GetArray(id - 1, ECX.component.fieldStride, &fieldStrides)  ||
        !r3GetArray(id - 1, ECX.component.fieldOffset, &fieldOffsets)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `newComponent` -- component internal array read failed\n");
        result = 0;
    }
    
    if (!r3SetArray(id - 1, &comp.mask, ECX.component.mask)        ||
        !r3SetArray(id - 1, &comp.hash, ECX.component.hash)        ||
        !r3SetArray(id - 1, &comp.max, ECX.component.fieldMax)     ||
        !r3SetArray(id - 1, &comp.fields, ECX.component.fieldCount)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `newComponent` -- component internal array write failed\n");
        result = 0;
    }
    
    u64 fsize = 0;
    FOR(u8, field, 0, comp.fields, 1) {
        fieldHashes[field] = _hashv1(comp.fieldv[field].hash);
        fieldOffsets[field] = fsize;
        fieldStrides[field] = comp.fieldv[field].stride;
        
        // accumulate field offsets
        fsize += comp.fieldv[field].stride * comp.max;
    }
    
    // allocate and assign field buffer from internal arena -- or fallback from accumulated fails
    if (!r3SetArray(id - 1, &fsize, ECX.component.fieldSize)                            ||
        !r3AssignArray(id - 1, ECX.arena.alloc(fsize, &ECX.arena), ECX.component.field)  || !result) {
        // component fallback -- read out field data for dealloc and return handle
        r3LogStdOut(R3_LOG_ERROR, "[ECX] `newComponent` fallback\n");
        ptr fieldHash = 0;
        ptr fieldStride = 0;
        ptr fieldOffset = 0;
        if (!(r3GetArray(id - 1, ECX.component.fieldHash, &fieldHash) && r3GetArray(id - 1, ECX.component.fieldStride, &fieldStride) && r3GetArray(id - 1, ECX.component.fieldOffset, &fieldOffset))   ||
            !ECX.arena.free(fieldHash, &ECX.arena)         ||
            !ECX.arena.free(fieldStride, &ECX.arena)       ||
            !ECX.arena.free(fieldOffset, &ECX.arena)       ||
            !r3PushArray(&id, ECX.component.free)) {
            r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `newComponent` fallback -- component internal arena dealloc/array push failed\n");
        }
        return 0;
    }

    u8 gen = 0;
    if (!r3GetArray(id - 1, ECX.component.gen, &gen)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `newComponent` -- component internal array read failed\n");
        if (freed) {
            if (!r3PushArray(&id, ECX.component.free)) {
                r3LogStdOut(R3_LOG_ERROR, "[ECX] Failed `newComponent` -- component internal array push failed\n");
                return 0;
            }
        } else { --ECX.component.next; }
        return 0;
    }
    
    ECX.component.count++;
    return _packComponent(id, gen);
}

u8 ecxDelComponent(ECXComponent comp) {
    u8 cid = _componentID(comp);
    u8 gen = _componentGEN(comp);
    if (!cid || cid > ECX.component.next || gen != ECX.component.gen[cid - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `delComponent` -- invalid component: (id)%d (gen)%d\n", cid, gen);
        return 1;
    }
    
    // read out field data for dealloc
    u8 fieldCount = 0;
    u64 fieldSize = 0;
    ptr field = 0;
    ptr fieldHash = 0;
    ptr fieldStride = 0;
    ptr fieldOffset = 0;
    if (!(r3GetArray(cid - 1, ECX.component.fieldSize, &fieldSize) && r3GetArray(cid - 1, ECX.component.fieldCount, &fieldCount))
    ||  !(r3GetArray(cid - 1, ECX.component.field, &field) && r3GetArray(cid - 1, ECX.component.fieldHash, &fieldHash))
    ||  !(r3GetArray(cid - 1, ECX.component.fieldStride, &fieldStride) && r3GetArray(cid - 1, ECX.component.fieldOffset, &fieldOffset))
    ||  (!fieldSize && fieldCount) || (!field || !fieldHash || !fieldStride || !fieldOffset)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during `delComponent` -- component internal array read failed\n");
        return 0;
    }
    
    if (!ECX.arena.free(field, &ECX.arena)                          ||
        !r3SetArray(cid - 1, &(u8){0}, ECX.component.mask)          ||
        !r3SetArray(cid - 1, &(u8){0}, ECX.component.hash)          ||
        !r3SetArray(cid - 1, &(u8){0}, ECX.component.fieldMax)      ||
        !r3SetArray(cid - 1, &(u8){0}, ECX.component.fieldSize)     ||
        !r3SetArray(cid - 1, &(u8){0}, ECX.component.fieldCount)    ||
        !ECX.arena.free(fieldHash, &ECX.arena)                      ||
        !ECX.arena.free(fieldStride, &ECX.arena)                    ||
        !ECX.arena.free(fieldOffset, &ECX.arena)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during `delComponent` -- component internal array write failed\n");
        return 0;
    }

    // update now invalid configs
    FOR_I(0, ECX.config.count, 1) {
        u8* compSet = ECX.config.componentSet[i];
        u8 compCount = ECX.config.componentCount[i];
        u8 removed = 0;

        // swap-remove component
        FOR_J(0, compCount, 1) {
            if (compSet[j] == cid) {
                compSet[j] = compSet[--compCount];
                ECX.config.componentCount[i] = compCount;
                removed = 1;
                break;
            }
        }
        
        // free cached ECXComposition data if no components in configuration
        if (compCount == 0) ecxDecompose(i + 1);
        
        // update configuration signature
        if (removed) {
            // free cached ECXComposition data if component removed
            ptr** cached = ((ptr***)ECX.config.fieldSet)[i];
            if (cached) ecxDecompose(i + 1);

            u64 newSig = 0;
            FOR_K(0, compCount, 1) newSig ^= ECX.component.mask[compSet[k] - 1];
            ECX.config.signature[i] = newSig;
        }
    }

    // update now invalid masks
    FOR_I(0, ECX.query.count, 1) {
        u64 cmask = ECX.component.mask[cid - 1];
        ECX.query.all[i]  &= ~cmask;
        ECX.query.any[i]  &= ~cmask;
        ECX.query.none[i] &= ~cmask;
    }


    gen++; // bump generation
    if (!r3PushArray(&cid, ECX.component.free) ||
        !r3SetArray(cid - 1, &gen, ECX.component.gen)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during `delComponent` -- component internal array push failed\n");
        return 0;
    }

    ECX.component.count--;
    return 1;
}


u8 ecxSetField(u8 field, ptr in, ECXEntity entity, ECXComponent comp) {
    u32 entityId = _entityID(entity);
    u16 entityGen = _entityGEN(entity);
    if (!entityId || entityId > ECX.entity.next || entityGen != ECX.entity.gen[entityId - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `setField` -- invalid entity: (id)%d (gen)%d\n", entityId, entityGen);
        return 0;
    }

    u8 componentId = _componentID(comp);
    u8 componentGen = _componentGEN(comp);
    if (!componentId || componentId > ECX.component.next               ||
        componentGen != ECX.component.gen[componentId - 1] ||
        field >= ECX.component.fieldCount[componentId - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `setField` -- invalid component/field: (id)%d (gen)%d (field)%d\n", componentId, componentGen, field);
        return 0;
    }

    u64 entityMask = ECX.entity.mask[entityId - 1];
    u64 componentMask = ECX.component.mask[componentId - 1];
    if ((entityMask & componentMask) != componentMask) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `setField` -- component not bound to entity: (id)%d (gen)%d\n", entityId, entityGen);
        return 0;
    }
    
    ptr fields = ECX.component.field[componentId - 1];
    u16* fieldStride = ECX.component.fieldStride[componentId - 1];
    u16* fieldOffset = ECX.component.fieldOffset[componentId - 1];

    ptr f = (ptr)((u8*)fields + fieldOffset[field]);
    ptr slot = (ptr)((u8*)f + (fieldStride[field] * (entityId - 1)));
    if (!r3WriteMemory(fieldStride[field], in, slot)) {
        r3LogStdOutF(R3_LOG_ERROR, "[ECX] Failed `setField` -- memory write to component field failed: (id)%d (gen)%d (field)%d\n", componentId, componentGen, field);
        return 0;
    }
    
    return 1;
}

u8 ecxGetField(u8 field, ptr out, ECXEntity entity, ECXComponent comp) {
    u32 entityId = _entityID(entity);
    u16 entityGen = _entityGEN(entity);
    if (!entityId || entityId > ECX.entity.next || entityGen != ECX.entity.gen[entityId - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `getField` -- invalid entity: (id)%d (gen)%d\n", entityId, entityGen);
        return 0;
    }
    
    u8 componentId = _componentID(comp);
    u8 componentGen = _componentGEN(comp);
    if (!componentId || componentId > ECX.component.next               ||
        componentGen != ECX.component.gen[componentId - 1] ||
        field >= ECX.component.fieldCount[componentId - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `getField` -- invalid component/field: (id)%d (gen)%d (field)%d\n", componentId, componentGen, field);
        return 0;
    }

    u64 entityMask = ECX.entity.mask[entityId - 1];
    u64 componentMask = ECX.component.mask[componentId - 1];
    if ((entityMask & componentMask) != componentMask) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `getField` -- component not bound to entity: (id)%d (gen)%d\n", entityId, entityGen);
        return 0;
    }
    
    ptr fields = ECX.component.field[componentId - 1];
    u16* fieldStride = ECX.component.fieldStride[componentId - 1];
    u16* fieldOffset = ECX.component.fieldOffset[componentId - 1];

    ptr f = ((u8*)fields + fieldOffset[field]);
    ptr slot = ((u8*)f + (fieldStride[field] * (entityId - 1)));
    if (!r3ReadMemory(fieldStride[field], slot, out)) {
        r3LogStdOutF(R3_LOG_ERROR, "[ECX] Failed `getField` -- memory read from component field failed: (id)%d (gen)%d (field)%d\n", componentId, componentGen, field);
        return 0;
    }
    
    return 1;
}


u8 ecxSetFieldArray(u8 field, u8 in, ECXComponent comp) {
    u8 componentId = _componentID(comp);
    u8 componentGen = _componentGEN(comp);
    if (!componentId || componentId > ECX.component.next               ||
        componentGen != ECX.component.gen[componentId - 1] ||
        field >= ECX.component.fieldCount[componentId - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `setFieldArray` -- invalid component/field: (id)%d (gen)%d (field)%d\n", componentId, componentGen, field);
        return 0;
    }
    
    ptr fields = ECX.component.field[componentId - 1];
    u32 fieldMax = ECX.component.fieldMax[componentId - 1];
    u16* fieldStride = ECX.component.fieldStride[componentId - 1];
    u16* fieldOffset = ECX.component.fieldOffset[componentId - 1];

    ptr f = ((u8*)fields + fieldOffset[field]);
    if (!r3SetMemory(fieldStride[field] * fieldMax, in, f)) {
        r3LogStdOutF(R3_LOG_ERROR, "[ECX] Failed `setFieldArray` -- memory set from component field failed: (id)%d (gen)%d (field)%d\n", componentId, componentGen, field);
        return 0;
    }
    
    return 1;
}

ptr ecxGetFieldArray(u8 field, ECXComponent comp) {
    u8 componentId = _componentID(comp);
    u8 componentGen = _componentGEN(comp);
    if (!componentId || componentId > ECX.component.next               ||
        componentGen != ECX.component.gen[componentId - 1] ||
        field >= ECX.component.fieldCount[componentId - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `getFieldArray` -- invalid component/field: (id)%d (gen)%d (field)%d\n", componentId, componentGen, field);
        return 0;
    }

    ptr fields = ECX.component.field[componentId - 1];
    u16* fieldOffset = ECX.component.fieldOffset[componentId - 1];

    return (ptr)((u8*)fields + fieldOffset[field]);
}


u8 ecxBind(ECXEntity entity, ECXComponent comp) {
    u32 eid = _entityID(entity);
    u16 egen = _entityGEN(entity);
    if (!eid || eid > ECX.entity.next || egen != ECX.entity.gen[eid - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `bind` -- invalid entity: (id)%d (gen)%d\n", eid, egen);
        return 0;
    }

    u8 cid = _componentID(comp);
    u8 cgen = _componentGEN(comp);
    if (!cid || cid > ECX.component.next || cgen != ECX.component.gen[cid - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `bind` -- invalid component: (id)%d (gen)%d\n", cid, cgen);
        return 0;
    }

    u64* entityMask = ECX.entity.mask;
    u64 componentMask = ECX.component.mask[cid - 1];
    if ((entityMask[eid - 1] & componentMask) == componentMask) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `bind` -- component not bound to entity: (id)%d (gen)%d\n", eid, egen);
        return 1;
    }

    // apply bind
    entityMask[eid - 1] |= componentMask;

    // incremental update into queries/configs
    for (u16 q = 0; q < ECX.query.count; ++q) {
        u64 all  = ECX.query.all[q];
        u64 any  = ECX.query.any[q];
        u64 none = ECX.query.none[q];

        if (((entityMask[eid - 1] & all) == all) && ((entityMask[eid - 1] & none) == 0) && (!any || (entityMask[eid - 1] & any))) {
            ECXQuery query = ECX.query.config[q];
            u16 config = ECX.query.config[query - 1];

            u32* entitySet = ECX.config.entitySet[config - 1];
            u32 entityCount = ECX.config.entityCount[config - 1];
            
            // skip if already in set
            u32* gtl = ECX.config.globalToLocal[config - 1];
            if (gtl[eid] < entityCount && entitySet[gtl[eid]] == eid)
                continue;

            // add to dense array (O(1) append)
            entitySet[entityCount] = eid - 1;
            ECX.config.localToGlobal[config - 1][entityCount] = eid;
            gtl[eid] = (entityCount)++;
        }
    }

    return 1;
}

u8 ecxUnbind(ECXEntity entity, ECXComponent comp) {
    u32 eid = _entityID(entity);
    u16 egen = _entityGEN(entity);
    if (!eid || eid > ECX.entity.next || egen != ECX.entity.gen[eid - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `unbind` -- invalid entity: (id)%d (gen)%d\n", eid, egen);
        return 0;
    }

    u8 cid = _componentID(comp);
    u8 cgen = _componentGEN(comp);
    if (!cid || cid > ECX.component.next || cgen != ECX.component.gen[cid - 1]) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `unbind` -- invalid component: (id)%d (gen)%d\n", cid, cgen);
        return 0;
    }

    u64* entityMask = ECX.entity.mask;
    u64 componentMask = ECX.component.mask[cid - 1];
    if ((entityMask[eid - 1] & componentMask) != componentMask) {
        r3LogStdOutF(R3_LOG_WARN, "[ECX] Skipping `unbind` -- component not bound to entity: (id)%d (gen)%d\n", eid, egen);
        return 1;
    }

    // apply unbind
    entityMask[eid - 1] &= ~componentMask;

    // update now invalid configs
    for (u16 q = 0; q < ECX.query.count; ++q) {
        u64 all  = ECX.query.all[q];
        u64 any  = ECX.query.any[q];
        u64 none = ECX.query.none[q];

        if (!((entityMask[eid - 1] & all) == all && ((entityMask[eid - 1] & none) == 0) && (!any || (entityMask[eid - 1] & any)))) {
            ECXQuery query = ECX.query.config[q];
            u16 config = ECX.query.config[query - 1];

            u32* entitySet = ECX.config.entitySet[config - 1];
            u32 entityCount = ECX.config.entityCount[config - 1];
            
            u32* gtl = ECX.config.globalToLocal[config - 1];
            u32* ltg = ECX.config.localToGlobal[config - 1];

            u32 idx = gtl[eid];
            if (idx >= entityCount) continue;

            // swap-remove O(1)
            u32 last = entitySet[--(entityCount)];
            entitySet[idx] = last;
            ltg[idx] = last;
            gtl[last] = idx;
        }
    }

    return 1;
}


ECXQuery ecxQuery(ECXQueryDesc desc) {
    u64 all  = desc.all;
    u64 any  = desc.any;
    u64 none = desc.none;

    // Compute signature (used for reuse / caching)
    u64 signature = (all ^ (any << 21) ^ (none << 42));

    // probe exisiting queries
    for (u16 i = 0; i < ECX.query.count; ++i) {
        u64 qAll  = ECX.query.all[i];
        u64 qAny  = ECX.query.any[i];
        u64 qNone = ECX.query.none[i];
        if (qAll == all && qAny == any && qNone == none) {
            // TODO: cache a query based on seen count, no need to store one-off queries + configs :)
            // already exists → mark seen
            ECX.query.seen[i]++;
            return ECX.query.config[i];
        }
    }

    // alloc new query
    ECXQuery qid = ++ECX.query.next;
    ++ECX.query.count;

    r3SetArray(qid - 1, &all,  ECX.query.all);
    r3SetArray(qid - 1, &any,  ECX.query.any);
    r3SetArray(qid - 1, &none, ECX.query.none);
    r3SetArray(qid - 1, &(u32){1}, ECX.query.seen);
    r3SetArray(qid - 1, &(u8){0},  ECX.query.cached);

    // alloc new config
    u16 cid = ++ECX.config.next;
    ++ECX.config.count;

    r3SetArray(cid - 1, &signature, ECX.config.signature); // all mask as the "signature" component set

    // alloc + compute entity set
    u32 entityCount = 0;
    FOR(u32, e, 0, ECX.entity.next, 1) {
        u64 mask = ECX.entity.mask[e];
        if (((mask & all) == all) && ((mask & none) == 0) && (!any || (mask & any))) {
            entityCount++;
        }
    } if (!entityCount) {
        ECX.query.next--;
        ECX.config.next--;
        ECX.query.count--;
        ECX.config.count--;
        r3LogStdOutF(R3_LOG_ERROR, "Faield `query` -- enitty+component configuration not found: (signature)%llu\n", signature);
        return 0;
    }

    u32* entitySet = r3AllocMemory(entityCount * sizeof(u32));
    if (!entitySet) {
        r3LogStdOutF(R3_LOG_ERROR, "Faield `query` -- config entitySet allocation failed: (ecount)%d\n", entityCount);
        return 0;
    }

    FOR(u32, e, 0, entityCount, 1) {
        u64 mask = ECX.entity.mask[e];
        if (((mask & all) == all) && ((mask & none) == 0) && (!any || (mask & any))) {
            entitySet[e] = e;
        }
    }

    r3AssignArray(cid - 1, entitySet, ECX.config.entitySet);
    r3SetArray(cid - 1, &entityCount, ECX.config.entityCount);

    // alloc + compute component set
    u8* componentSet = r3AllocMemory(ECX.component.count * sizeof(u8));
    if (!componentSet) {
        ECX.query.next--;
        ECX.config.next--;
        ECX.query.count--;
        ECX.config.count--;
        r3FreeMemory(entitySet);
        r3LogStdOut(R3_LOG_ERROR, "Faield `query` -- config componentSet allocation failed\n");
        return 0;
    }
    
    u8 componentCount = 0;
    FOR(u8, c, 0, ECX.component.next, 1) {
        if (ECX.component.mask[c] & all) { // component participates in query
            componentSet[componentCount++] = c + 1; // ECX handles are index + 1
        }
    }

    r3AssignArray(cid - 1, componentSet, ECX.config.componentSet);
    r3SetArray(cid - 1, &componentCount, ECX.config.componentCount);

    // 'link' query -> config
    r3SetArray(qid - 1, &cid, ECX.query.config);
    ECX.query.cached[qid - 1] = 1;

    // alloc per-query local/global maps
    u32* ltg = r3AllocMemory((ECX.entity.next + 1) * sizeof(u32));
    u32* gtl = r3AllocMemory((ECX.entity.next + 1) * sizeof(u32));
    if (!ltg || !gtl) {
        r3LogStdOut(R3_LOG_ERROR, "Faield `query` -- config ltg/gtl allocation failed\n");
        r3FreeMemory(componentSet);
        r3FreeMemory(entitySet);
        return 0;
    }

    for (u32 i = 0; i < entityCount; ++i) {
        ltg[i] = entitySet[i];
        gtl[entitySet[i]] = i;
    }
    
    r3AssignArray(qid - 1, ltg, ECX.query.localToGlobal);
    r3AssignArray(qid - 1, gtl, ECX.query.globalToLocal);

    return qid;
}

ECXComposition ecxCompose(ECXQuery query) {
    u16 config = ECX.query.config[query - 1];
    if (!query || query > ECX.query.next || !config || config > ECX.config.next) {
        r3LogStdOutF(R3_LOG_ERROR, "[ECX] invalid configuration for composition -- %d\n", config);
        return (ECXComposition){0};
    }

    u8 *componentSet = ECX.config.componentSet[config - 1];
    u8 componentCount = ECX.config.componentCount[config - 1];
    if (!componentSet || !componentCount) return (ECXComposition){0};

    // check for existing composition
    u8 cached = 0;
    ptr base = NULL;
    if (ECX.config.fieldSet[config - 1] != NULL) {
        cached = 1;
        base = ECX.config.fieldSet[config - 1];
    } else {
        u64 totalFields = 0;
        FOR(u8, c, 0, componentCount, 1) totalFields += ECX.component.fieldCount[componentSet[c] - 1];
        
        u64 size = sizeof(u64) + sizeof(ptr*) * componentCount + sizeof(ptr) * totalFields;
        ptr head = ECX.arena.alloc(size, &ECX.arena);
        *(u64*)head = size;
        
        base = (u8*)head + sizeof(u64);
    }
    
    ptr** fieldSetOuter = (ptr**)base;
    ptr* fieldPool = (ptr*)((u8*)base + sizeof(ptr*) * componentCount);

    ECXComposition comp = { .viewCount = componentCount };
    FOR(u8, c, 0, componentCount, 1) {
        ECXComponent component = componentSet[c];
        ptr fields = ECX.component.field[component - 1];
        u8 fieldCount = ECX.component.fieldCount[component - 1];
        u16* fieldOffset = ECX.component.fieldOffset[component - 1];

        fieldSetOuter[c] = fieldPool;
        FOR(u8, f, 0, fieldCount, 1) *fieldPool++ = (ptr)((u8*)fields + fieldOffset[f]);

        comp.viewSet[c].fieldSet = fieldSetOuter[c];
        comp.viewSet[c].fieldCount = fieldCount;
    }
    
    if (!cached) ECX.config.fieldSet[config - 1] = fieldSetOuter;
    return comp;
}

u8 ecxDecompose(ECXQuery query) {
    u16 config = ECX.query.config[query - 1];
    if (!query || query > ECX.query.next || !config || config > ECX.config.next || !ECX.config.fieldSet[config - 1]) {
        r3LogStdOutF(R3_LOG_ERROR, "[ECX] invalid configuration for decomposition -- %d\n", config);
        return 0;
    }

    ptr head = (ptr)((u8*)ECX.config.fieldSet[config - 1] - sizeof(u64));

    ECX.arena.free(head, &ECX.arena);
    ECX.config.fieldSet[config - 1] = NULL;
    return 1;
}


none ecxIter(ECXQuery query, ECXSystem sys, ptr user) {
    u16 config = ECX.query.config[query - 1];
    if (!sys || !query || query > ECX.query.next || !config || config > ECX.config.next) {
        r3LogStdOutF(R3_LOG_ERROR, "[ECX] Failed `iter` -- invalid system/query configuration: (sys)%p (config)%d\n", sys, query);
        return;
    }
    
    u32* entitySet = ECX.config.entitySet[config - 1];
    u32 entityCount = ECX.config.entityCount[config - 1];
    if (!entitySet || !entityCount) {
        r3LogStdOutF(R3_LOG_ERROR, "[ECX] Failed `iter` -- failed internal query configuration pointer fetch: (config)%d\n", query);
        return;
    }

    ECXComposition comp = ecxCompose(query);
    FOR (u32, e, 0, entityCount, 1) sys(entitySet[e], user, &comp); // avoid copying large compositions via pointer
}


u8 ecxInit(u64 entityMax) {
    if (ECX.init) {
        r3LogStdOut(R3_LOG_WARN, "[ECX] Skipping redundant init\n");
        return 1;
    }
    
    ECX.init = 1;
    // alloc internal arena
    if (r3NewArenaAllocator(ECX_MEMORY, &ECX.arena) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during init -- internal arena alloc failed\n");
        return 0;
    }

    // init entity internal
    ECX.entity.next = 0;
    ECX.entity.count = 0;
    ECX.entity.gen = r3NewArray(entityMax, sizeof(u16));
    ECX.entity.free = r3NewArray(entityMax, sizeof(u32));
    ECX.entity.mask = r3NewArray(entityMax, sizeof(u64));

    if (!entityMax || entityMax > ECX_ENTITY_MAX ||
        !ECX.entity.gen || !ECX.entity.free || !ECX.entity.mask) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during init -- internal entity array alloc failed\n");
        return ecxExit();
    }

    // init component internal
    ECX.component.next = 0;
    ECX.component.count = 0;
    ECX.component.gen = r3NewArray(ECX_COMPONENT_MAX, sizeof(u8));
    ECX.component.free = r3NewArray(ECX_COMPONENT_MAX, sizeof(u8));
    ECX.component.mask = r3NewArray(ECX_COMPONENT_MAX, sizeof(u64));
    ECX.component.hash = r3NewArray(ECX_COMPONENT_MAX, sizeof(u32));
    ECX.component.field = r3NewArray(ECX_COMPONENT_MAX, sizeof(ptr));
    ECX.component.fieldMax = r3NewArray(ECX_COMPONENT_MAX, sizeof(u32));
    ECX.component.fieldSize = r3NewArray(ECX_COMPONENT_MAX, sizeof(u64));
    ECX.component.fieldCount = r3NewArray(ECX_COMPONENT_MAX, sizeof(u8));
    ECX.component.fieldHash = r3NewArray(ECX_COMPONENT_MAX, sizeof(u32*));
    ECX.component.fieldStride = r3NewArray(ECX_COMPONENT_MAX, sizeof(u16*));
    ECX.component.fieldOffset = r3NewArray(ECX_COMPONENT_MAX, sizeof(u16*));
    
    if (!ECX.component.free        ||
        !ECX.component.gen         ||
        !ECX.component.mask        ||
        !ECX.component.hash        ||
        !ECX.component.field       ||
        !ECX.component.fieldMax    ||
        !ECX.component.fieldHash   ||
        !ECX.component.fieldSize   ||
        !ECX.component.fieldCount  ||
        !ECX.component.fieldStride ||
        !ECX.component.fieldOffset) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during init -- internal component array alloc failed\n");
        return ecxExit();
    }
    
    // init query internal
    ECX.query.next = 0;
    ECX.query.count = 0;
    ECX.query.all = r3NewArray(64, sizeof(u64));
    ECX.query.any = r3NewArray(64, sizeof(u64));
    ECX.query.free = r3NewArray(64, sizeof(u8));
    ECX.query.none = r3NewArray(64, sizeof(u64));
    ECX.query.seen = r3NewArray(64, sizeof(u32));
    ECX.query.cached = r3NewArray(64, sizeof(u8));
    ECX.query.config = r3NewArray(64, sizeof(ECXQuery));
    ECX.query.localToGlobal = r3NewArray(64, sizeof(u32*));
    ECX.query.globalToLocal = r3NewArray(64, sizeof(u32*));

    if (!ECX.query.free            ||
        !ECX.query.all             ||
        !ECX.query.any             ||
        !ECX.query.none            ||
        !ECX.query.seen            ||
        !ECX.query.config          ||
        !ECX.query.cached          ||
        !ECX.query.localToGlobal   ||
        !ECX.query.globalToLocal) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during init -- internal query array alloc failed\n");
        return ecxExit();
    }

    
    // init config internal
    ECX.config.next = 0;
    ECX.config.count = 0;
    ECX.config.free = r3NewArray(64, sizeof(u16));
    ECX.config.signature = r3NewArray(64, sizeof(u64));
    ECX.config.fieldSet = r3NewArray(64, sizeof(ptr**));
    ECX.config.entitySet = r3NewArray(64, sizeof(u32*));
    ECX.config.entityCount = r3NewArray(64, sizeof(u32));
    ECX.config.componentSet = r3NewArray(64, sizeof(u8*));
    ECX.config.componentCount = r3NewArray(64, sizeof(u8));
    ECX.config.localToGlobal = r3NewArray(64, sizeof(u32*));
    ECX.config.globalToLocal = r3NewArray(64, sizeof(u32*));
    if (!ECX.config.free           ||
        !ECX.config.fieldSet       ||
        !ECX.config.signature      ||
        !ECX.config.entitySet      ||
        !ECX.config.entityCount    ||
        !ECX.config.componentSet   ||
        !ECX.config.componentCount ||
        !ECX.config.localToGlobal  ||
        !ECX.config.globalToLocal) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during init -- internal query array alloc failed\n");
        return ecxExit();
    }
    
    r3LogStdOut(R3_LOG_OK, "[ECX] Initialized\n");
    return 1;
}

u8 ecxExit(none) {
    if (!ECX.init) {
        r3LogStdOut(R3_LOG_WARN, "[ECX] Skipping redundant exit\n");
        return 1;
    }
    
    ECX.init = 0;
    // dealloc internal arena
    if (!r3DelArenaAllocator(&ECX.arena)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during exit -- internal arena dealloc failed\n");
    }

    // exit entity internal
    if (!r3DelArray(ECX.entity.gen)  ||
        !r3DelArray(ECX.entity.free) ||
        !r3DelArray(ECX.entity.mask)) {
            r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during exit -- internal entity array dealloc failed\n");
    }
    ECX.entity.next = 0;
    ECX.entity.count = 0;

    // exit component internal
    if (!r3DelArray(ECX.component.free)          ||
        !r3DelArray(ECX.component.gen)           ||
        !r3DelArray(ECX.component.mask)          ||
        !r3DelArray(ECX.component.hash)          ||
        !r3DelArray(ECX.component.field)         ||
        !r3DelArray(ECX.component.fieldMax)      ||
        !r3DelArray(ECX.component.fieldSize)     ||
        !r3DelArray(ECX.component.fieldHash)     ||
        !r3DelArray(ECX.component.fieldCount)    ||
        !r3DelArray(ECX.component.fieldStride)   ||
        !r3DelArray(ECX.component.fieldOffset)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during exit -- internal component array dealloc failed\n");
    }
    ECX.component.next = 0;
    ECX.component.count = 0;

    // exit query internal
    if (!r3DelArray(ECX.query.free)           ||
        !r3DelArray(ECX.query.all)            ||
        !r3DelArray(ECX.query.any)            ||
        !r3DelArray(ECX.query.none)           ||
        !r3DelArray(ECX.query.seen)           ||
        !r3DelArray(ECX.query.config)         ||
        !r3DelArray(ECX.query.cached)         ||
        !r3DelArray(ECX.query.localToGlobal)  ||
        !r3DelArray(ECX.query.globalToLocal)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during exit -- internal query array dealloc failed\n");
        return ecxExit();
    }
    ECX.query.next = 0;
    ECX.query.count = 0;

    
    // exit config internal
    if (!r3DelArray(ECX.config.free)           ||
        !r3DelArray(ECX.config.signature)      ||
        !r3DelArray(ECX.config.fieldSet)       ||
        !r3DelArray(ECX.config.entitySet)      ||
        !r3DelArray(ECX.config.entityCount)    ||
        !r3DelArray(ECX.config.componentSet)   ||
        !r3DelArray(ECX.config.componentCount) ||
        !r3DelArray(ECX.config.localToGlobal)  ||
        !r3DelArray(ECX.config.globalToLocal)) {
        r3LogStdOut(R3_LOG_ERROR, "[ECX] Error during exit -- internal query array dealloc failed\n");
        return ecxExit();
    }
    ECX.config.next = 0;
    ECX.config.count = 0;

    r3LogStdOut(R3_LOG_OK, "[ECX] Exited\n");
    return 1;
}
