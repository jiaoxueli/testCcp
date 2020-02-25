#include "Logger.h"
#include "common.h"

#ifdef window
#include <Shlobj_core.h>
#endif

#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>
#include <spdlog/spdlog.h>

std::string Logger::getAppdata()
{
#ifdef window
	TCHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		return Util::SysWideToMultiByte(szPath, CP_UTF8);
	}
	else
	{
		return std::string();
	}
#else
#endif
    return std::string();
}

Logger::Logger()
{
	std::string logPath = getAppdata() + "/LiveCpp/logs/basic.log";
	logger = spdlog::basic_logger_mt("livecpp", logPath);
}

Logger::~Logger()
{
}

Logger & Logger::getInstance()
{
	static Logger instance;
	return instance;
}

void Logger::push(const std::string & content)
{
	logger->info(content);
}
