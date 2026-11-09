#pragma once

#include <stddef.h>
#include <stdint.h>

struct Global {
    const wchar_t *shared_storage_mutex_name;
    uintptr_t shared_storage_mutex_handle;
    const wchar_t *shared_storage_name;
    size_t shared_storage_capacity;
    size_t shared_storage_size;
    uintptr_t shared_storage_handle;
};
