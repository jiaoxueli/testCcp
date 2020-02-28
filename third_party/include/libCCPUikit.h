#pragma once

typedef void * CCPWindowHandle;

#ifdef _MSC_VER
#ifdef LIBCCPUIKIT_EXPORTS
        #define CCP_CALL_API __declspec(dllexport)
    #else
        #define CCP_CALL_API __declspec(dllimport)
    #endif

#elif defined __APPLE__
    #define CCP_CALL_API __attribute__((visibility("default")))
#elif defined __linux__ && !defined __ANDROID__
#define DLLAPI __attribute__((visibility("default")))
#else
    #error  "not support current platform!"
#endif

#include <string>

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		int64_t time;		// = microSecondsSinceEpoch
		int level;
		int threadid;
		std::string message;
		std::string file;
		int line;
	}ccp_loginfo;
	
	typedef struct
	{
		std::string lang;
		std::string reserver1;
		long reserver2;
	}ccp_config;

	typedef struct _ccp_authinfo
	{
		uint32_t account;			// = SSO Id
		std::string schoolId;		// = 机构Id
		std::string classId;		// = 班级Id
		std::string userName;		// = 用户名
		std::string realName;		// = 用户实名
		std::string appId;			// = 登录认证
		std::string appToken;		// = 登录认证
		std::string mainClassId;	// = 主房间ID
		std::string groupId;		// = 主房间内组ID

		_ccp_authinfo()
		{
			account = 0;
		}
} ccp_authinfo;

	typedef struct _ccp_callback
	{
		void(*onEnterClassRoom)(int error){};
		void(*onClassRoomWillClosed)(){};
		void(*onClassRoomDidClosed)(){};
		void(*onCCPLogCallback)(const ccp_loginfo & log){};

		_ccp_callback()
		{
			memset(this, 0, sizeof(_ccp_callback));
		}
	} ccp_callback;


	// 机器检测结果
	typedef struct _ccp_machineInfo {
		int micResult;			// 麦克风检测结果
		int micCount;			// 麦克风数量
		int micSelect;			// 选择结果
		int camResult;
		int camCount;
		int camSelect;
		int speakerResult;
		int speakerCount;
		int speakerSelect;
		int negDelay;			// 网络延迟
		int packetLoss;			// 丢包率
		std::string ip;			// ip
		std::string cpu;		// cup信息
		std::string os;			// 操作系统信息
	}ccp_machineInfo;
	typedef void(*onCheckMachine)(const ccp_machineInfo &, int statu);
	

	CCP_CALL_API void libCCPClientInitialize(const ccp_config & config);
	CCP_CALL_API void libCCPSetCallBack(const ccp_callback * cb);
	CCP_CALL_API long libEnterClassRoom(const ccp_authinfo * auth);
	CCP_CALL_API void libLeaveClassRoom(); 
	CCP_CALL_API const char * libCCPVersion();

	CCP_CALL_API void libCheckMachine(onCheckMachine);

    CCP_CALL_API CCPWindowHandle libGetMainWindow();

#ifdef __cplusplus
}
#endif


