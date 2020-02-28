#pragma once
#include "libCCPUikit.h"
#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include "IpcListener.h"

class WrapUikit:public IpcListener
{
public:
	virtual ~WrapUikit();
	void OnRead(const std::string& content) override;
	void OnInitUikit(ccp_config* conf) override;
	long OnEnterClassroom(ccp_authinfo* info) override;
	void OnLeaveClassroom() override;
	void StartIpc(const std::string& pipe_name);
	void SetUikitCallback();

	static WrapUikit& getInstance()
	{
		static WrapUikit wrap_uikit;
		return wrap_uikit;
	}
private:
	WrapUikit();
	ccp_callback *callback;

    
    
	void Push(std::function<void()> pa);
	void Start();
	void Stop();
	void ThreadFunc();
	bool stop_ = false;
	std::queue<std::function<void()>> task_;
	std::unique_ptr<std::thread> thread_ccp;
	std::mutex mu;
	std::condition_variable cv;
};

