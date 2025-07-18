#pragma once
#include <array>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <psapi.h>
#include <thread>

namespace Scanner
{

    enum SigType
    {
        Relative,
        Absolute,
        Basic,
        Jmp,
    };

    struct Sig
    {
        const char* name;
        const char* signature;
        const int offset;
        const SigType type;

        constexpr Sig(const char* name, const char* signature, const int offset, const SigType type)
            : name(name), signature(signature), offset(offset), type(type) {
        }
    };

    constexpr std::array<Sig, 2> sigs
    {
        Sig("world", "48 8b 0d ?? ?? ?? ?? 48 81 c1 a8 24 00 00 e8 ?? ?? ?? ?? c1 e8 1f 34 01 48 83 c4 28 c3", 3, SigType::Relative),
        Sig("anim", "48 8b 0d ?? ?? ?? ?? 41 89 ?? ?? ?? ?? ?? 45 89 ?? ?? ?? ?? ?? e8 ?? ?? ?? ?? 48 8b ?? ?? 48 8b f0 4d 8b ?? f3 41 0f 10 ?? ?? 4d 8b ?? ?? 0f 28 f1 48 89 ?? ??", 3, SigType::Relative)
    };

    extern std::unordered_map<std::string, DWORD_PTR> cached_sigs;

    template <typename T>
    T get(const char* name) {
        return reinterpret_cast<T>(cached_sigs.find(name)->second);
    }

    void cache();
}
