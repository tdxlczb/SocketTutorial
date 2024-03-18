#pragma once
#include <chrono>
#include <ctime>
#include <string>
#include "socket_define.h"

struct timeval;

namespace tdxl
{
namespace u2socket
{

void InitSocket();

void FinishSocket();

timeval CastDuration(const std::chrono::microseconds& duration);

bool GetSocketAddr(SOCKET socket, bool bLocal, std::string& addr, uint16_t& port);

}// namespace u2socket
}// namespace tdxl
