#pragma once
#include <print>
#include <Windows.h>
#include <psapi.h>
#include <Processthreadsapi.h>
#include <iostream>

#define cast_ptr(ptr) reinterpret_cast<DWORD_PTR>(ptr)
#define rva_to_ptr(type, base_address, rva) reinterpret_cast<type>(cast_ptr(base_address) + rva)
#define print_error(string, ...) \
		std::println(string, __VA_ARGS__); \
        std::cin.get(); \
        exit(EXIT_FAILURE)

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

    extern HANDLE hDevice;
	
	void init(int pid);
	
	template <typename T>
	T read(PVOID& address)
	{
        ReadResult res{};
        ReadRequest req{
            .addr = address,
            .size = sizeof(T),
            .buffer = malloc(sizeof(T)),
        };

		if (!DeviceIoControl(hDevice, IOCTL_READ_PROCESS, &req, sizeof(ReadRequest), &res, sizeof(ReadResult), nullptr, nullptr)) {
			print_error("ERROR: READ {:X} FAILED", cast_ptr(address));
        }

		if (res.size != sizeof(T) || res.buffer == nullptr) {
			print_error("ERROR: READ {:X} FAILED BUFFER OR SIZE IS WRONG", cast_ptr(address));
		}

        return *reinterpret_cast<T*>(res.buffer);
    }

    bool write(PVOID address, const PVOID& buffer, const SIZE_T& size);
    PVOID alloc(const PVOID address, const SIZE_T size);
	void create_thread(const PVOID address, const PVOID params);
}
