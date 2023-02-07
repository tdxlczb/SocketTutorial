#pragma once
#include <string>

namespace Zeus
{
uint8_t RandUint8();
uint8_t RandUint8(uint8_t max);
uint8_t RandUint8(uint8_t min, uint8_t max);

uint16_t RandUint16();
uint16_t RandUint16(uint16_t max);
uint16_t RandUint16(uint16_t min, uint16_t max);

uint32_t RandUint32();
uint32_t RandUint32(uint32_t max);
uint32_t RandUint32(uint32_t min, uint32_t max);

uint64_t RandUint64();
uint64_t RandUint64(uint64_t max);
uint64_t RandUint64(uint64_t min, uint64_t max);

float RandFloat();
float RandFloat(float max);
float RandFloat(float min, float max);

double RandDouble();
double RandDouble(double max);
double RandDouble(double min, double max);

bool RandBool();

char RandLetter();
char RandLowerLetter();
char RandUpperLetter();
char RandNumberLetter();
char RandHex();
void RandBytes(void* output, size_t size);
std::string RandWord();
std::string RandString(size_t size);
}
