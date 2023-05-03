#pragma once
#include <Windows.h>
#include <vector>

class Memory
{
public:
	Memory()
	{
		handle = NULL;
	}
	~Memory()
	{
		CloseHandle(handle);
	}
	
	template <class T>
	T readMem(DWORD addr)
	{
		T x;
		ReadProcessMemory(handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
		return x;
	}
	template <class T>
	T writeMem(DWORD addr, T x)
	{
		WriteProcessMemory(handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
		return x;
	}

	DWORD getProcess(const char*);
	DWORD getModule(const char*);
	DWORD getAddress(DWORD, std::vector<DWORD>);
private:
	HANDLE handle;
	DWORD processId = 0;
}; 
