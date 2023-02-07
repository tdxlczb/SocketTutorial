#include "server_sample.h"
#include <iostream>
#include <chrono>
#include "foundation/time/time.h"
#include <u2socket/base/socket_define.h>
#include <u2socket/base/socket_utils.h>

#define BUF_SIZE 100

ServerSample::ServerSample()
{
    InitSocket();
}

ServerSample::~ServerSample()
{
    FinishSocket();
}

void ServerSample::InitSocket()
{
#ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "Failed. Error Code: " << WSAGetLastError() << std::endl;
        exit(EXIT_FAILURE);
    }
#endif // WIN32
}

void ServerSample::FinishSocket()
{
#ifdef WIN32
    WSACleanup();
#endif // WIN32
}

void ServerSample::Func01()
{
    //创建套接字
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_HOPOPTS);
    //绑定套接字
    struct sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = AF_INET;  //使用IPv4地址
    sockAddr.sin_addr.s_addr = inet_addr("0.0.0.0");  //具体的IP地址
    sockAddr.sin_port = htons(51772);  //端口
     //sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);  //具体的IP地址
     //sockAddr.sin_port = htons(0);  //端口
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    //进入监听状态
    listen(servSock, 20);
    //接收客户端请求
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    int len = sizeof(addr);
    int ret = getsockname(servSock, (sockaddr*)&addr, &len);
    if (ret != 0)
    {
        printf("getsockname Error!\n");
    }
    printf("servSock address = %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    while (true)
    {
        //向客户端发送数据
        char str[20] = "Hello World!";
        send(clntSock, str, strlen(str) + sizeof(char), NULL);
        Sleep(1000);
    }
    //关闭套接字
    closesocket(clntSock);
    closesocket(servSock);
}

void ServerSample::Func02()
{
    //创建套接字
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_HOPOPTS);
    //绑定套接字
    struct sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = AF_INET;  //使用IPv4地址
    sockAddr.sin_addr.s_addr = inet_addr("0.0.0.0");  //具体的IP地址
    sockAddr.sin_port = htons(51234);  //端口
    //sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);  //具体的IP地址
    //sockAddr.sin_port = htons(0);  //端口
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    //进入监听状态
    listen(servSock, 20);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    int len = sizeof(addr);
    int ret = getsockname(servSock, (sockaddr*)&addr, &len);
    if (ret != 0)
    {
        printf("getsockname Error!\n");
    }
    printf("server [%s:%d] wait for client connect...\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    fd_set fdSocket;
    FD_ZERO(&fdSocket);
    FD_SET(servSock, &fdSocket);//将servSock添加进该集合

    auto selectTime = tdxl::u2socket::CastDuration(std::chrono::seconds(1));
    //int iTimeOut = 3000;
    //::setsockopt(servSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&iTimeOut, sizeof(iTimeOut));

    while (true)
    {
        fd_set fdRead = fdSocket;
        //nfds：select监视的文件句柄数，视进程中打开的文件数而定。
        //readfds：select监视的可读文件句柄集合
        //writefds：select监视的可写文件句柄集合
        //exceptfds：select监视的异常文件句柄集合
        //timeout：本次select()超时结束时间, 可精确至百万分之一秒，为NULL时阻塞
        int nRet = select(NULL, &fdRead, NULL, NULL, &selectTime);
        if (nRet <= 0)
            continue;

        for (int i = 0; i < (int)fdSocket.fd_count; ++i)
        {
            if (FD_ISSET(fdSocket.fd_array[i], &fdRead))
            {
                if (fdSocket.fd_array[i] == servSock)
                {
                    sockaddr_in addrRemote;
                    int nAddrLen = sizeof(addrRemote);
                    SOCKET sNew = ::accept(servSock, (sockaddr*)&addrRemote, &nAddrLen);
                    FD_SET(sNew, &fdSocket);
                    printf("client [%s:%d] connected\n", inet_ntoa(addrRemote.sin_addr), ntohs(addrRemote.sin_port));
                }
                else
                {
                    //uint8_t data;
                    //int result = recv(fdSocket.fd_array[i], (char*)&data, 1, MSG_PEEK);
                    //if (result <= 0)
                    //{
                    //    auto err = WSAGetLastError(); //SOCKET_ERROR
                    //    printf("error code = %d\n", err);
                    //}
                    char buffer[1024];
                    memset(buffer, 0, 1024);
                    int nRecev = recv(fdSocket.fd_array[i], buffer, 1024, 0);
                    struct sockaddr_in addr;
                    memset(&addr, 0, sizeof(addr));
                    int len = sizeof(addr);
                    int ret = getsockname(fdSocket.fd_array[i], (sockaddr*)&addr, &len);
                    if (ret != 0)
                    {
                        printf("getsockname error!\n");
                    }
                    if (nRecev > 0)
                    {
                        std::string timestring = Zeus::GetNowString();
                        printf("[%s]received client [%s:%d] msg:%s\n", timestring.data(), inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), buffer);
                        //send(fdSocket.fd_array[i], buffer, strlen(buffer), 0);
                        //char *str = "Hello World!";
                        //send(fdSocket.fd_array[i], str, strlen(str) + sizeof(char), NULL);
                    }
                    else
                    {
                        struct sockaddr_in addr;
                        memset(&addr, 0, sizeof(addr));
                        int len = sizeof(addr);
                        int ret = getsockname(fdSocket.fd_array[i], (sockaddr*)&addr, &len);
                        if (ret != 0)
                        {
                            printf("getsockname Error!\n");
                        }
                        printf("client [%s:%d] disconnected\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                        closesocket(fdSocket.fd_array[i]);
                        FD_CLR(fdSocket.fd_array[i], &fdSocket);
                    }
                }
            }
        }
    }

    closesocket(servSock);
}

void ServerSample::Udp01()
{
    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
    //绑定套接字
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = AF_INET;  //使用IPv4地址
    servAddr.sin_addr.s_addr = inet_addr("172.20.91.45");  //具体的IP地址
    //servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
    servAddr.sin_port = htons(51234);  //端口
    bind(sock, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));
    //接收客户端请求
    SOCKADDR clntAddr;  //客户端地址信息
    int nSize = sizeof(SOCKADDR);
    char buffer[BUF_SIZE];  //缓冲区
    while (true)
    {
        memset(&buffer, 0, sizeof(buffer));
        int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &clntAddr, &nSize);
        printf("Message form client: %s\n", buffer);
        //sendto(sock, buffer, strLen, 0, &clntAddr, nSize);
    }
    closesocket(sock);
}

#include <vector>
inline int RTPSelect(const SOCKET *sockets, int8_t *readflags, size_t numsocks)
{
    using namespace std;

    std::vector<struct pollfd> fds(numsocks);

    for (size_t i = 0; i < numsocks; i++)
    {
        fds[i].fd = sockets[i];
        fds[i].events = POLLIN;
        fds[i].revents = 0;
        readflags[i] = 0;
    }

    int timeoutmsec = 2310;
    int status = WSAPoll(&(fds[0]), (ULONG)numsocks, timeoutmsec);
    if (status > 0)
    {
        for (size_t i = 0; i < numsocks; i++)
        {
            if (fds[i].revents)
                readflags[i] = 1;
        }
    }
    return status;
}

#include <chrono>
#include <iostream>
#define C1000000 1000000ui64
#define CEPOCH 11644473600000000ui64
uint32_t GetMicroSeconds(double m_t)
{
    uint32_t microsec;

    if (m_t >= 0)
    {
        int64_t sec = (int64_t)m_t;
        microsec = (uint32_t)(1e6*(m_t - (double)sec) + 0.5);
    }
    else // m_t < 0
    {
        int64_t sec = (int64_t)(-m_t);
        microsec = (uint32_t)(1e6*((-m_t) - (double)sec) + 0.5);
    }

    if (microsec >= 1000000)
        return 999999;
    // Unsigned, it can never be less than 0
    // if (microsec < 0)
    // 	return 0;
    return microsec;
}

uint64_t CalculateMicroseconds(uint64_t performancecount, uint64_t performancefrequency)
{
    uint64_t f = performancefrequency;
    uint64_t a = performancecount;
    uint64_t b = a / f;
    uint64_t c = a%f; // a = b*f+c => (a*1000000)/f = b*1000000+(c*1000000)/f

    return b*C1000000 + (c*C1000000) / f;
}
double CurrentTime()
{
    static int inited = 0;
    static uint64_t microseconds, initmicroseconds;
    static LARGE_INTEGER performancefrequency;

    uint64_t emulate_microseconds, microdiff;
    SYSTEMTIME systemtime;
    FILETIME filetime;

    LARGE_INTEGER performancecount;

    QueryPerformanceCounter(&performancecount);

    if (!inited) {
        inited = 1;
        QueryPerformanceFrequency(&performancefrequency);
        GetSystemTime(&systemtime);
        SystemTimeToFileTime(&systemtime, &filetime);
        microseconds = (((uint64_t)(filetime.dwHighDateTime) << 32) + (uint64_t)(filetime.dwLowDateTime)) / (uint64_t)10;
        microseconds -= CEPOCH; // EPOCH
        initmicroseconds = CalculateMicroseconds(performancecount.QuadPart, performancefrequency.QuadPart);
    }

    emulate_microseconds = CalculateMicroseconds(performancecount.QuadPart, performancefrequency.QuadPart);

    microdiff = emulate_microseconds - initmicroseconds;

    double t = 1e-6*(double)(microseconds + microdiff);
    return t;
}

void printNowTime1()
{
    std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now(); // 获取当前时间点
    std::chrono::system_clock::duration duration_since_epoch = time_point_now.time_since_epoch(); // 从1970-01-01 00:00:00到当前时间点的时长
    time_t microseconds_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(duration_since_epoch).count(); // 将时长转换为微秒数
                                                                                                                           //std::cout << "time=" << microseconds_since_epoch << std::endl;

    std::cout << "time=" << GetMicroSeconds(CurrentTime()) << std::endl;
}


void ServerSample::Udp02()
{
    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
    //绑定套接字
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = AF_INET;  //使用IPv4地址
    servAddr.sin_addr.s_addr = inet_addr("172.20.91.33");  //具体的IP地址
    //servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
    servAddr.sin_port = htons(5004);  //端口

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
    int size = 50;
    bind(sock, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));
    //setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char *)&size, sizeof(int));
    //setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(int));
    //int ttl2 = 1;
    //auto status1 = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&ttl2, sizeof(int));
    //接收客户端请求
    SOCKADDR clntAddr;  //客户端地址信息
    int nSize = sizeof(SOCKADDR);
    char buffer[1550] = {'\0'};  //缓冲区
    unsigned long len;
    bool dataavailable = false;
    int offset = 0;
    do {
        len = 0;
        ioctlsocket(sock, FIONREAD, &len);
        if (len <= 0) // make sure a packet of length zero is not queued
        {
            int8_t isset = 0;
            int status = RTPSelect(&sock, &isset, 1);
            if (status < 0)
                return;

            if (isset)
                dataavailable = true;
            else
                dataavailable = false;
        }
        else
            dataavailable = true;

        if (dataavailable)
        {
            memset(&buffer, 0, sizeof(buffer));
            int strLen = recvfrom(sock, buffer, 1550, 0, &clntAddr, &nSize);
            if (strLen > offset)
            {
                //printf("Message form client: %s\n", buffer + 12);
                std::cout << "Message form client : " << std::string(buffer + offset, strLen - offset) << "\n";
                printNowTime1();
            }
            //sendto(sock, buffer, strLen, 0, &clntAddr, nSize);
        }
    } while (true);

    closesocket(sock);
}

void ServerSample::Unicast()
{
}

void ServerSample::Multicast()
{
    std::cout << "input multicast:[addr, port]" << std::endl;
    std::string addr;
    uint16_t port;
    std::cin >> addr >> port;
    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
    //绑定套接字
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = AF_INET;  //使用IPv4地址
    //servAddr.sin_addr.s_addr = inet_addr("172.20.91.33");  //具体的IP地址
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
    servAddr.sin_port = htons(port);  //端口

    int nOptval = 1;
    //用于端口接收单播、组播、广播
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&nOptval, sizeof(nOptval));
    if (ret != 0)
    {
        std::cout << "setsockopt fail: " << WSAGetLastError() << std::endl;
        return;
    }

    ret = bind(sock, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));
    if (ret != 0)
    {
        std::cout << "bind fail: " << WSAGetLastError() << std::endl;
        return;
    }

    int size = 32768;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char *)&size, sizeof(int)) != 0)
    {

    }
    if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(int)) != 0)
    {

    }
    unsigned char buffer1[8192];
    DWORD outputsize;
    DWORD numaddresses, i;
    SOCKET_ADDRESS_LIST *addrlist;

    if (WSAIoctl(sock, SIO_ADDRESS_LIST_QUERY, NULL, 0, &buffer1, 8192, &outputsize, NULL, NULL))
        return ;

    int ttl2 = 1, status;
    status = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&ttl2, sizeof(int));



    ip_mreq multiCast;
    multiCast.imr_multiaddr.S_un.S_addr = inet_addr(addr.c_str());
    multiCast.imr_interface.S_un.S_addr = htonl(INADDR_ANY);
    setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&multiCast, sizeof(multiCast));

    SOCKADDR clntAddr;  //客户端地址信息
    int nSize = sizeof(SOCKADDR);
    char buffer[BUFSIZ] = { '\0' };  //缓冲区
    unsigned long len;
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int ret = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)&clntAddr, &nSize);
        if (ret >= 0)
        {
            std::cout << "recvfrom : " << std::string(buffer) << "\n";
        }
    }
    closesocket(sock);
}

void ServerSample::Broadcast()
{
}
