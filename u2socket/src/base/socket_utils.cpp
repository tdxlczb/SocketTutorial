#include "u2socket/base/socket_utils.h"
#include <iostream>
#include "u2socket/base/socket_define.h"

namespace tdxl
{
namespace u2socket
{

void InitSocket()
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

void FinishSocket()
{
#ifdef WIN32
    WSACleanup();
#endif // WIN32
}

timeval CastDuration(const std::chrono::microseconds & duration)
{
    timeval time;
    if (duration >= std::chrono::seconds(1))
    {
        auto second = std::chrono::duration_cast<std::chrono::seconds>(duration);
        time.tv_sec = second.count();
        time.tv_usec = (duration - second).count();
    }
    else
    {
        time.tv_sec = 0;
        time.tv_usec = duration.count();
    }
    return time;
}

}// namespace u2socket
}// namespace tdxl