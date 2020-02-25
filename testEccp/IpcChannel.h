#pragma once
#include "IpcListener.h"
#include  "json/json/json.h"
#include <thread>
#include <uv.h>

enum CallbackRet
{
	JoinClass = 6,
	CloseClass = 16,
	IpcConnect = 10001,
	RecvConfig = 10002,
	RecvOpenClass = 10003,
};

class IpcChannel
{
public:
	virtual ~ IpcChannel();
	static IpcChannel& GetInstance();
	void SetListener(IpcListener* listener);
	void Read(const std::string& content);
	void Send(int value);
	void InitIpc(const std::string& pipe_name);  
 

	void MessageRouter(const std::string& type, const Json::Value& data);
	Json::Value parseJson(const char* begin, const char* end);

private:
	IpcChannel();
	IpcChannel(const IpcChannel& other) = delete;
	IpcChannel operator =(const IpcChannel& other) = delete;
	IpcListener *listener;
	Json::CharReaderBuilder builder;
	std::unique_ptr<Json::CharReader> reader{ builder.newCharReader() };
	Json::StreamWriterBuilder writer;
	std::unique_ptr<std::thread> ipc_thread;
};

