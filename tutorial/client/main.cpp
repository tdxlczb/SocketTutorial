// client01.cpp : 定义控制台应用程序的入口点。
//

#include <memory>
#include <iostream>
#include "client_sample.h"
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>

void GetHostTest()
{
    //struct hostent *host;

    auto host = gethostbyname("www.seewo.com");

    printf("officcial name: %s\n", host->h_name);
    for (int i = 0; host->h_aliases[i]; i++)
    {
        printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);
    }

    printf("Address type: %s\n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");

    for (int i = 0; host->h_addr_list[i]; i++)
    {
        printf("IP addr %d: %s\n", i + 1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
    }
}

int main()
{
    std::shared_ptr<ClientSample> s1 = std::make_shared<ClientSample>();
    //s1->Func02();
    s1->Multicast();
    getchar();
    return 0;
}


