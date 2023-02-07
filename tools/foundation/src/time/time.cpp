#include "foundation/time/time.h"
#include <chrono>

#pragma warning(disable:4800)

namespace Zeus
{
time_t GetNow()
{
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

std::string GetNowString()
{
    auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    time_t t = std::chrono::system_clock::to_time_t(now);
    int milliSecond = now.time_since_epoch().count() % 1000;
    auto localTm = gmtime(&t);
    char buff[32];
    int len = strftime(buff, 32, "%Y%m%d-%H:%M:%S", localTm);
    sprintf(buff + len, ".%03u", milliSecond);
    return std::string(buff);
}

}
