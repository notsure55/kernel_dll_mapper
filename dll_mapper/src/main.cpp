#include "driver/driver.hpp"
#include "memory/memory.hpp"
#include <winternl.h>

#include <print>
#include <vector>
#include <fstream>
#include <utility>

#define cast_ptr(ptr) reinterpret_cast<DWORD_PTR>(ptr)
#define rva_to_ptr(type, base_address, rva) reinterpret_cast<type>(cast_ptr(base_address) + rva)
#define print_error(string, ...) \
		std::println(string, __VA_ARGS__); \
        std::cin.get(); \
        exit(EXIT_FAILURE)

std::vector<char> read_dll_bytes(const std::string& module_path) {
	std::ifstream file_stream(module_path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

	const auto size{ file_stream.tellg() };
	file_stream.seekg(0, std::ios::beg);

	std::vector<char> bytes(size);
	file_stream.read(bytes.data(), size);

	return bytes;
}

void* write_bytes_to_process(const std::vector<char>& bytes) {
	const auto dos_header{ reinterpret_cast<const IMAGE_DOS_HEADER*>(bytes.data()) };
	const auto nt_header{ reinterpret_cast<const IMAGE_NT_HEADERS*>(bytes.data() + dos_header->e_lfanew) };

	auto* const base_address{ Driver::alloc(nullptr, static_cast<SIZE_T>(nt_header->OptionalHeader.SizeOfImage)) };

	if (base_address == nullptr) {
		print_error("ERROR: FAILED TO ALLOCATE MEMORY FOR DLL STUB");
	}

	const auto* section{ IMAGE_FIRST_SECTION(nt_header) };

	for (size_t i{ 0 }; i < nt_header->FileHeader.NumberOfSections; ++i, ++section) {
		const auto name{ std::string(reinterpret_cast<const char*>(section->Name), 8) };

		size_t ret{};
		if (!Driver::write(
			reinterpret_cast<PVOID>(cast_ptr(base_address) + section->VirtualAddress),
			reinterpret_cast<PVOID>(cast_ptr(bytes.data()) + section->PointerToRawData),
			static_cast<SIZE_T>(section->SizeOfRawData)
		)) {
			print_error("ERROR: FAILED TO WRITE MEMORY TO SECTION {:X}", cast_ptr(base_address) + section->VirtualAddress);
		}
	}

	if (!Driver::write(base_address, reinterpret_cast<PVOID>(cast_ptr(bytes.data())), 4096)) {

	}

	return base_address;
}

using load_library_p = HMODULE(__stdcall*)(LPCSTR lpLibFileName);
using get_proc_address_p = FARPROC(__stdcall*)(HMODULE hModule, LPCSTR lpProcName);

struct Params {
	void* const base_address;
	load_library_p load_library;
	get_proc_address_p get_proc_address;
};

void relocation_stub(Params* params) {
	const auto dos_header{ reinterpret_cast<IMAGE_DOS_HEADER*>(params->base_address) };
	const auto nt_header{ reinterpret_cast<IMAGE_NT_HEADERS*>(cast_ptr(params->base_address) + dos_header->e_lfanew) };
	const auto delta{ cast_ptr(params->base_address) - nt_header->OptionalHeader.ImageBase };

	const auto reloc_dir{ nt_header->OptionalHeader.DataDirectory[5] };
	const auto* reloc_entries{ reinterpret_cast<IMAGE_BASE_RELOCATION*>(cast_ptr(params->base_address) + reloc_dir.VirtualAddress) };

    #pragma pack(push, 1)
	struct BASE_RELOCATION_BLOCK {
		WORD Offset : 12;
		WORD Type : 4;
	};
    #pragma pack(pop)

	while (reloc_entries->VirtualAddress != 0) {
		const auto block_count{ (reloc_entries->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD) };
		auto block{ reinterpret_cast<BASE_RELOCATION_BLOCK*>(cast_ptr(reloc_entries) + 1) };

		for (size_t i{ 0 }; i < block_count; ++i, ++block) {
			if (block->Type == IMAGE_REL_BASED_DIR64) {
				auto* const address{ reinterpret_cast<DWORD_PTR*>(
					block->Offset +
					reloc_entries->VirtualAddress +
					cast_ptr(params->base_address)
					) };

				*address += delta;
			}
		}
		
		reloc_entries = reinterpret_cast<IMAGE_BASE_RELOCATION*>(cast_ptr(reloc_entries) + reloc_entries->SizeOfBlock);
	}

	const auto imp_dir{ nt_header->OptionalHeader.DataDirectory[1] };
	auto ies{ reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(cast_ptr(params->base_address) + imp_dir.VirtualAddress) };

	// loop through all modules and import all functions
	for (size_t i{0}; ies[i].Characteristics != 0; ++i) {
		const auto import_des = ies[i];
		const auto module_handle{ params->load_library(rva_to_ptr(char*, params->base_address, import_des.Name)) };

		auto* address_table_entry{ rva_to_ptr(IMAGE_THUNK_DATA*, params->base_address, import_des.FirstThunk) };
		auto* name_table_entry{ rva_to_ptr(IMAGE_THUNK_DATA*, params->base_address, import_des.OriginalFirstThunk) };

		if (name_table_entry == nullptr) {
			name_table_entry = address_table_entry;
		}

		for (; name_table_entry->u1.Function != 0; ++name_table_entry, ++address_table_entry) {
			
			// grab imported_fnc so we can use name later
			const auto* const imported_fnc{ rva_to_ptr(IMAGE_IMPORT_BY_NAME*,
				params->base_address, name_table_entry->u1.AddressOfData)
			};

			// check if ordinal
			const bool is_ordinal{ IMAGE_SNAP_BY_ORDINAL(name_table_entry->u1.Ordinal)};

			// if ordinal call proc address by ordinal instead of name
			if (is_ordinal) {
				auto ordinal = IMAGE_ORDINAL(name_table_entry->u1.Ordinal);
				address_table_entry->u1.Function = reinterpret_cast<DWORD_PTR>(
					params->get_proc_address(module_handle, reinterpret_cast<const char*>(ordinal)));
			}
			else {
				address_table_entry->u1.Function = reinterpret_cast<DWORD_PTR>(
					params->get_proc_address(module_handle, imported_fnc->Name));
			}
		}
	}

	// TLS CALLBACK CALLINGTONS

	const auto tls_dir{ nt_header->OptionalHeader.DataDirectory[9] };
	if (tls_dir.Size > 0 && tls_dir.VirtualAddress != 0) {
		const auto tls_table{ rva_to_ptr(IMAGE_TLS_DIRECTORY*, params->base_address, tls_dir.VirtualAddress) };
		const auto* tls_callback{ reinterpret_cast<PIMAGE_TLS_CALLBACK*>(tls_table->AddressOfCallBacks) };

		while (*tls_callback) {
			(*tls_callback)(params->base_address, DLL_PROCESS_ATTACH, nullptr);
			tls_callback++;
		}
	}

	using dll_main_p = BOOL(__stdcall*)(HINSTANCE hinstDLL,
		DWORD fdwReason, LPVOID lpvReserved);

	const auto DllMain{ rva_to_ptr(dll_main_p, params->base_address, nt_header->OptionalHeader.AddressOfEntryPoint)};

	DllMain(reinterpret_cast<HINSTANCE>(params->base_address), DLL_PROCESS_ATTACH, nullptr);
}

std::pair<void*, void*> write_reloc_stub(const Params& params) {
	auto* const stub_addr{ Driver::alloc(nullptr, 4096) };

	if (stub_addr == nullptr) {
		print_error("ERROR: FAILED TO ALLOCATE MEMORY FOR STUB");
	}

	auto result{ Driver::write(stub_addr, relocation_stub, 4096) };
	if (!result) {
		print_error("ERROR: FAILED TO WRITE STUB TO STUB_ADDR");
	}

	auto* const params_addr{ Driver::alloc(nullptr, sizeof(Params)) };

	if (params_addr == nullptr) {
		print_error("ERROR: FAILED TO ALLOCATE MEMORY FOR PARAMS");
	}

	result = Driver::write(params_addr, (PVOID)&params, sizeof(Params));
	if (!result) {
		print_error("ERROR: FAILED TO WRITE PARAMS TO MEMORY");
	}

	return std::make_pair(stub_addr, params_addr);
}

template<typename T>
T get_remote_func_address(const std::string module_name,
	const std::string func_name, const DWORD pid) {

	const auto module{ GetModuleHandle(module_name.c_str()) };
	if (module == nullptr) {
		print_error("ERROR: FAILED TO GET HANDLE TO {} MODULE", module_name);
	}

	const auto func_address{ GetProcAddress(module, func_name.c_str()) };
	const auto func_offset{ cast_ptr(func_address) - cast_ptr(module) }; // calc offset for func so we can get 

	const auto remote_module{ Memory::get_module(module_name.c_str(), pid) };

	if (remote_module.modBaseAddr == 0) {
		print_error("ERROR: FAILED TO GET REMOTE MODULE {}", module_name);
	}

	return reinterpret_cast<T>(remote_module.modBaseAddr + func_offset);
}

// 
int main(int argc, const char* argv[]) {
	//const std::string dll_path{"C:\\Users\\schwarztoter\\source\\repos\\test_dll\\x64\\Release\\test_dll.dll"};
	const auto pid{ atoi(argv[1]) };
	const std::string dll_path{ argv[2] };
	const auto bytes{ read_dll_bytes(dll_path) };

	//const auto pid{ Memory::get_pid("cs2.exe") };
	Driver::init(pid);
	const auto p_handle{ OpenProcess(PROCESS_ALL_ACCESS, false, pid) };

	auto* const base_address{ write_bytes_to_process(bytes) };
	std::println("base_address: {:X}", cast_ptr(base_address));

	const Params params = {
		.base_address = base_address,
		.load_library = get_remote_func_address<load_library_p>("KERNEL32.DLL", "LoadLibraryA", pid),
		.get_proc_address = get_remote_func_address<get_proc_address_p>("KERNEL32.DLL", "GetProcAddress", pid),
	};

	const auto reloc_info{ write_reloc_stub(params) };
	//  0x00c9 
	/*CreateRemoteThreadEx(
	p_handle,
	nullptr,
	0,
	reinterpret_cast<LPTHREAD_START_ROUTINE>(reloc_info.first),
	reloc_info.second,
	0,
	nullptr,
	nullptr
	);*/
	
	Driver::create_thread(reloc_info.first, reloc_info.second);


	std::println("Injected!");
	Sleep(5000);
	return 0;
}