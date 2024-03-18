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
    int     err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        std::cout << "WSAStartup failed with error: " << err << std::endl;
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

timeval CastDuration(const std::chrono::microseconds& duration)
{
    timeval time;
    if (duration >= std::chrono::seconds(1))
    {
        auto second  = std::chrono::duration_cast<std::chrono::seconds>(duration);
        time.tv_sec  = (long) second.count();
        time.tv_usec = (long) ((duration - second).count());
    }
    else
    {
        time.tv_sec  = 0;
        time.tv_usec = (long) duration.count();
    }
    return time;
}

bool GetSocketAddr(SOCKET socket, bool bLocal, std::string& addr, uint16_t& port)
{
    //getsockname函数用于获取与某个套接字关联的本地协议地址
    //getpeername函数用于获取与某个套接字关联的外地协议地址
    sockaddr_in socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr)); //对象初始化，每个字节都用0填充
    int addrLen = sizeof(socketAddr);
    if (bLocal)
    {
        if (SOCKET_ERROR == getsockname(socket, (sockaddr*) &socketAddr, &addrLen))
        {
            std::cout << "getsockname failed, error=" << WSAGetLastError() << std::endl;
            return false;
        }
    }
    else
    {
        if (SOCKET_ERROR == getpeername(socket, (sockaddr*) &socketAddr, &addrLen))
        {
            std::cout << "getpeername failed, error=" << WSAGetLastError() << std::endl;
            return false;
        }
    }
    addr = inet_ntoa(socketAddr.sin_addr);
    port = ntohs(socketAddr.sin_port);
    return true;
}

} // namespace u2socket
} // namespace tdxl