#include "WrapUikit.h"
#include <string>
#include <iostream>
#include "IpcChannel.h"
#include "common.h"

WrapUikit::WrapUikit()
{
	callback = nullptr;
	IpcChannel::GetInstance().SetListener(this);
}


WrapUikit::~WrapUikit()
{
	if (callback != nullptr)
	{
		delete callback;
		callback = nullptr;
	}
}

void WrapUikit::OnRead(const std::string & content)
{
}

void WrapUikit::StartIpc(const std::string & pipe_name)
{
	IpcChannel::GetInstance().InitIpc(pipe_name);
}

void WrapUikit::OnInitUikit(ccp_config *conf)
{
//	libCCPClientInitialize(*conf);
	SetUikitCallback();
}

void WrapUikit::SetUikitCallback()
{
	if (callback == nullptr)
	{
		callback = new ccp_callback();
		callback->onCCPLogCallback = [](const ccp_loginfo & log) {
			LOG("log");

		};
		callback->onClassRoomWillClosed = [] {
			LOG("will closed");
		};
		callback->onClassRoomDidClosed = [] {
			LOG("classroom did closed");
			IpcChannel::GetInstance().Send(CallbackRet::CloseClass);
		};
		callback->onEnterClassRoom = [](int error) {
            std::cout<<"threadID: "<<std::this_thread::get_id()<<" enterclassRoom: "<<error<<std::endl;
			LOG("error number " + std::to_string(error));
			//		if (error == 200 || error==0)
			{
				IpcChannel::GetInstance().Send(CallbackRet::JoinClass);
			}
		};
	}
    std::cout<<"setCallback threadId: "<<std::this_thread::get_id()<<std::endl;
	libCCPSetCallBack(callback);
}

long WrapUikit::OnEnterClassroom(ccp_authinfo* info)
{
#ifdef __WIN32
    libEnterClassRoom(info);
#else
    int argc=0;
    char* argv[]={(char*)"123",(char*)""};
    libEnterLiveClassRoomForMac(argc, argv, nullptr);
#endif
    return 0;
}

void WrapUikit::OnLeaveClassroom()
{
	libLeaveClassRoom();
}

void WrapUikit::Push(std::function<void()> pa) {
	std::lock_guard<std::mutex> ga(mu);
	task_.push(pa);
	cv.notify_one();
}

void WrapUikit::Start()
{
	thread_ccp = std::make_unique<std::thread>(std::bind(&WrapUikit::ThreadFunc, this));
}

void WrapUikit::Stop()
{
	auto stop_func = [this] { stop_ = true; };
	Push(stop_func);
	thread_ccp->join();
}

void WrapUikit::ThreadFunc()
{
	while (!stop_)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> ul(mu);
			cv.wait(ul, [this] { return !task_.empty(); });
			task = task_.front();
			task_.pop();
		}
		task();
	}
}
