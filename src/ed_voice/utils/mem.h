#ifndef __UTILS_MEM_H__
#define __UTILS_MEM_H__

#include <stdint.h>

namespace utils {

using MemProtection = unsigned long;
constexpr MemProtection kMemProtectionRWE = 0x40;

bool ChangeMemProtection(uint8_t* begin, unsigned length,
                         MemProtection protection_new, MemProtection* protection_old);

void FillNop(uint8_t* p, unsigned length);

uint8_t* GetCallJmpDest(uint8_t* p, unsigned length);

void FillWithJmp(uint8_t* p, void* dest);

void FillWithCall(uint8_t* p, void* dest);

}  // namespace utils

#endif  // __UTILS_MEM_H__
