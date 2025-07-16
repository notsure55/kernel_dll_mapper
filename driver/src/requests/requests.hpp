#pragma once
#include "../driver.hpp"

using ZwCreateThreadEx_t = NTSTATUS(NTAPI*) (
	OUT PHANDLE hThread,
	IN ACCESS_MASK DesiredAccess,
	IN PVOID ObjectAttributes,
	IN HANDLE ProcessHandle,
	IN PVOID lpStartAddress,
	IN PVOID lpParameter,
	IN ULONG Flags,
	IN SIZE_T StackZeroBits,
	IN SIZE_T SizeOfStackCommit,
	IN SIZE_T SizeOfStackReserve,
	IN PVOID AttributeList
	);


namespace Requests {
	NTSTATUS attach_process(const PVOID system_buffer);
	NTSTATUS write_memory(const PVOID system_buffer, Driver::WriteResult* result);
	NTSTATUS read_memory(const PVOID system_buffer, Driver::ReadResult* result);
	NTSTATUS alloc_memory(const PVOID system_buffer, Driver::AllocResult* result);
	NTSTATUS create_thread(const PVOID system_buffer);
}