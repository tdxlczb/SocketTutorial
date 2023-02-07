#include "foundation/random/random.h"
#include <ctime>
#include <mutex>

#pragma warning(disable:4800)

namespace Zeus
{
static int Rand()
{
    thread_local  std::once_flag flag;
    std::call_once(flag, []()
    {
        std::srand(std::time(0));
    });
    return std::rand();
}

#define  AUTO_RAND_MAX(FUN) return FUN() % (max?max:max+1);

#define AUTO_RAND_MAXF(FUN) return 0;//std::fmod(FUN(),max)

#define  AUTO_RAND_RANGE(FUN) return min+FUN(max - min);

uint8_t RandUint8()
{
    return static_cast<uint8_t>(Rand());
}
uint8_t RandUint8(uint8_t max)
{
    AUTO_RAND_MAX(RandUint8);
}
uint8_t RandUint8(uint8_t min, uint8_t max)
{
    AUTO_RAND_RANGE(RandUint8);
}

uint16_t RandUint16()
{
    return static_cast<uint16_t>(Rand());
}
uint16_t RandUint16(uint16_t max)
{
    AUTO_RAND_MAX(RandUint16);
}
uint16_t RandUint16(uint16_t min, uint16_t max)
{
    AUTO_RAND_RANGE(RandUint16);
}

uint32_t RandUint32()
{
    return static_cast<uint32_t>(Rand());
}
uint32_t RandUint32(uint32_t max)
{
    AUTO_RAND_MAX(RandUint32);
}
uint32_t RandUint32(uint32_t min, uint32_t max)
{
    AUTO_RAND_RANGE(RandUint32);
}

uint64_t RandUint64()
{
    return static_cast<uint64_t>(Rand()) * static_cast<uint64_t>(Rand());
}
uint64_t RandUint64(uint64_t max)
{
    AUTO_RAND_MAX(RandUint64);
}
uint64_t RandUint64(uint64_t min, uint64_t max)
{
    AUTO_RAND_RANGE(RandUint64);
}

float RandFloat()
{
    return static_cast<float>(Rand() + (Rand() % 101) / 101);
}
float RandFloat(float max)
{
    AUTO_RAND_MAXF(RandFloat);
}
float RandFloat(float min, float max)
{
    AUTO_RAND_RANGE(RandFloat);
}

double RandDouble()
{
    return static_cast<double>(Rand() + (Rand() % 101) / 101);
}
double RandDouble(double max)
{
    AUTO_RAND_MAXF(RandDouble);
}
double RandDouble(double min, double max)
{
    AUTO_RAND_RANGE(RandDouble);
}

bool RandBool()
{
    return static_cast<bool>(Rand() % 2);
}

char RandLetter()
{
    static const char charset[] = { "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWZYZ" };
    return charset[Rand() % (sizeof(charset) - 1)];
}
char RandLowerLetter()
{
    static const char charset[] = { "abcdefghijklmnopqrstuvwxyz" };
    return charset[Rand() % (sizeof(charset) - 1)];
}
char RandUpperLetter()
{
    static const char charset[] = { "ABCDEFGHIJKLMNOPQRSTUVWZYZ" };
    return charset[Rand() % (sizeof(charset) - 1)];
}
char RandNumberLetter()
{
    static const char charset[] = { "0123456789" };
    return charset[Rand() % (sizeof(charset) - 1)];
}
char RandHex()
{
    static const char charset[] = { "0123456789ABCDEF" };
    return charset[Rand() % (sizeof(charset) - 1)];
}
void RandBytes(void* output, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        auto temp = Rand();
        *(reinterpret_cast<char*>(output) + i) = temp;
    }
}
std::string RandWord()
{
    auto size = RandUint8(5, 15);
    return RandString(size);
}
std::string RandString(size_t size)
{
    std::string stream;
    stream.reserve(size);
    for (size_t i = 0; i < size; i++)
    {
        stream.push_back(RandLetter());
    }
    return stream;
}

}
