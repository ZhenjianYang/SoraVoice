#include "mem.h"

#include <Windows.h>

#include <cstring>

namespace {
static_assert(utils::kMemProtectionRWE == PAGE_EXECUTE_READWRITE);
static_assert(sizeof(utils::MemProtection) == sizeof(DWORD));
constexpr uint8_t kopnop = 0x90;
constexpr uint8_t kopjmp = 0xE9;
constexpr uint8_t kopcall = 0xE8;
}  // namespace

bool utils::ChangeMemProtection(uint8_t* begin, unsigned length,
                                MemProtection protection_new, MemProtection* protection_old) {
    return VirtualProtect(begin, length, protection_new, protection_old);
}

void utils::FillNop(uint8_t* begin, unsigned length) {
    std::memset(begin, kopnop, length);
}

uint8_t* utils::GetCallJmpDest(uint8_t* p, unsigned length) {
    switch (length) {
    case 5: case 6:
        return p + length + *(int*)(p + length - 4);
    case 2:
        return p + length + (int)*(char*)(p + length - 1);
    default:
        return nullptr;
    }
}

void utils::FillWithJmp(uint8_t* p, void* dest) {
    int dis = (uint8_t*)dest - p - 5;
    *p = kopjmp;
    *(int*)(p + 1) = dis;
}

void utils::FillWithCall(uint8_t* p, void* dest) {
    int dis = (uint8_t*)dest - p - 5;
    *p = kopcall;
    *(int*)(p + 1) = dis;
}
