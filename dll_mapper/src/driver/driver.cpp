#include "driver.hpp"

namespace Driver {

    HANDLE hDevice = nullptr;

    void init(int pid) {
        hDevice = CreateFileA("\\\\.\\meatdriver", GENERIC_READ | GENERIC_WRITE,
                                     0, nullptr, OPEN_EXISTING, 0, nullptr);

        if (hDevice == INVALID_HANDLE_VALUE) {
            print_error("ERROR: FAILED TO GET HANDLE TO DEVICE");
        }

        if (!DeviceIoControl(hDevice, IOCTL_OPEN_PROCESS, &pid, sizeof(ULONG), nullptr, 0, nullptr, nullptr)) {
            print_error("ERROR: FAILED TO INIT DRIVER");
        }

        std::println("Driver intialized!");
    }

    bool write(PVOID address, const PVOID& buffer, const SIZE_T& size) {
        WriteResult res{};
        WriteRequest req{
            .addr = address,
            .buffer = buffer,
            .size = size,
        };

        if (!DeviceIoControl(hDevice, IOCTL_WRITE_PROCESS, &req, sizeof(WriteRequest), &res, sizeof(WriteResult), nullptr, nullptr)) {
            print_error("ERROR: WRITE {:X}", cast_ptr(address));
        }

        return res.size == size;
    }

    PVOID alloc(const PVOID address, const SIZE_T size) {

        AllocResult res{};
        AllocRequest req{
            .addr = address,
            .size = size,
        };

        if (!DeviceIoControl(hDevice, IOCTL_ALLOC_PROCESS, &req, sizeof(AllocRequest), &res, sizeof(AllocResult), nullptr, nullptr)) {
            print_error("ERROR: ALLOC");
        }

        return res.addr;
    }

    void create_thread(const PVOID address, const PVOID params) {        
        ThreadRequest req{
            .addr = address,
            .params = params,
        };

        if (!DeviceIoControl(hDevice, IOCTL_CREATE_THREAD_PROCESS, &req, sizeof(ThreadRequest), nullptr, NULL, nullptr, nullptr)) {
            print_error("ERROR: THREAD_CREATE");
        }
    }

    void close_handle() {
        CloseHandle(hDevice);
    }
}
