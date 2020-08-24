#pragma once
#include <iostream>
#include <string>

#if defined(_WIN32)
#include <Windows.h>
#endif

#include <cassert>

#ifdef _DEBUG
#define PE_ASSERT(exp) assert(exp)
#else
#define PE_ASSERT(exp)
#endif

namespace Debug
{
	namespace Flags
	{
		enum LogType
		{
			Message, Warning, Error
		};
	}


	void Log(std::string Text, Flags::LogType type)
	{
	#ifdef _DEBUG

		#if defined(_WIN32)
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		switch (type)
		{
		case Flags::Message: SetConsoleTextAttribute(hConsole, 15); std::cout << Text + "\n";
			break;
		case Flags::Warning: SetConsoleTextAttribute(hConsole, 14); std::cout << "WARNING: " + Text + "\n";
			break;
		case Flags::Error: SetConsoleTextAttribute(hConsole, 12); std::cout << "ERROR: " + Text + "\n";
			break;
		default:
			break;
		}

		SetConsoleTextAttribute(hConsole, 15);

		#else

		switch (type)
		{
		case Flags::Message: std::cout << Text + "\n";
			break;
		case Flags::Warning: std::cout << "Warning: " + Text + "\n";
			break;
		case Flags::Error: std::cout << "Error: " + Text + "\n";
			break;
		default:
			break;

		#endif
	#endif
	}	
}
