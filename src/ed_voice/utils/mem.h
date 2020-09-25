#ifndef __UTILS_MEM_H__
#define __UTILS_MEM_H__

#include "base/byte.h"

namespace utils {

using MemProtection = unsigned long;
constexpr MemProtection kMemProtectionRWE = 0x40;

bool ChangeMemProtection(byte* begin, unsigned length,
                         MemProtection protection_new, MemProtection* protection_old);

void FillNop(byte* p, unsigned length);

byte* GetCallJmpDest(byte* p, unsigned length);

void FillWithJmp(byte* p, void* dest);

void FillWithCall(byte* p, void* dest);

}  // namespace utils

#endif  // __UTILS_MEM_H__
