#pragma once

#include <spdlog/logger.h>
#include <string>

class Logger
{
public:
	virtual ~Logger();
	static Logger& getInstance();

	void push(const std::string& content);
protected:
	std::string getAppdata();
private:
	Logger();
	std::shared_ptr<spdlog::logger> logger;
};

