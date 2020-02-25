#include "IpcChannel.h"
#include "IpcService.h"
#include <thread>
#include <future>
#include <uv.h>
#include <iostream>

static const std::string account = "account";
static const std::string appId = "appId";
static const std::string appToken = "appToken";
static const std::string langue = "langue";
static const std::string realName = "realName";
static const std::string schoolId = "schoolId";
static const std::string userId = "userId";
static const std::string userName = "userName";
static const std::string classId = "classId";
static const std::string lessonId = "lessonId";
static const std::string reserver1 = "reserver1";
static const std::string reserver2 = "reserver2";
static const std::string mainClassId = "mainClassId";
static const std::string groupId = "groupId";
static uv_async_t main_async;

void IpcChannel::MessageRouter(const std::string & type, const Json::Value & data)
{
	if (type == "config")
	{
		if (data.isString())
		{
			auto data_string = data.asString();
			Json::Value root = parseJson(data_string.data(), data_string.data() + data_string.length());
			if (!root.empty())
			{
				ccp_config *conf = new ccp_config();
				if (root.isMember(langue))
				{
					conf->lang = root[langue].asString() == "0" ? "zh_CN" : "";
				}
				if (root.isMember(reserver1))
				{
					conf->reserver1 = root[reserver1].asString();
				}
				if (root.isMember(reserver2))
				{
					conf->reserver2 = root[reserver2].asInt64();
				}
				listener->OnInitUikit(conf);
			}
		}
		Send(RecvConfig);
	}
	else if (type == "openClassroom")
	{
		if (data.isString())
		{
			auto data_string = data.asString();
			Json::Value root = parseJson(data_string.data(), data_string.data() + data_string.length());
			if (!root.empty())
			{
				ccp_authinfo *info = new ccp_authinfo();
				if (root.isMember(account))
				{
					info->account = (uint32_t)std::stoul(root[account].asString());
				}
				if (root.isMember(schoolId))
				{
					info->schoolId = root[schoolId].asString();
				}
				if (root.isMember(classId))
				{

				}
				if (root.isMember(lessonId))
				{
					info->classId = root[lessonId].asString();
				}
				if (root.isMember(userId))
				{

				}
				if (root.isMember(appId))
				{
					info->appId = root[appId].asString();
				}
				if (root.isMember(appToken))
				{
					info->appToken = root[appToken].asString();
				}
				if (root.isMember(realName))
				{
					info->realName = root[realName].asString();
				}
				if (root.isMember(userName))
				{
					info->userName = root[userName].asString();
				}
				if (root.isMember(mainClassId))
				{
					info->mainClassId = root[mainClassId].asString();
				}
				if (root.isMember(groupId))
				{
					info->groupId = root[groupId].asString();
				}
				listener->OnEnterClassroom(info);
			}
		}
		Send(RecvOpenClass);
	}
	else if (type == "closeClassroom")
	{
		listener->OnLeaveClassroom();
	}
}

Json::Value IpcChannel::parseJson(const char * begin, const char * end)
{
	Json::Value root;
	Json::String errs;
	bool ret = reader->parse(begin, end, &root, &errs);
	if (ret && errs.empty())
	{
		return root;
	}
	return Json::Value();
}

IpcChannel::IpcChannel()
{
	//reader = std::make_unique<Json::CharReader>( builder.newCharReader());
}

IpcChannel::~IpcChannel()
{
	if (ipc_thread)
	{
		ipc_thread->join();
	}
}

IpcChannel & IpcChannel::GetInstance()
{
	static IpcChannel ins;
	return ins;
}

void IpcChannel::SetListener(IpcListener * listener)
{
	this->listener = listener;
}

typedef struct {
    const char* first;
    int len;
}MyStr;
void IpcChannel::Read(const std::string & content)
{
    MyStr str={};
    str.first = content.c_str();
    str.len = (int)content.length();
    main_async.data = (void*)&str;
    uv_async_send(&main_async);
}

void IpcChannel::Send(int value)
{
	Json::Value data;
	data["data"] = value;
	data["type"] = "response";
	Json::Value root;
	root["type"] = "data";
	root["data"] = data;
	const std::string out = Json::writeString(writer, root);
	IpcService::Write(out+"\f");
}

static void mainAsyncCb(uv_async_t *handle)
{
    std::cout<<"mainAsyncCb threadID: "<<std::this_thread::get_id()<<std::endl;
    auto myStr = (MyStr*)handle;
    std::string filterMessage {myStr->first,(size_t)myStr->len};

    std::cout<<"mainAsyncCb content: "<<filterMessage.c_str()<<std::endl;
    size_t index = 0;
    do {
        index = filterMessage.find("\f");
        if (index == 0 || index == std::string::npos)
        {
            break;
        }
        std::string subMessage = filterMessage.substr(0, index);
        const char* doc = subMessage.data();
        size_t length = std::strlen(doc);
        length = subMessage.length();
        Json::String errs;
        Json::Value root = IpcChannel::GetInstance().parseJson(doc, doc + length);
        if (!root.empty())
        {
            if (root.isMember("type"))
            {
                auto value = root["type"];
                if (root.isMember("data"))
                {
                    auto data = root["data"];
                    IpcChannel::GetInstance().MessageRouter(value.asString(), root["data"]);
                }
            }
        }
        filterMessage = filterMessage.substr(index + 1, filterMessage.length());
    } while (index != std::string::npos);
}

void IpcChannel::InitIpc(const std::string & pipe_name)
{
    std::cout<<"InitIpc threadId: "<<std::this_thread::get_id()<<std::endl;
    uv_async_init(uv_default_loop(), &main_async, mainAsyncCb);
    ipc_thread = std::make_unique<std::thread>([pipe_name]{
        IpcService::StartIpcService(pipe_name);
    });
}
