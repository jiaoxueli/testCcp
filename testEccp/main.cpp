//
//  main.cpp
//  testEccp
//
//  Created by jiaoxueli on 2020/2/24.
//  Copyright © 2020 jiaoxueli. All rights reserved.
//

#include <iostream>
//#include "libCCPUikit.h"

#include <cxxopts.hpp>
#include "WrapUikit.h"
#include <uv.h>

int main(int argc, char * argv[]) {

//    ccp_callback ccp_cb;
//    ccp_cb.onEnterClassRoom = [](int err) -> void {
//        std::string msg = "******************************onEnterClassRoom：" + std::to_string(err);
//        std::cout << msg.c_str() <<std::endl;
//    };
//    ccp_cb.onClassRoomWillClosed = []() -> void {
//        std::cout << ("############################onClassRoomWillClosed\n");
//    };
//    ccp_cb.onClassRoomDidClosed = []()->void {
//        std::cout << ("############################onClassRoomDidClosed\n");
//    };
//    libCCPSetCallBack(&ccp_cb);
//
//
//    ccp_authinfo cpp_auth;
 //   return libEnterLiveClassRoomForMac(argc, argv, nullptr);
    
    std::cout<<"threadId: "<<std::this_thread::get_id()<<std::endl;
    
    cxxopts::Options options("");
    options
    .positional_help("[options args]")
    .show_positional_help();
    
    options
    .allow_unrecognised_options()
    .add_options()("c,config", "set config", cxxopts::value<std::string>())
    ("p,parameter", "class parameters", cxxopts::value<std::string>())
    ("d,debug", "show console", cxxopts::value<bool>())
    ("h,help", "Print help");
    options.parse_positional({ "config","parameter","debug" });
    
    auto result = options.parse(argc, argv);
    if (result.count("help") > 0)
    {
        std::cout << options.help({ "","Group" }) << std::endl;
        return 0;
    }
    if (result.count("d") > 0)
    {
        //Util::isWriteLog(true);
    }
    //else
    //{
    //    AllocConsole();
    //    freopen("CONOUT$", "w+t", stdout);
    //}
    //std::string pipe_name;
    if (result.count("parameter"))
    {
        auto default_loop = uv_default_loop();
        uv_loop_init(default_loop);
        std::string  pipe_name = result["parameter"].as<std::string>();
        std::cout << pipe_name << std::endl;
        WrapUikit::getInstance().StartIpc(pipe_name);
        uv_run(default_loop, UV_RUN_DEFAULT);
    }
    std::cout<<"over\n";
    return 0;
    
}
