#include <include/libR3/mem/mem.h>
#include <include/libR3/io/file.h>
#include <include/libR3/io/log.h>
#include <stdio.h>

#define R3_FILE_HEADER_SIZE sizeof(R3FileHeader)

R3Result r3FlagFile(R3FileFlag flags, ptr file) {
    if (!flags || flags > R3_FILE_FLAGS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `FlagFile` -- invalid `FileFlag`(s)\n");
        return R3_RESULT_ERROR;
    } if (!file) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `FlagFile` -- invalid `char` pointer\n");
        return R3_RESULT_ERROR;
    }

    R3FileHeader* h = ((R3FileHeader*)((u8*)file - R3_FILE_HEADER_SIZE));
    h->mask &= flags;
    
    return R3_RESULT_SUCCESS;
}

R3Result r3FileHeader(R3FileHeader* header, ptr file) {
    if (!header || !file) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewFile` -- invalid `File` pointer\n");
        return R3_RESULT_ERROR;
    }

    *header = *((R3FileHeader*)((u8*)file - R3_FILE_HEADER_SIZE));

    return R3_RESULT_SUCCESS;
}


R3Result r3DelFile(ptr file) {
    if (!file) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelFile` -- invalid `char` pointer\n");
        return R3_RESULT_ERROR;
    }

    ptr raw = (ptr)((u8*)file - R3_FILE_HEADER_SIZE);

    r3FreeMemory(raw);
    file = NULL;

    return R3_RESULT_SUCCESS;
}

ptr r3NewFile(u64 bytes) {
    if (!bytes) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewFile` -- invalid file length\n");
        return NULL;
    }

    ptr raw = r3AllocMemory(R3_FILE_HEADER_SIZE + ((bytes + 1) * sizeof(char)));
    if (!raw) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `NewFile` -- out of memory\n");
        return NULL;
    }
    
    *((R3FileHeader*)raw) = (R3FileHeader) {
        .size = bytes,
        .cursor = 0,
        .rOps = 0,
        .wOps = 0,
        .mask = R3_FILE_MUTABLE|R3_FILE_WRITE
    };

    ptr file = (ptr)((u8*)raw + R3_FILE_HEADER_SIZE);
    *((u8*)file + bytes) = '\0';
    
    return file;
}


R3Result r3RewindFile(u64 bytes, ptr file) {
    if (!file) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelFile` -- invalid `char` pointer\n");
        return R3_RESULT_ERROR;
    }

    R3FileHeader* h = ((R3FileHeader*)((u8*)file - R3_FILE_HEADER_SIZE));
    if ((h->mask & R3_FILE_MUTABLE) != R3_FILE_MUTABLE) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ClearFile` -- file flagged as immutable\n");
        return R3_RESULT_ERROR;
    }

    if (h->cursor - bytes <= 0 || !bytes) h->cursor = 0;
    else h->cursor -= bytes;

    return R3_RESULT_SUCCESS;
}

R3Result r3SeekFile(u64 bytes, ptr file) {
    if (!file) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `DelFile` -- invalid `char` pointer\n");
        return R3_RESULT_ERROR;
    }
    
    R3FileHeader* h = ((R3FileHeader*)((u8*)file - R3_FILE_HEADER_SIZE));
    if ((h->mask & R3_FILE_MUTABLE) != R3_FILE_MUTABLE) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ClearFile` -- file flagged as immutable\n");
        return R3_RESULT_ERROR;
    }

    if (h->cursor + bytes > h->size) h->cursor = h->size;
    else h->cursor += bytes;

    return R3_RESULT_SUCCESS;
}


R3Result r3LoadFile(char* path, ptr file) {
    if (!path) {
		r3LogStdOut(R3_LOG_ERROR, "Invalid file save path\n");
		return R3_RESULT_ERROR;
	}

	if (!file) {
		r3LogStdOut(R3_LOG_ERROR, "Invalid file pointer\n");
		return R3_RESULT_ERROR;
	}

	FILE* iobuf = NULL;
	if (!(iobuf = (fopen((const char*)path, "rb")))) {
		r3LogStdOutF(R3_LOG_ERROR, "Failed to load file content: (path)%s\n", path);
		return R3_RESULT_ERROR;
	}

    if (fseek(iobuf, 0, SEEK_END)) {
		r3LogStdOutF(R3_LOG_ERROR, "Failed to access file content: (path)%s\n", path);
		fclose(iobuf);
		return 0;
	}

	R3FileHeader* h = ((R3FileHeader*)((u8*)file - R3_FILE_HEADER_SIZE));
	u64 fsize = (u16)ftell(iobuf);
    if (fsize > h->size) {
		r3LogStdOutF(R3_LOG_WARN, "Failed `LoadFile` -- file overflow: (file size)%d (load size)%d\n", h->size, fsize);
		return 0;
	}
	
	if (fseek(iobuf, 0, SEEK_SET)) {
		r3LogStdOutF(R3_LOG_ERROR, "Failed `LoadFile` -- file path inaccessible: (path)%s\n", path);
		fclose(iobuf);
		return 0;
	}

	u64 read = fread((u8*)file + h->cursor, 1, fsize, iobuf);
	if (read != fsize) r3LogStdOutF(R3_LOG_WARN, "Failed `LoadFile` -- could not read full file: (read)%llu (expected)%llu\n", read, fsize);
	
	fclose(iobuf);
    h->wOps++;

    return R3_RESULT_SUCCESS;
}

R3Result r3SaveFile(u64 bytes, char* path, ptr file) {
    if (!path) {
		r3LogStdOut(R3_LOG_ERROR, "Invalid file save path\n");
		return R3_RESULT_ERROR;
	} if (!file) {
		r3LogStdOut(R3_LOG_ERROR, "Invalid file pointer\n");
		return R3_RESULT_ERROR;
	}

	FILE* iobuf = NULL;
	if (!(iobuf = (fopen((const char*)path, "wb")))) {
		r3LogStdOutF(R3_LOG_ERROR, "Failed `SaveFile` -- file path inaccessible: (path)%s\n", path);
		return R3_RESULT_ERROR;
	}

    R3FileHeader* h = ((R3FileHeader*)((u8*)file - R3_FILE_HEADER_SIZE));
    if (bytes > h->size) {
        r3LogStdOutF(R3_LOG_ERROR, "Failed `SaveFile` -- invalid byte count: (bytes)%llu (size)%llu\n", bytes, h->size);
		return R3_RESULT_ERROR;
    }

	u64 saved = fwrite(file, 1, bytes, iobuf);
	if (saved != bytes) r3LogStdOutF(R3_LOG_WARN, "Failed `SaveFile` -- could not write full file: (wrote)%llu (expected)%llu\n", saved, bytes);

	fclose(iobuf);
    h->rOps++;

	return R3_RESULT_SUCCESS;
}


R3Result r3ClearFile(ptr file) {
    if (!file) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ClearFile` -- invalid `char` pointer\n");
        return R3_RESULT_ERROR;
    }
    
    R3FileHeader* h = ((R3FileHeader*)((u8*)file - R3_FILE_HEADER_SIZE));
    if ((h->mask & R3_FILE_MUTABLE) != R3_FILE_MUTABLE) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ClearFile` -- file flagged as immutable\n");
        return R3_RESULT_ERROR;
    }

    if (r3SetMemory(h->size, 0, file) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ClearFile` -- file memory set failed\n");
        return R3_RESULT_ERROR;
    }
    
    h->cursor = 0;
    h->wOps++;

    return R3_RESULT_SUCCESS;
}

R3Result r3ReadFile(u64 bytes, ptr source, ptr dest) {
    if (!bytes) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadFile` -- invalid file length\n");
        return R3_RESULT_ERROR;
    } if (!dest || !source) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadFile` -- invalid source/dest pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3FileHeader* h = ((R3FileHeader*)((u8*)source - R3_FILE_HEADER_SIZE));
    if (bytes > h->size) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadFile` -- out of bounds read caught\n");
        return R3_RESULT_ERROR;
    }

    ptr readat = (ptr)((u8*)source + h->cursor);
    if (r3ReadMemory(bytes, readat, dest) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `ReadFile` -- source file memory read failed\n");
        return R3_RESULT_ERROR;
    }

    // TODO: null-terminate dest file?
    h->rOps++;

    return R3_RESULT_SUCCESS;
}

R3Result r3WriteFile(u64 bytes, ptr source, ptr dest) {
    if (!bytes) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteFile` -- invalid file length\n");
        return R3_RESULT_ERROR;
    } if (!source || !dest) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteFile` -- invalid source/dest pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3FileHeader* h = ((R3FileHeader*)((u8*)dest - R3_FILE_HEADER_SIZE));
    if ((h->mask & R3_FILE_MUTABLE) != R3_FILE_MUTABLE) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteFile` -- file flagged as immutable\n");
        return R3_RESULT_ERROR;
    }

    if (bytes > h->size) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteFile` -- out of bounds write caught\n");
        return R3_RESULT_ERROR;
    }

    ptr writeat = (ptr)((u8*)dest + h->cursor);
    if (r3WriteMemory(bytes, source, writeat) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `WriteFile` -- dest file memory write failed\n");
        return R3_RESULT_ERROR;
    }

    if (!(h->mask & R3_FILE_WRITE)) h->cursor += bytes;  // file flagged as `append`
    h->wOps++;

    return R3_RESULT_SUCCESS;
}

R3Result r3CopyFile(u64 bytes, ptr source, ptr dest) {
    if (!source || !dest) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `CopyFile` -- invalid source/dest pointer(s)\n");
        return R3_RESULT_ERROR;
    }

    R3FileHeader* h = ((R3FileHeader*)((u8*)source - R3_FILE_HEADER_SIZE));
    if (bytes > h->size) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `CopyFile` -- out of bounds copy caught\n");
        return R3_RESULT_ERROR;
    }

    if (r3ReadMemory(bytes, source, dest) != R3_RESULT_SUCCESS) {
        r3LogStdOut(R3_LOG_ERROR, "Failed `CopyFile` -- dest file memory write failed\n");
        return R3_RESULT_ERROR;
    }

    h->rOps++;

    return R3_RESULT_SUCCESS;
}
