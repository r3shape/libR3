#include <include/libR3/ds/array.h>
#include <include/libR3/mem/mem.h>
#include <include/libR3/io/log.h>

typedef struct R3KVP { char* k; ptr v; } R3KVP;

inline u32 _fnv1a(const char* k) {
	if (!k) return I32_MAX;
	u32 h = 2166136261u; 	// FNV-1a offset basis
	while (*k) {
		h ^= (char)*k++;	// XOR with current character
		h *= 16777619;		// multiply by FNV-1a prime
	}; return h;
}

static inline u64 _probeWriteSlot(const char* key, ptr array) {
	R3ArrayHeader h;
    r3ArrayHeader(&h, array);

    u64 slots = h.slots;
	u64 slot = _fnv1a(key) % slots;
	u64 start = slot;
	FOR_I(0, slots, 1) {
		R3KVP* kvp = &((R3KVP*)array)[slot];
		if (kvp->k != NULL && strcmp(kvp->k, key) == 0) {
			r3LogStdOutF(R3_LOG_WARN, "`_probeWriteSlot` -- probed overwrite slot: (key)%s (slot)%d\n", kvp->k, slot);
			return (u64)slot;
		} else if (kvp->v == NULL) {
			r3LogStdOutF(R3_LOG_OK, "`_probeWriteSlot` -- probed write slot: (key)%s (slot)%d\n", key, slot);
			return (u64)slot;
		} else {
			slot = (slot + 1) % slots;
			if (slot == start) {
				r3LogStdOutF(R3_LOG_ERROR, "Failed `_probeWriteSlot` -- failed to probe write slot: (key)%s\n", key);
				return I64_MAX;
			}
		}
	}
	return slot;
}

static inline u64 _probeReadSlot(const char* key, ptr array) {
	R3ArrayHeader h;
    r3ArrayHeader(&h, array);

    u64 slots = h.slots;
	u64 slot = _fnv1a(key) % slots;
	u64 start = slot;
	FOR_I(0, slots, 1) {
		R3KVP* kvp = &((R3KVP*)array)[slot];
		if (kvp && kvp->k != NULL && strcmp(kvp->k, key) == 0) {
			r3LogStdOutF(R3_LOG_OK, "`_probeReadSlot` -- probed read slot: (key)%s (slot)%d\n", kvp->k, slot);
			return (u64)slot;
		} else {
			slot = (slot + 1) % slots;
			if (slot == start) {
				r3LogStdOutF(R3_LOG_ERROR, "Failed `_probeReadSlot` -- failed to probe read slot: (key)%s\n", key);
				return I64_MAX;
			}
		}
	}
	return (u64)slot;
}


R3Result r3DelHashArray(ptr array) { return r3DelArray(array); }

ptr r3NewHashArray(u64 slots, u16 stride) {
    ptr raw = r3NewArray(slots, sizeof(R3KVP));
    ((R3ArrayHeader*)((u8*)raw - sizeof(R3ArrayHeader)))->mask = stride;
    return raw;
}

// TODO: gather non-null kvp's and recompute after array resize
ptr r3ResizeHashArray(u64 slots, ptr array) { return NULL; }


R3Result r3GetHashArray(char* key, ptr array, ptr value) {
    u8 key_err; u8 value_err; u8 ptr_err;
	if ((key_err = (!key)) || (value_err = (!value)) || (ptr_err = (!array))) {
		if (key_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `GetHashArray` -- key error\n");
			return R3_RESULT_ERROR;
		} if (value_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `GetHashArray` -- value error\n");
			return R3_RESULT_ERROR;
		} if (ptr_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `GetHashArray` -- invalid array ptr\n");
			return R3_RESULT_ERROR;
		}
	}

    R3ArrayHeader h;
    r3ArrayHeader(&h, array);
    u16 stride = h.mask;
    
    u32 slot = _probeReadSlot(key, array);
	if (slot > h.slots) {
		r3LogStdOutF(R3_LOG_ERROR, "Failed `GetHashArray` -- failed to probe read hashed slot: (key)%s (slot)%d (slots)%llu\n", key, slot, h.slots);
		return R3_RESULT_ERROR;
	}

    R3KVP kvp;
    r3GetArray(slot, array, &kvp);
    if (!kvp.k) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `GetHashArray` -- removing unexpected null key: (key)%s (slot)%d\n", key, slot);
        ((R3KVP*)array)[slot].k = NULL;
        ((R3KVP*)array)[slot].v = NULL;
        return R3_RESULT_ERROR;
    } if (r3ReadMemory(stride, kvp.v, value) != R3_RESULT_SUCCESS) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `GetHashArray` -- failed to read memory for key: (key)%s (slot)%d\n", key, slot);
		return 0;
    }

    return R3_RESULT_SUCCESS;
}

R3Result r3SetHashArray(char* key, ptr value, ptr array) {
    u8 key_err; u8 value_err; u8 ptr_err;
	if ((key_err = (!key)) || (value_err = (!value)) || (ptr_err = (!array))) {
		if (key_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `SetHashArray` -- key error\n");
			return R3_RESULT_ERROR;
		} if (value_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `SetHashArray` -- value error\n");
			return R3_RESULT_ERROR;
		} if (ptr_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `SetHashArray` -- invalid array ptr\n");
			return R3_RESULT_ERROR;
		}
	}

    R3ArrayHeader h;
    r3ArrayHeader(&h, array);
    
    u16 stride = h.mask;
    u32 slot = _probeWriteSlot(key, array);
	if (slot > h.slots) {
		r3LogStdOutF(R3_LOG_ERROR, "Failed `SetHashArray` -- failed to probe write hashed slot: (key)%s (slot)%d (slots)%llu\n", key, slot, h.slots);
		return R3_RESULT_ERROR;
	} else { r3LogStdOutF(R3_LOG_OK, "`SetHashArray` -- probed write hashed slot: (key)%s (slot)%d (stride)%d\n", key, slot, stride); }

    R3KVP kvp;
    r3GetArray(slot, array, &kvp);
    if (!kvp.v) {
        kvp.v = r3AllocMemory(stride);
        if (!(kvp.v)) {
            r3LogStdOutF(R3_LOG_ERROR, "[arr] failed to allocate memory for key: (key)%s (slot)%d\n", key, slot);
            return R3_RESULT_ERROR;
        }
    }
    
    kvp.k = key;
    if (r3WriteMemory(stride, value, kvp.v) != R3_RESULT_SUCCESS || r3SetArray(slot, &kvp, array) != R3_RESULT_SUCCESS) {
        r3FreeMemory(kvp.v);
		r3LogStdOutF(R3_LOG_ERROR, "[arr] failed to write value for key: (key)%s (slot)%d\n", key, slot);
        return R3_RESULT_ERROR;
    }

    return R3_RESULT_SUCCESS;
}

R3Result r3RemHashArray(char* key, ptr array, ptr value) {
    u8 key_err; u8 value_err; u8 ptr_err;
	if ((key_err = (!key)) || (value_err = (!value)) || (ptr_err = (!array))) {
		if (key_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `RemHashArray` -- key error\n");
			return R3_RESULT_ERROR;
		} if (value_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `RemHashArray` -- value error\n");
			return R3_RESULT_ERROR;
		} if (ptr_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `RemHashArray` -- invalid array ptr\n");
			return R3_RESULT_ERROR;
		}
	}

    R3ArrayHeader h;
    r3ArrayHeader(&h, array);
    u16 stride = h.mask;
    
    u32 slot = _probeReadSlot(key, array);
	if (slot > h.slots) {
		r3LogStdOutF(R3_LOG_ERROR, "Failed `RemHashArray` -- failed to probe read hashed slot: (key)%s (slot)%d (slots)%llu\n", key, slot, h.slots);
		return R3_RESULT_ERROR;
	}

    R3KVP kvp;
    r3GetArray(slot, array, &kvp);
    if (!kvp.k) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `RemHashArray` -- removing unexpected null key: (key)%s (slot)%d\n", key, slot);
        ((R3KVP*)array)[slot].k = NULL;
        ((R3KVP*)array)[slot].v = NULL;
        return R3_RESULT_ERROR;
    } if (r3ReadMemory(stride, kvp.v, value) != R3_RESULT_SUCCESS) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `RemHashArray` -- failed to read memory for key: (key)%s (slot)%d\n", key, slot);
		return 0;
    }

    r3FreeMemory(kvp.v);
    r3RemArray(slot, array, &kvp);

    return R3_RESULT_SUCCESS;
}

R3Result r3AssignHashArray(char* key, ptr value, ptr array) {
    u8 key_err; u8 value_err; u8 ptr_err;
	if ((key_err = (!key)) || (value_err = (!value)) || (ptr_err = (!array))) {
		if (key_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `AssignHashArray` -- key error\n");
			return R3_RESULT_ERROR;
		} if (value_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `AssignHashArray` -- value error\n");
			return R3_RESULT_ERROR;
		} if (ptr_err) {
			r3LogStdOut(R3_LOG_ERROR, "Failed `AssignHashArray` -- invalid array ptr\n");
			return R3_RESULT_ERROR;
		}
	}

    R3ArrayHeader h;
    r3ArrayHeader(&h, array);
    u32 slot = _probeWriteSlot(key, array);
	if (slot > h.slots) {
		r3LogStdOutF(R3_LOG_ERROR, "Failed `AssignHashArray` -- failed to probe write hashed slot: (key)%s (slot)%d (slots)%llu\n", key, slot, h.slots);
		return R3_RESULT_ERROR;
	}

    R3KVP kvp;
    r3GetArray(slot, array, &kvp);
    if (!kvp.v) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `RemHashArray` -- removing unexpected null key: (key)%s (slot)%d\n", key, slot);
        ((R3KVP*)array)[slot].k = NULL;
        ((R3KVP*)array)[slot].v = NULL;
        return R3_RESULT_ERROR;
    } else { *(u64*)(((R3KVP*)array)[slot].v) = (u64)value; }

    return R3_RESULT_SUCCESS;
}
