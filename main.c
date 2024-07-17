#include "FlexAlloc.h"
#include <windows.h>

void secure_print(const char* message) {
    OutputDebugStringA(message);
}

void test_allocate() {
    size_t size = 1024;
    size_t alignment = 16;
    memory_block block = allocate(size, alignment);

    if (block.ptr != NULL && block.size >= size) {
        secure_print("test_allocate: PASSED\n");
    }
    else {
        secure_print("test_allocate: FAILED\n");
    }

    deallocate(block, alignment);
}

void test_deallocate() {
    size_t size = 1024;
    size_t alignment = 16;
    memory_block block = allocate(size, alignment);

    if (block.ptr == NULL) {
        secure_print("test_deallocate: FAILED (allocation failed)\n");
        return;
    }

    deallocate(block, alignment);

    // Check if memory is successfully deallocated by trying to reallocate it
    memory_block block2 = allocate(size, alignment);
    if (block2.ptr != NULL) {
        secure_print("test_deallocate: PASSED\n");
    }
    else {
        secure_print("test_deallocate: FAILED\n");
    }

    deallocate(block2, alignment);
}

void test_try_expand() {
    size_t size = 1024;
    size_t new_size = 2048;
    size_t alignment = 16;
    memory_block block = allocate(size, alignment);

    if (block.ptr == NULL) {
        secure_print("test_try_expand: FAILED (allocation failed)\n");
        return;
    }

    if (try_expand(&block, new_size)) {
        secure_print("test_try_expand: PASSED\n");
    }
    else {
        secure_print("test_try_expand: FAILED\n");
    }

    deallocate(block, alignment);
}

int main() {
    secure_print("Running unit tests...\n");

    test_allocate();
    test_deallocate();
    test_try_expand();

    secure_print("Unit tests completed.\n");

    return 0;
}
