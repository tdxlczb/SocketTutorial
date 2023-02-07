#pragma once
#include <chrono>
#include <ctime>

struct timeval;

namespace tdxl
{
namespace u2socket
{

void InitSocket();

void FinishSocket();

timeval CastDuration(const std::chrono::microseconds& duration);

}// namespace u2socket
}// namespace tdxl
