#include "client_sample.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include <u2socket/base/socket_define.h>
#include <u2socket/base/socket_utils.h>
#include "foundation\time\time.h"

#define BUF_SIZE 100

ClientSample::ClientSample()
{
    InitSocket();
}

ClientSample::~ClientSample()
{
    UninitSocket();
}

void ClientSample::InitSocket()
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

void ClientSample::UninitSocket()
{
#ifdef WIN32
    WSACleanup();
#endif // WIN32
}

struct MsgData
{
    std::unique_ptr<char[]> _data;
    size_t _dataCapcity = 0;
    size_t _writeOffset = 0;
};

void WriteConsume(MsgData& msgData, size_t byteCount)
{
    msgData._writeOffset += byteCount;
}

void WriteByte(MsgData& msgData, const uint8_t & data)
{
    std::memcpy(msgData._data.get() + msgData._writeOffset, &data, sizeof(uint8_t));
    msgData._writeOffset += sizeof(uint8_t);
}

void WriteHostInt(MsgData& msgData, const uint32_t & data)
{
    std::memcpy(msgData._data.get() + msgData._writeOffset, &data, sizeof(uint32_t));
    msgData._writeOffset += sizeof(uint32_t);
}

void WriteNetInt(MsgData& msgData, const uint32_t & data)
{
    uint32_t netData = htonl(data);
    WriteHostInt(msgData, netData);
}

void WriteHead(MsgData& msgData, uint8_t msgType, size_t length)
{
    WriteNetInt(msgData, length);
    WriteByte(msgData, msgType);
    WriteConsume(msgData, 1);
}

void WriteStr(MsgData& msgData, const void * content, size_t size)
{
    std::memcpy(msgData._data.get() + msgData._writeOffset, content, size);
    msgData._writeOffset += size;
}

void WriteMessage(MsgData &msg, uint8_t msgType, const std::string&message)
{
    int msgLen = message.size();
    //Reset(length + 8);
    {
        msg._data.reset(new char[msgLen + 8]);
        msg._dataCapcity = msgLen + 8;
    }
    //WriteBegin();
    {
        WriteByte(msg, 0xAB);
    }
    //WriteHead(msgType, length);
    {
        WriteHead(msg, msgType, msgLen);
    }
    //WriteBody(message, length);
    {
        WriteStr(msg, message.data(), msgLen);
    }
    //WriteEnd();
    {
        WriteByte(msg, 0xBA);
    }
}

void SendHeartbeat(SOCKET sock)
{
    std::string str("heartbeat");
    while (true)
    {
        MsgData msg;
        WriteMessage(msg, 1, str);
        send(sock, msg._data.get(), msg._writeOffset, NULL);
        Sleep(3000);
    }
}

void SendData(SOCKET sock)
{
    std::string str;
    while (true)
    {
        Sleep(500);
        std::vector<std::string> args;
        std::string arg;
        while (std::cin >> arg)
        {
            args.push_back(arg);
            if (std::cin.get() == '\n')
                break;
        }
        if (args.size() >= 1)
        {
            if (args[0] == "0")
            {
                str = u8R"({"msgId": "150","msgType": "micPagingConnect","msgData":{"deviceId":"abcd","deviceName":"寻呼麦2","mac":"macabcd","password":"MTIzNDU2","sipServer":"172.20.91.35","sipUser":"7009","sipDomain":"172.20.91.35","sipPwd":"7009"}})";
            }
            else if (args[0] == "1")
            {
                str = u8R"({"msgId": "1","msgType": "micPagingEdit","msgData":{"deviceId":"abcd","deviceName":"寻呼麦1","mac":"mac123","password":"123456"}})";
            }
            else if (args[0] == "2")
            {
                str = u8R"({"msgId": "2","msgType": "micPagingStartLive","msgData":{"deviceId":"abcd","liveId":"111","liveName":"广播开启","roomId":"BN3Y","outputSourceList":["Dante2E1-80a33a","Dante2E1-80a9de"]}})";
            }
            else if (args[0] == "3")
            {
                str = u8R"({"msgId": "3","msgType": "micPagingLiveControl","msgData":{"deviceId":"abcd","liveId":"111","action":0}})";
            }
            else if (args[0] == "4")
            {
                str = u8R"({"msgId": "4","msgType": "micPagingLiveControl","msgData":{"deviceId":"abcd","liveId":"111","action":1}})";
            }
            else if (args[0] == "5")
            {
                str = u8R"({"msgId": "5","msgType": "micPagingLiveControl","msgData":{"deviceId":"abcd","liveId":"111","action":2}})";
            }
            else if (args[0] == "6")
            {
                str = u8R"({"msgId": "6","msgType": "micPagingGetAudioList","msgData":{"deviceId":"abcd","liveId":"111","action":2}})";
            }
            else if (args[0] == "7")
            {
                str = u8R"({"msgId": "7","msgType": "micPagingGetSpeakerList","msgData":{"deviceId":"abcd","liveId":"111","action":2}})";
            }
            else if (args[0] == "8")
            {
                str = u8R"({"msgData":{"deviceId":"85d39464c5653aa3","liveId":"93ff5e893fac4bcb87ccbf5287de753d","liveName":"","outputSourceList":["Dante2E1-80a33a","Dante2E1-80a9de"],"roomId":""},"msgId":"2","msgType":"micPagingStartLive"})";
            }
            else if (args[0] == "9")
            {
                std::string str1("heartbeat");
                MsgData msg;
                WriteMessage(msg, 1, str1);
                send(sock, msg._data.get(), msg._writeOffset, NULL);
                continue;
            }
            MsgData msg;
            WriteMessage(msg, 2, str);
            send(sock, msg._data.get(), msg._writeOffset, NULL);
            Sleep(100);
        }
    }
}

void ClientSample::Func01()
{
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_HOPOPTS);

    struct sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("172.20.91.41");
    sockAddr.sin_port = htons(51234);
    //sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //sockAddr.sin_port = htons(0);

    connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    int len = sizeof(addr);
    int ret = getsockname(sock, (sockaddr*)&addr, &len);
    if (ret != 0)
    {
        printf("getsockname Error!\n");
    }
    printf("sock address = %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    auto error = GetLastError();

    //接收服务器传回的数据
    char szBuffer[MAXBYTE] = { 0 };
    char str[MAXBYTE] = { 0 };
    long num = 0;
    while (true)
    {
        num++;
        sprintf_s(str, "%ld", num);
        int size = send(sock, str, strlen(str) + sizeof(char), NULL);
        std::string timestring = Zeus::GetNowString();
        printf("[%s]send message to server: %s, size=%d\n", timestring.data(), str, size);
        Sleep(5000);

        //接收服务器传回的数据
        //char szBuffer[MAXBYTE] = { 0 };
        //recv(sock, szBuffer, MAXBYTE, NULL);
        //timestring = Zeus::GetNowString();
        ////输出接收到的数据
        //printf("[%s]receive message form server: %s\n", timestring.data(), szBuffer);
    }
    //关闭套接字
    closesocket(sock);
}

bool RunConnect(SOCKET & clientSock, sockaddr_in & sockAddr)
{
    int ret = connect(clientSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    //因为是非阻塞的，这个时候错误码应该是WSAEWOULDBLOCK，Linux下是EINPROGRESS
    int code = WSAGetLastError();
    if (ret < 0 && code != WSAEWOULDBLOCK && code == WSAEINTR) {
        printf("connect failed with error: %ld\n", code);
        //return;
    }
    while (true)
    {
        fd_set readFDSet = {}, writeFDSet = {}, exceptFDSet = {};
        FD_ZERO(&readFDSet);
        FD_ZERO(&writeFDSet);
        FD_ZERO(&exceptFDSet);
        FD_SET(clientSock, &writeFDSet);
        FD_SET(clientSock, &exceptFDSet);
        auto timeout = tdxl::u2socket::CastDuration(std::chrono::milliseconds(100));
        ret = select(clientSock + 1, &readFDSet, &writeFDSet, &exceptFDSet, &timeout);
        if (ret == 0) {
            printf("select timeout\n");
        }
        else if (ret < 0) {
            printf("select failed with error: %ld\n", WSAGetLastError());
        }
        else {
            printf("select get data\n");
            if (FD_ISSET(clientSock, &writeFDSet))
            {
                printf("select success2\n");
                return true;
            }
            return false;
        }
    }
}

void ClientSample::Func02()
{
    SOCKET clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_HOPOPTS);

    struct sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("172.20.91.35");
    sockAddr.sin_port = htons(51772);
    //sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //sockAddr.sin_port = htons(0);

    //socket设置为非阻塞 
    unsigned long on = 1;
    if (ioctlsocket(clientSock, FIONBIO, &on) < 0) {
        printf("ioctlsocket failed\n");
    }

    auto selectTime = tdxl::u2socket::CastDuration(std::chrono::seconds(1));
    while (true)
    {
        if (RunConnect(clientSock, sockAddr))
            break;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    int len = sizeof(addr);
    int ret = getsockname(clientSock, (sockaddr*)&addr, &len);
    if (ret != 0)
    {
        printf("getsockname Error!\n");
    }
    printf("sock address = %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    std::thread th0([clientSock]() {
        SendHeartbeat(clientSock);
    });
    std::thread th1([clientSock]() {
        SendData(clientSock);
    });
    std::thread th2([clientSock]() {
        char startBuffer[7] = { 0 };
        char endBuffer[1] = { 0 };
        char szBuffer[MAXBYTE] = { 0 };

        while (true)
        {
            memset(&startBuffer, 0, sizeof(startBuffer));
            memset(&endBuffer, 0, sizeof(endBuffer));
            int recvlen = recv(clientSock, startBuffer, sizeof(startBuffer), NULL);
            if (recvlen <= 0)
            {
                continue;
            }
            uint8_t type = 0;
            std::memcpy(&type, startBuffer + 5, 1);
            int host = 0;
            std::memcpy(&host, startBuffer + 1, 4);
            int size = ntohl(host);
            if (size <= 0)
            {
                continue;
            }
            char * buff = new char[size];
            memset(buff, 0, sizeof(buff));
            recvlen = recv(clientSock, buff, size, NULL);
            if (recvlen <= 0)
            {
                continue;
            }
            std::string timestring = Zeus::GetNowString();
            printf("[%s]Message %d form server: %s,size=%d\n", timestring.data(), type, buff, size);

            recvlen = recv(clientSock, endBuffer, sizeof(endBuffer), NULL);
            if (recvlen <= 0)
            {
                continue;
            }
            Sleep(100);
        }
    });
    th0.join();
    th1.join();
    th2.join();
    //关闭套接字
    closesocket(clientSock);
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

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

//定义时钟分辨率，以ms为单位
#define TIMER_ACCURACY		1
class TimerTask
{
public:
    TimerTask() {};

    void AddTimerTask(const std::string& name, const std::function<void()>& callback, const std::chrono::steady_clock::duration& duration, bool once, bool precise)
    {
        _callback = callback;

        QueryPerformanceFrequency(&nFreq);

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        TIMECAPS tc;
        if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
        {
            _status = 1;
            return;
        }
        //分辨率的值不能超出系统的取值范围
        _wAccuracy = min(max(tc.wPeriodMin, TIMER_ACCURACY), tc.wPeriodMax);

        //调用timeBeginPeriod函数设置定时器的分辨率 
        timeBeginPeriod(_wAccuracy);

        QueryPerformanceCounter(&t1);
        // 设定41毫秒定时器
        _mmTimerId = timeSetEvent(milliseconds, 0, TimerCallback, (DWORD)this, TIME_PERIODIC);
        if (_mmTimerId == 0)
        {
            _status = 2;
        }
        _status = 0;
    }

    static void CALLBACK TimerCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
    {
        TimerTask* self = (TimerTask*)dwUser;
        self->count++;
        //LARGE_INTEGER t2;
        //QueryPerformanceCounter(&t2);
        //double dt = (t2.QuadPart - self->t1.QuadPart) / (double)self->nFreq.QuadPart;
        //if (dt > 60.0)
        //{
        //    std::cout << "count=" << self->count << std::endl;
        //    self->count = 0;
        //    QueryPerformanceCounter(&self->t1);
        //}
        self->_callback();
    }
    ~TimerTask()
    {
        if (_mmTimerId == 0)
            return;

        timeKillEvent(_mmTimerId);
        timeEndPeriod(_wAccuracy);
    }
private:
    MMRESULT _mmTimerId = 0;
    UINT _wAccuracy = 0;
    int _status;
    std::function<void()> _callback;
    int count = 0;

    LARGE_INTEGER nFreq;
    LARGE_INTEGER t1;
    LARGE_INTEGER t2;
};
void ClientSample::Udp01()
{
    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
    //服务器地址信息
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("172.20.91.33");
    //servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(5004);

    //int size = 100;
    //bind(sock, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));
    //setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char *)&size, sizeof(int));
    //setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(int));
    //int ttl2 = 1;
    //auto status1 = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&ttl2, sizeof(int));

    //不断获取用户输入并发送给服务器，然后接受服务器数据
    struct sockaddr fromAddr;
    int addrLen = sizeof(fromAddr);
    char buffer[1440] = { 0 };
    memset(buffer, 1, 1440);
    TimerTask timerTask;
    int num = 0;
    timerTask.AddTimerTask("test", [&]()
    {
        num++;
        std::cout << num << std::endl;
        //printf("Input a string: ");
        //std::cin >> buffer;
        std::string str = std::to_string(num);
        sendto(sock, str.data(), str.length(), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
        //int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &fromAddr, &addrLen);
        //buffer[strLen] = 0;
        //printf("Message form server: %s\n", buffer);
        printNowTime1();
    }, std::chrono::milliseconds(1000), false, true);

    getchar();

    //int num = 0;
    //while (true)
    //{
    //    num++;
    //    memset(&buffer, 0, sizeof(buffer));
    //    std::cout << num << std::endl;
    //    //printf("Input a string: ");
    //    //std::cin >> buffer;
    //    std::string str = std::to_string(num);
    //    sendto(sock, buffer, 1452, 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
    //    //int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &fromAddr, &addrLen);
    //    //buffer[strLen] = 0;
    //    //printf("Message form server: %s\n", buffer);
    //    printNowTime1();
    //    Sleep(50);
    //}

    closesocket(sock);
}

void ClientSample::Unicast()
{
}

void ClientSample::Multicast()
{
    std::cout << "input multicast:[addr, port]" << std::endl;
    std::string addr;
    uint16_t port;
    std::cin >> addr >> port;
    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
    //服务器地址信息
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(addr.c_str());
    //servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    UCHAR uLoop = 1; // 0-同一台主机，1-跨主机
    int ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&uLoop, sizeof(uLoop));
    if (ret != 0)
    {
        std::cout << "setsockopt error: " << WSAGetLastError() << std::endl;
        return;
    }

    int num = 0;
    while (true)
    {
        num++;
        std::cout << num << std::endl;
        std::string str = std::to_string(num);
        sendto(sock, str.data(), str.length(), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
        Sleep(1000);
    }

    closesocket(sock);
}

void ClientSample::Broadcast()
{
}
