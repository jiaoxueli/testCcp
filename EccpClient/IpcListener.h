#pragma once
#include <string>
#include <libCCPUikit.h>

class IpcListener 
{
public:
	virtual void OnRead(const std::string& content)=0;
	virtual void OnInitUikit(ccp_config* conf) = 0;
	virtual long OnEnterClassroom(ccp_authinfo* info) = 0;
	virtual void OnLeaveClassroom() = 0;
};
