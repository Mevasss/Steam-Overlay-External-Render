#pragma once
#include <iostream>

class Logs
{
public:
	inline void Log(std::string log)
	{
		std::cout << "[~] " << log << std::endl;
	}
	inline void WLog(std::string log)
	{
		std::cout << "[!] " << log << std::endl;
	}
	inline void ELog(std::string log)
	{
		std::cout << "[ERR] " << log << std::endl;
	}
}; inline Logs L;