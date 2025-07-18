#pragma once
#include <print>
using std::println;

#define print_ptr(ptr) println("{:s} = {:X}", #ptr, reinterpret_cast<DWORD_PTR>(ptr));
#define cast_ptr(ptr) reinterpret_cast<DWORD_PTR>(ptr)
