#include <include/libR3/io/string.h>
#include <include/libR3/mem/mem.h>
#include <include/libR3/io/log.h>

#define R3_STRING_HEADER_SIZE sizeof(R3StringHeader)

R3Result r3FlagString(R3StringFlag flags, char* string) {
    if (!flags || flags > R3_STRING_FLAGS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `FlagString` - invalid `StringFlag`(s)\n");
        return R3_RESULT_ERROR;
    } if (!string) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `FlagString` - invalid `char` pointer\n");
        return R3_RESULT_ERROR;
    }

    R3StringHeader* h = ((R3StringHeader*)((u8*)string - R3_STRING_HEADER_SIZE));
    h->mask &= flags;
    
    return R3_RESULT_SUCCESS;
}

R3Result r3StringHeader(R3StringHeader* header, char* string) {
    if (!header || !string) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewString` - invalid `String` pointer\n");
        return R3_RESULT_ERROR;
    }

    *header = *((R3StringHeader*)((u8*)string - R3_STRING_HEADER_SIZE));

    return R3_RESULT_SUCCESS;
}

R3Result r3DelString(char* string) {
    if (!string) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelString` - invalid `char` pointer\n");
        return R3_RESULT_ERROR;
    }

    ptr raw = (ptr)((u8*)string - R3_STRING_HEADER_SIZE);

    r3FreeMemory(raw);
    string = NULL;

    return R3_RESULT_SUCCESS;
}

char* r3NewString(u64 length) {
    if (!length) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewString` - invalid string length\n");
        return NULL;
    }

    ptr raw = r3AllocMemory(R3_STRING_HEADER_SIZE + ((length + 1) * sizeof(char)));
    if (!raw) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewString` - out of memory\n");
        return NULL;
    }
    
    *((R3StringHeader*)raw) = (R3StringHeader) {
        .length = length,
        .cursor = 0,
        .rOps = 0,
        .wOps = 0,
        .mask = R3_STRING_MUTABLE|R3_STRING_WRITE
    };

    char* string = (char*)((u8*)raw + R3_STRING_HEADER_SIZE);
    *((u8*)string + length) = '\0';
    
    return string;
}


R3Result r3RewindString(u64 bytes, char* string) {
    if (!string) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelString` - invalid `char` pointer\n");
        return R3_RESULT_ERROR;
    }

    R3StringHeader* h = ((R3StringHeader*)((u8*)string - R3_STRING_HEADER_SIZE));
    if ((h->mask & R3_STRING_MUTABLE) != R3_STRING_MUTABLE) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ClearString` - string flagged as immutable\n");
        return R3_RESULT_ERROR;
    }

    if (h->cursor - bytes <= 0 || !bytes) h->cursor = 0;
    else h->cursor -= bytes;

    return R3_RESULT_SUCCESS;
}

R3Result r3SeekString(u64 bytes, char* string) {
    if (!string) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelString` - invalid `char` pointer\n");
        return R3_RESULT_ERROR;
    }
    
    R3StringHeader* h = ((R3StringHeader*)((u8*)string - R3_STRING_HEADER_SIZE));
    if ((h->mask & R3_STRING_MUTABLE) != R3_STRING_MUTABLE) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ClearString` - string flagged as immutable\n");
        return R3_RESULT_ERROR;
    }

    if (h->cursor + bytes > h->length) h->cursor = h->length;
    else h->cursor += bytes;

    return R3_RESULT_SUCCESS;
}

R3Result r3ClearString(char* string) {
    if (!string) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ClearString` - invalid `char` pointer\n");
        return R3_RESULT_ERROR;
    }
    
    R3StringHeader* h = ((R3StringHeader*)((u8*)string - R3_STRING_HEADER_SIZE));
    if ((h->mask & R3_STRING_MUTABLE) != R3_STRING_MUTABLE) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ClearString` - string flagged as immutable\n");
        return R3_RESULT_ERROR;
    }

    if (r3SetMemory(h->length, 0, string) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ClearString` - string memory set failed\n");
        return R3_RESULT_ERROR;
    }
    
    h->cursor = 0;
    h->wOps++;

    return R3_RESULT_SUCCESS;
}

R3Result r3ReadString(u64 length, char* source, char* dest) {
    if (!length) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadString` - invalid string length\n");
        return R3_RESULT_ERROR;
    } if (!dest || !source) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadString` - invalid source/dest pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3StringHeader* h = ((R3StringHeader*)((u8*)source - R3_STRING_HEADER_SIZE));
    if (length > h->length) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadString` - out of bounds read caught\n");
        return R3_RESULT_ERROR;
    }

    ptr readat = (ptr)((u8*)source + h->cursor);
    if (r3ReadMemory(length, readat, dest) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadString` - source string memory read failed\n");
        return R3_RESULT_ERROR;
    }

    // TODO: null-terminate dest string?
    h->rOps++;

    return R3_RESULT_SUCCESS;
}

R3Result r3WriteString(u64 length, char* source, char* dest) {
    if (!length) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteString` - invalid string length\n");
        return R3_RESULT_ERROR;
    } if (!source || !dest) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteString` - invalid source/dest pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3StringHeader* h = ((R3StringHeader*)((u8*)dest - R3_STRING_HEADER_SIZE));
    if ((h->mask & R3_STRING_MUTABLE) != R3_STRING_MUTABLE) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteString` - string flagged as immutable\n");
        return R3_RESULT_ERROR;
    }

    if (length > h->length) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteString` - out of bounds write caught\n");
        return R3_RESULT_ERROR;
    }

    ptr writeat = (ptr)((u8*)dest + h->cursor);
    if (r3WriteMemory(length, source, writeat) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteString` - dest string memory write failed\n");
        return R3_RESULT_ERROR;
    }

    if (!(h->mask & R3_STRING_WRITE)) h->cursor += length;  // string flagged as `append`
    h->wOps++;

    return R3_RESULT_SUCCESS;
}

R3Result r3CopyString(u64 bytes, char* source, char* dest) {
    if (!source || !dest) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `CopyString` - invalid source/dest pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3StringHeader* h = ((R3StringHeader*)((u8*)source - R3_STRING_HEADER_SIZE));
    if (bytes > h->length) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `CopyString` - out of bounds copy caught\n");
        return R3_RESULT_ERROR;
    }

    if (r3ReadMemory(bytes, source, dest) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `CopyString` - dest string memory write failed\n");
        return R3_RESULT_ERROR;
    }

    h->rOps++;

    return R3_RESULT_SUCCESS;
}
