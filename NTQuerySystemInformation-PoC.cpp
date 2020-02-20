// PoC: NTQuerySystemInformation() using SystemHandleInformation information class (Bruno Oliveira @mphx2)

#include <windows.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#define NT_SUCCESS(x) ((x) >= 0)

PVOID ProcAddress;

typedef NTSTATUS(NTAPI* _NtQuerySystemInformation)(
	ULONG SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
	);

typedef struct SYSTEM_HANDLE
{
	ULONG ProcessId;
	UCHAR ObjectTypeNumber;
	UCHAR Flags;
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE_INFORMATION_, * PSYSTEM_HANDLE_INFORMATION_;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG HandleCount;
	SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

PVOID FindBaseAddress(ULONG pid) {
	HINSTANCE hNtDLL = LoadLibraryA("ntdll.dll");
	PSYSTEM_HANDLE_INFORMATION buffer;
	ULONG bufferSize = 0xffffff;
	buffer = (PSYSTEM_HANDLE_INFORMATION)malloc(bufferSize);
	NTSTATUS status;

	_NtQuerySystemInformation NtQuerySystemInformation =_NtQuerySystemInformation(GetProcAddress(hNtDLL, "NtQuerySystemInformation"));
	status = NtQuerySystemInformation(0x10, buffer, bufferSize, NULL); // 0x10 = SystemHandleInformation
	if (!NT_SUCCESS(status)) {
		printf("NTQueryInformation Failed!\n");
		exit(-1);
	}

	for (ULONG i = 0; i < buffer->HandleCount; i++) {
		if (buffer->Handles[i].ProcessId == pid) {
			ProcAddress = buffer->Handles[i].Object;
		}

	}
	free(buffer);
	return ProcAddress;

}

void main()

{
	printf("NTQuerySystemInformation() PoC -- Bruno Oliveira @mphx2\n");
	PVOID result;
	result = FindBaseAddress(GetCurrentProcessId());
	printf("The kernel address of the current process is 0x%p\n", result);
	 
}
