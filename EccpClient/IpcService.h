#pragma once
#include <string>
namespace IpcService 
{
	void StartIpcService(const std::string& pipe_name, void* async);
	void Write(const std::string& message);
//    void StopLoop();
}

