#include "mem.h"

#include <Windows.h>

#include <cstring>

#include "base/byte.h"

namespace {
static_assert(utils::kMemProtectionRWE == PAGE_EXECUTE_READWRITE);
static_assert(sizeof(utils::MemProtection) == sizeof(DWORD));
constexpr byte kopnop = 0x90;
constexpr byte kopjmp = 0xE9;
constexpr byte kopcall = 0xE8;
}  // namespace

bool utils::ChangeMemProtection(byte* begin, unsigned length,
                                MemProtection protection_new, MemProtection* protection_old) {
    return VirtualProtect(begin, length, protection_new, protection_old);
}

void utils::FillNop(byte* begin, unsigned length) {
    std::memset(begin, kopnop, length);
}

byte* utils::GetCallJmpDest(byte* p, unsigned length) {
    switch (length) {
    case 5: case 6:
        return p + length + *(int*)(p + length - 4);
    case 2:
        return p + length + (int)*(char*)(p + length - 1);
    default:
        return nullptr;
    }
}

void utils::FillWithJmp(byte* p, void* dest) {
    int dis = (byte*)dest - p - 5;
    *p = kopjmp;
    *(int*)(p + 1) = dis;
}

void utils::FillWithCall(byte* p, void* dest) {
    int dis = (byte*)dest - p - 5;
    *p = kopcall;
    *(int*)(p + 1) = dis;
}
