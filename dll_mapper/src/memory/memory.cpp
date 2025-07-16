#include "memory.hpp"

namespace Memory {
    // https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-process32next
    // https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/ns-tlhelp32-processentry32

    DWORD get_pid(const char* p_name) {
        const auto handle { CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };

        PROCESSENTRY32 p_entry;
        p_entry.dwSize = sizeof(PROCESSENTRY32);

        while(Process32Next(handle, &p_entry)) {
            if (strncmp(p_entry.szExeFile, p_name, strlen(p_name)) == 0) {
                CloseHandle(handle);
                std::println("{}", p_entry.th32ProcessID);
                return p_entry.th32ProcessID;
            }
        }

        CloseHandle(handle);
        return -1;
    }

    MODULEENTRY32 get_module(const char* module_name, const DWORD pid) {
        const auto handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (handle != INVALID_HANDLE_VALUE) {
            MODULEENTRY32 modEntry;
            modEntry.dwSize = sizeof(modEntry);
            if (Module32First(handle, &modEntry)) {
                do {
                    if (!strcmp(modEntry.szModule, module_name)) {
                        CloseHandle(handle);
                        return modEntry;
                    }
                } while (Module32Next(handle, &modEntry));
            }
        }

        CloseHandle(handle);
    }
}
