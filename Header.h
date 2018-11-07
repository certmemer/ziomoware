#pragma once
#define VC_EXTRALEAN
#include <windows.h>
#include <TlHelp32.h>
#pragma comment (lib, "ntdll.lib")

/*turbo imports c+p ntinternals*/
EXTERN_C NTSYSAPI NTSTATUS NTAPI NtReadVirtualMemory(
    IN 	HANDLE	ProcessHandle,
    IN  PVOID	BaseAddress,
    OUT PVOID	Buffer,
    IN 	ULONG   NumberOfBytesToRead,
    OUT PULONG  NumberOfBytesReaded  OPTIONAL);

EXTERN_C NTSYSAPI NTSTATUS NTAPI NtWriteVirtualMemory(
    IN 	HANDLE	ProcessHandle,
    IN 	PVOID	BaseAddress,
    IN  PVOID	Buffer,
    IN 	ULONG   NumberOfBytesToWrite,
    OUT PULONG  NumberOfBytesWritten OPTIONAL);

class c_mem
{
private:
    DWORD       m_id{};
    HANDLE      m_handle{};
public:
    auto open_process(const char* process_name) {
        PROCESSENTRY32  proc_entry{};
        HANDLE          t32_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        proc_entry.dwSize = sizeof(proc_entry);

        for (Process32First(t32_snap, &proc_entry); Process32Next(t32_snap, &proc_entry);) {
            if (!strcmp(proc_entry.szExeFile, process_name)) {
                m_id = proc_entry.th32ProcessID;
                m_handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, 0, m_id);
                break;
            }
        }
        CloseHandle(t32_snap);

        return !!m_id;
    }

    auto get_module(const char* module_name) {
        DWORD           address{};
        MODULEENTRY32   mod_entry{};
        HANDLE          t32_snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_id);
        mod_entry.dwSize = sizeof(mod_entry);

        for (Module32First(t32_snap, &mod_entry); Module32Next(t32_snap, &mod_entry);) {
            if (!strcmp(mod_entry.szModule, module_name)) {
                address = DWORD(mod_entry.modBaseAddr);
                break;
            }
        }
        CloseHandle(t32_snap);

        return address;
    }

    template <typename T> auto write(DWORD address, const T& value) {
        NtWriteVirtualMemory(
            m_handle,
            reinterpret_cast<PVOID>(address),
            reinterpret_cast<PVOID>(const_cast<T*>(&value)),
            sizeof(T),
            nullptr);
    }

    template <typename T> T read(DWORD address, ULONG size = sizeof(T)) {
        T buf{};

        NtReadVirtualMemory(
            m_handle,
            reinterpret_cast<PVOID>(address),
            reinterpret_cast<PVOID>(&buf),
            size,
            nullptr);

        return buf;
    }
};
extern c_mem memory;