#pragma once

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#define FD_SETSIZE 1024
#include <WinSock2.h>
#include <Ws2tcpip.h>

namespace tdxl
{
namespace u2socket
{
typedef SOCKET SocketFD;
const SocketFD UNINIT_SOCKET = INVALID_SOCKET;
}// namespace u2socket
}// namespace tdxl
#else
#include <arpa/inet.h>
#include <netinet/in.h>
namespace tdxl
{
namespace u2socket
{
typedef int SocketFD;
const SocketFD UNINIT_SOCKET = -1;
}// namespace u2socket
}// namespace tdxl
#endif
