#include "common.h"
#include "Logger.h"
#include <iostream>

namespace Util {
	bool isWriteLog_ = false;

#ifdef __WIN32
	std::string SysWideToMultiByte(const std::wstring& wide, uint32_t code_page) {
		int wide_length = static_cast<int>(wide.length());
		if (wide_length == 0)
			return std::string();

		// Compute the length of the buffer we'll need.
		int charcount = WideCharToMultiByte(code_page, 0, wide.data(), wide_length,
			NULL, 0, NULL, NULL);
		if (charcount == 0)
			return std::string();

		std::string mb;
		mb.resize(charcount);
		WideCharToMultiByte(code_page, 0, wide.data(), wide_length,
			&mb[0], charcount, NULL, NULL);

		return mb;
	}
	std::wstring ASCIIToWide(const std::string& mb)
	{
		if (mb.empty())
			return std::wstring();

		int mb_length = static_cast<int>(mb.length());
		// Compute the length of the buffer.
		int charcount = MultiByteToWideChar(CP_UTF8, 0,
			mb.data(), mb_length, NULL, 0);
		if (charcount == 0)
			return std::wstring();

		std::wstring wide;
		wide.resize(charcount);
		MultiByteToWideChar(CP_UTF8, 0, mb.data(), mb_length, &wide[0], charcount);

		return wide;
	}
#endif
    
	void Logger(const std::string& functionName, int line, const std::string & content)
	{
		if(isWriteLog_)
		{
			Logger::getInstance().push("["+functionName+"] "+"[lineNo:"+std::to_string(line)+"]" +content);
		}
		else
		{
			std::cout << content << std::endl;
		}
	}

	void isWriteLog(bool writeLog)
	{
		isWriteLog_ = writeLog;
	}
}
