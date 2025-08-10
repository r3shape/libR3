#include <r3kit/include/mem/alloc.h>
#include <r3kit/include/io/log.h>
#include <r3kit/include/ds/tree.h>

static inline ptr get_header(u8 fields, u16 stride, ptr data) {
    return (ptr)((u8*)data - (stride * fields));
}

static inline u16 get_field(u8 fields, u8 field, u16 stride, ptr data) {
    return ((u16*)get_header(fields, stride, data))[field];
}

static inline none set_header(u8 fields, u8 field, u16 stride, u16 value, ptr data) {
    *(u16*)((u8*)data - (stride * (fields - field))) = value;
}

u16 r3_tree_field(Tree_Header_Field field, Tree* in) {
	return get_field(TREE_HEADER_FIELDS, field, sizeof(u16), in->data);
}

u8 r3_tree_alloc(u16 id, u16 max, u16 stride, Tree* out) {
    if (id > I16_MAX) {
        r3_log_stdoutf(ERROR_LOG, "invalid tree id: (id)%d\n", id);
        return 0;
    } if (max > I16_MAX) {
        r3_log_stdoutf(ERROR_LOG, "invalid tree max: (max)%d\n", max);
        return 0;
    } if (out == NULL || out->data != NULL) {
        r3_log_stdout(ERROR_LOG, "invalid tree pointer\n");
        return 0;
    }

    ptr raw = r3_mem_alloc((sizeof(u16) * TREE_HEADER_FIELDS) + stride, 8);
	if (!raw) {
        r3_log_stdout(ERROR_LOG, "failed to allocate tree\n");
		return 0;
	}
    
    out->nodes = NULL;
    out->data = (ptr)((u8*)raw + (sizeof(u16) * TREE_HEADER_FIELDS));

    set_header(TREE_HEADER_FIELDS, ID_FIELD, sizeof(u16), id, out->data);
    set_header(TREE_HEADER_FIELDS, MAX_FIELD, sizeof(u16), max, out->data);
    set_header(TREE_HEADER_FIELDS, SIZE_FIELD, sizeof(u16), stride, out->data);
    set_header(TREE_HEADER_FIELDS, NODES_FIELD, sizeof(u16), 0, out->data);
    set_header(TREE_HEADER_FIELDS, STRIDE_FIELD, sizeof(u16), stride, out->data);
    
    return 1;
}

u8 r3_tree_dealloc(Tree* in) {
    if (!in || !in->data) {
        r3_log_stdoutf(ERROR_LOG, "invalid tree pointer\n");
        return 0;
    }

    if (in->nodes) {
        r3_mem_dealloc(in->nodes);
    }

    r3_mem_dealloc(get_header(TREE_HEADER_FIELDS, sizeof(u16), in->data));

    return 1;
}
