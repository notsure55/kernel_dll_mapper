#pragma once
#include <ntifs.h>

// DIC CONTROL CODE
#define IOCTL_OPEN_PROCESS CTL_CODE(0x8000, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_READ_PROCESS CTL_CODE(0x8000, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_PROCESS CTL_CODE(0x8000, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ALLOC_PROCESS CTL_CODE(0x8000, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CREATE_THREAD_PROCESS CTL_CODE(0x8000, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)


namespace Driver {
    struct ReadRequest {
        PVOID addr;
        PVOID buffer;
        SIZE_T size;
    };

    struct ReadResult {
        PVOID buffer;
        SIZE_T size;
    };

    struct WriteRequest {
        PVOID addr;
        PVOID buffer;
        SIZE_T size;
    };

    struct WriteResult {
        SIZE_T size;
    };

    struct AllocRequest {
        PVOID addr;
        SIZE_T size;
    };

    struct AllocResult {
        PVOID addr;
        SIZE_T size;
    };

    struct ThreadRequest {
        PVOID addr;
        PVOID params;
    };

    struct ThreadResult {
        PVOID addr;
    };
}
