#include "scanner.hpp"
#include <ntimage.h>

extern "C" NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation(
	IN int SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);

typedef struct _SYSTEM_MODULE_ENTRY
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[256];
} SYSTEM_MODULE_ENTRY, * PSYSTEM_MODULE_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG Count;
	SYSTEM_MODULE_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

#define SystemModuleInformation 11

namespace Scanner {
	// https://m0uk4.gitbook.io/notebooks/mouka/windowsinternal/find-kernel-module-address-todo
	DWORD_PTR get_ntoskrnel_base_address() {
		ULONG bytes{};
		auto status = ZwQuerySystemInformation(SystemModuleInformation, nullptr, bytes, &bytes);

		auto pMods = (PSYSTEM_MODULE_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, bytes, 'tag');
		RtlZeroMemory(pMods, bytes);

		status = ZwQuerySystemInformation(SystemModuleInformation, pMods, bytes, &bytes);

		return reinterpret_cast<DWORD_PTR>(pMods->Module[0].ImageBase);
	}

	void* get_func(UCHAR signature[], SIZE_T sig_len) {
		const auto base_address{ get_ntoskrnel_base_address() };
		DbgPrint("Back in get_func 0x%llX", base_address);
		const auto dos_header{ reinterpret_cast<IMAGE_DOS_HEADER*>(base_address) };
		const auto nt_header{ reinterpret_cast<IMAGE_NT_HEADERS*>(base_address + dos_header->e_lfanew) };
		const auto* section{ IMAGE_FIRST_SECTION(nt_header) };

		DWORD_PTR text_base{0x0};// { base_address + section->VirtualAddress };
		SIZE_T text_size{0x0};// { section->Misc.VirtualSize }; */

		for (size_t i{0}; i < nt_header->FileHeader.NumberOfSections; ++i, ++section) {
			const auto name{ reinterpret_cast<const char*>(section->Name) };
			if (strncmp(name, ".text", 5) == 0) {
				text_base = base_address + section->VirtualAddress;
				text_size = section->Misc.VirtualSize;
				DbgPrint("text_base: %llX, text_size: %llX", text_base, text_size);
			}
		}

		//return (PVOID)text_size;

		if (text_base != 0 && text_size != 0) {
			PUCHAR text{ reinterpret_cast<PUCHAR>(text_base) };
			if (text != nullptr) {
				for (size_t i{ 0 }; i < text_size - sig_len; ++i) {
					bool found = true;
					for (size_t j{ 0 }; j < sig_len; ++j) {
						if (signature[j] != text[i + j] && signature[j] != 0xCC) {
							found = false;
							break;
						}
					}
					if (found == true) {
						return reinterpret_cast<PVOID>(text_base + i);
					}
				}
				return (PVOID)0x30;
			}
			return (PVOID)0x20;
		}
		else {
			DbgPrint("ERROR: text_base && text size are 0");
		}

		DbgPrint("ERROR: Failed to find signature!");
		return (PVOID)0x10;
	}

	//index 21
	PVOID get_ZwCreateThreadEx() {
		UCHAR bytes[] = {
			0x48, 0x8b, 0xc4,
			0xfa, 
			0x48, 0x83, 0xec, 0x10,
			0x50,
			0x9c,
			0x6a, 0x10,
			0x48, 0x8d, 0x05, 0xCC, 0xCC, 0xCC, 0xCC,
			0x50,
			0xb8, 0xCC, 0xCC, 0xCC, 0xCC,
			0xe9, 0xCC, 0xCC, 0xCC, 0xCC,
			0xc3,
			0x90
		};

		ULONG win_10_sys_call{ 0x000000C2 };
		ULONG win_11_sys_call{ 0x000000C9 };
		*reinterpret_cast<PULONG>(&bytes[21]) = win_11_sys_call;

		return get_func(bytes, sizeof(bytes));
	}
}