// server01.cpp : 定义控制台应用程序的入口点。
//

#include <memory>
#include <thread>
#include "server_sample.h"
#pragma comment(lib, "Ws2_32.lib")

int main()
{
    std::shared_ptr<ServerSample> s1 = std::make_shared<ServerSample>();
    //s1->Func02();
    s1->Multicast();
    getchar();
    return 0;
}

