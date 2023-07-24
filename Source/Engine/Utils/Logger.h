#pragma once
#include <cstdio>

#include "windows.h"

enum LogLevel
{
	INFO,
	WARNING,
	ERR,
	DEBUG,
	MAX
};

class Logger
{
public:
	const char* Levels[LogLevel::MAX] = {"Info", "Warning","Error","Debug"};

	Logger()
	{
		AllocConsole();
		OutputHandle = GetStdHandle(STD_OUTPUT_HANDLE );
	}

	void Log(LogLevel logLevel,const char* pszText, ...)
	{
		if(logLevel == LogLevel::MAX)
		{
			Write("Attempted to Log at MAX level");
			return;
		}

		char buffer[2048] = {};
		va_list aptr;
		int ret;

		va_start(aptr, pszText);
		ret = vsprintf_s(buffer, sizeof(buffer) - 1, pszText, aptr);
		va_end(aptr);

		buffer[ret] = NULL;
		Write("[%s] %s", Levels[logLevel], buffer);
	}

private:
	void Write(const char* pszText, ...)
	{
		char buffer[2048] = {};
		va_list aptr;
		int ret;

		va_start(aptr, pszText);
		ret = vsprintf_s(buffer, sizeof(buffer) - 1 , pszText, aptr);
		va_end(aptr);

		buffer[ret] = '\n';
		DWORD temp;
		WriteFile(OutputHandle, buffer,ret + 1, &temp, nullptr );
	}

private:
	//HANDLE ConsoleHandle{}; 
	HANDLE OutputHandle{};
};

static Logger logger;