#pragma once
#include <string>

#define MY_CONFIG WM_USER+100
#define MY_JOINCLASS WM_USER+101
#define MY_CLOSECLASS WM_USER+102

const std::wstring appID = L"com.koolearn.kooup";
#define LOG(content) Util::Logger(__FUNCTION__,__LINE__,content)
namespace Util {
#ifdef _WIN32
	std::string SysWideToMultiByte(const std::wstring& wide, uint32_t code_page);
	std::wstring ASCIIToWide(const std::string& str);
#endif
	void Logger(const std::string& functionName,int line, const std::string& content);
	void isWriteLog(bool writeLog);
}
