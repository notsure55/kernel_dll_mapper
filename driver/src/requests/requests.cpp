#include "requests.hpp"
#include "../scanner/scanner.hpp"

extern "C" { //undocumented windows internal functions (exported by ntoskrnl)
	NTKERNELAPI NTSTATUS MmCopyVirtualMemory(
		PEPROCESS SourceProcess,
		PVOID SourceAddress,
		PEPROCESS TargetProcess,
		PVOID TargetAddress,
		SIZE_T BufferSize,
		KPROCESSOR_MODE PreviousMode,
		PSIZE_T ReturnSize
	);
}

namespace Requests {

	PEPROCESS process{ nullptr };

	NTSTATUS attach_process(const PVOID system_buffer) {
		const auto pid{ *reinterpret_cast<ULONG*>(system_buffer) };
		return PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(pid), &process);
	}
	NTSTATUS write_memory(const PVOID system_buffer, Driver::WriteResult* result) {

		if (process == nullptr) {
			DbgPrint("ERROR: PROCESS HANDLE INVALID");
			return STATUS_INVALID_DEVICE_REQUEST;
		}

		const auto request{ reinterpret_cast<Driver::WriteRequest*>(system_buffer) };

		auto status = MmCopyVirtualMemory(
			PsGetCurrentProcess(),
			request->buffer,
			process,
			request->addr,
			request->size,
			KernelMode,
			&result->size
		);

		return status;
	}
	NTSTATUS read_memory(const PVOID system_buffer, Driver::ReadResult* result) {

		if (process == nullptr) {
			DbgPrint("ERROR: PROCESS HANDLE INVALID");
			return STATUS_INVALID_DEVICE_REQUEST;
		}
		const auto request{ reinterpret_cast<Driver::ReadRequest*>(system_buffer) };

		auto status = MmCopyVirtualMemory(
			process,
			request->addr,
			PsGetCurrentProcess(),
			result->buffer,
			request->size,
			KernelMode,
			&result->size
		);

		return status;
	}
	NTSTATUS alloc_memory(const PVOID system_buffer, Driver::AllocResult* result) {

		if (process == nullptr) {
			DbgPrint("ERROR: PROCESS HANDLE INVALID");
			return STATUS_INVALID_DEVICE_REQUEST;
		}

		const auto request{ reinterpret_cast<Driver::AllocRequest*>(system_buffer) };

		KAPC_STATE stack{};

		auto addr{ request->addr };
		auto size{ request->size };

		KeStackAttachProcess(
			process,
			&stack
		);

		auto status = ZwAllocateVirtualMemory(
			ZwCurrentProcess(),
			&addr,
			0,
			&size,
			MEM_COMMIT | MEM_RESERVE,
			PAGE_EXECUTE_READWRITE
		);

		KeUnstackDetachProcess(&stack);

		if (NT_SUCCESS(status)) {
			result->addr = addr;
			result->size = size;
		}

		return status;
	}

	NTSTATUS create_thread(const PVOID system_buffer) {

		auto request{reinterpret_cast<Driver::ThreadRequest*>(system_buffer)};

		static ZwCreateThreadEx_t ZwCreateThreadEx{ reinterpret_cast<ZwCreateThreadEx_t>(Scanner::get_ZwCreateThreadEx()) };

		KAPC_STATE stack{};

		KeStackAttachProcess(
			process,
			&stack
		);

		HANDLE t_handle{nullptr};
		auto status{ ZwCreateThreadEx(
			&t_handle,
			THREAD_ALL_ACCESS,
			nullptr,
			ZwCurrentProcess(),
			request->addr,
			request->params,
			NULL,
			NULL,
			NULL,
			NULL,
			nullptr
		) };

		KeUnstackDetachProcess(&stack);

		return status;
	}
}