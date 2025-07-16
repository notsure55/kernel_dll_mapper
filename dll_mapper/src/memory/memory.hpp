#pragma once

#include <print>
#include <windows.h>
#include <psapi.h>
#include <Processthreadsapi.h>
#include <iostream>
#include <string>
#include <tlhelp32.h>

namespace Memory {
    DWORD get_pid(const char* p_name);
    MODULEENTRY32 get_module(const char* module_name, const DWORD pid);
}
