#ifndef INC_UTILS_UTILS_H_
#define INC_UTILS_UTILS_H_

// -----------------------------------------
#include "murmur3/murmur3.h"
#define E_ROUND(f) ceilf(f * 100) / 100

uint32_t getID(const char* txt);
uint32_t getID(const void* buff, size_t nbytes);

// -----------------------------------------

//#ifdef _DEBUG
void dbg(const char* fmt, ...);
//#else
//#define dbg(...)
//#endif

bool fatal(const char* fmt, ...);

bool isPressed(int key);
bool isPressedWithOrWithoutFocus(int key);

bool fileExists(const std::string& afilename);
json loadJson(const std::string& filename);
bool fileExists(const std::string& afilename);

template <typename T>
T Clamp(const T& n, const T& lower, const T& upper) {
	return std::max(lower, std::min(n, upper));
}

void ToUpperCase(std::string&);
float urand(float loVal, float hiVal);
int Strnicmp(const char* str1, const char* str2, int n);
const char *convert(const std::string & s);

#endif

