#ifndef FLEX_ALLOC_H
#define FLEX_ALLOC_H

#include <windows.h>
#include <stddef.h>

typedef struct {
    void* ptr;
    size_t size;
} memory_block;

/// Retrieve the system's page size.
size_t getSystemPageSize() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwPageSize;
}

/// Allocate memory with given size and alignment.
/// On success, returns a memory_block with a valid pointer and actual size.
/// On failure, returns a memory_block with ptr == NULL and size == 0.
memory_block allocate(size_t size, size_t alignment) {
    memory_block result = {NULL, 0};

    // Ensure alignment is a power of two
    if ((alignment & (alignment - 1)) != 0) {
        return result;
    }

    size_t pageSize = getSystemPageSize();
    if (alignment <= pageSize) {
        result.ptr = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (result.ptr != NULL) {
            result.size = size;
        }
    } else {
        // For larger alignments, allocate extra memory and align manually
        size_t totalSize = size + alignment;
        void* rawPtr = VirtualAlloc(NULL, totalSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (rawPtr != NULL) {
            uintptr_t alignedPtr = ((uintptr_t)rawPtr + alignment - 1) & ~(alignment - 1);
            result.ptr = (void*)alignedPtr;
            result.size = totalSize - (alignedPtr - (uintptr_t)rawPtr);
        }
    }

    return result;
}

/// Deallocate the memory block with the given alignment.
void deallocate(memory_block block, size_t alignment) {
    // Ensure alignment is a power of two
    if ((alignment & (alignment - 1)) != 0 || block.ptr == NULL) {
        return;
    }

    size_t pageSize = getSystemPageSize();
    if (alignment <= pageSize) {
        VirtualFree(block.ptr, 0, MEM_RELEASE);
    } else {
        // Handle larger alignments, if allocated with custom methods
        VirtualFree((void*)((uintptr_t)block.ptr & ~(pageSize - 1)), 0, MEM_RELEASE);
    }
}

/// Attempt to expand the memory block in-place to the new size.
/// Returns 1 if successful, otherwise 0.
int try_expand(memory_block* block, size_t new_size) {
    if (block->ptr != NULL) {
        HANDLE hHeap = GetProcessHeap();
        void* new_ptr = HeapReAlloc(hHeap, HEAP_REALLOC_IN_PLACE_ONLY, block->ptr, new_size);
        if (new_ptr != NULL) {
            block->ptr = new_ptr;
            block->size = new_size;
            return 1;
        }
    }
    return 0;
}

#endif // FLEX_ALLOC_H
