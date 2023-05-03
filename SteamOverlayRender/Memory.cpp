#include "Memory.h"
#include <TlHelp32.h>
#include <iostream>
#include <iomanip>

DWORD Memory::getProcess(const char* proc)
{
	HANDLE hProcessId = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	do
	{
		if (!strcmp(pEntry.szExeFile, proc))
		{
			processId = pEntry.th32ProcessID;
			CloseHandle(hProcessId);
			handle = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
		}
	} while (Process32Next(hProcessId, &pEntry));
	return processId;
}
DWORD Memory::getModule(const char* modName)
{
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
	MODULEENTRY32 mEntry;
	mEntry.dwSize = sizeof(mEntry);
	do
	{
		if (!strcmp(mEntry.szModule, modName))
		{
			CloseHandle(hModule);
			return (DWORD)mEntry.hModule;
		}
	} while (Module32Next(hModule, &mEntry));
	return 0;
}
DWORD Memory::getAddress(DWORD addr, std::vector<DWORD> vect)
{
	for (int i = 0; i < vect.size(); i++)
	{
		ReadProcessMemory(handle, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += vect[i];
	}
	return addr;
}