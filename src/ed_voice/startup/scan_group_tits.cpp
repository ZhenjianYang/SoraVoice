#include "startup/scan_group_common.h"

#include <Windows.h>

#include "asm/asm.h"
#include "global/global.h"
#include "utils/log.h"
#include "utils/section_info.h"

namespace {
constexpr byte opjmp = 0xE9;
constexpr byte opcall = 0xE8;
constexpr byte opnop = 0x90;

DEFINE_GROUP_BEGIN(Tits)

DEFINE_PIECE_BEGIN(Tits, Hwnd, ".text", PatternType::Bytes,
        "68 ?? ?? ?? 00 "
        "68 00 00 04 00 "
        "FF ?? ?? ?? ?? 00 "
        "A3 ?? ?? ?? 00 "
        "85 C0")
DEFINE_ADDITIONAL_MATCH(begin, end) {
    LOG("%s:%s matched at 0x%08X, start addtional check...",
        Group->Name().c_str(), Name.c_str(), (unsigned)(begin));
    byte* addr = *(byte**)(begin + 1);
    constexpr char Falcom[] = "46 00 61 00 6c 00 63 00 6f 00 6d 00";
    constexpr std::size_t length = sizeof(Falcom) / 3;
    if (!Group->InSection(".rdata", addr, length)) {
        return false;
    }
    bool add = MemMatch::BytesMatcher(Falcom).Match(addr, length);
    if (add) {
        LOG("%s:%s addtional check passed!", Group->Name().c_str(), Name.c_str());
    } else {
        LOG("%s:%s addtional check failed!", Group->Name().c_str(), Name.c_str());
    }
    return add;
}
DEFINE_APPLY() {
    const auto& results = GetResults();
    void** addr = *(void***)(results.front() + 17);
    global.addrs.pHwnd = addr;
}
DEFINE_PIECE_END(Hwnd)

DEFINE_PIECE_BEGIN(Tits, Text, ".text", PatternType::Bytes,
    "80 F9 23 0F 85 ?? ?? ?? ??")
    DEFINE_ADDITIONAL_MATCH(begin, end) {
    LOG("%s:%s matched at 0x%08X, start addtional check...",
        Group->Name().c_str(), Name.c_str(), (unsigned)(begin));
    int jmp = *(int*)(begin + 5);
    byte* dst = begin + 9 + jmp;
    bool add = Group->InSection(".text", dst, 1);
    if (add) {
        LOG("%s:%s addtional check passed!", Group->Name().c_str(), Name.c_str());
    } else {
        LOG("%s:%s addtional check failed!", Group->Name().c_str(), Name.c_str());
    }
    return add;
}
DEFINE_APPLY() {
    const auto& results = GetResults();
    byte* p = results.front() + 3;
    int jmp = *(int*)(p + 2);
    byte* dst = p + 6 + jmp;
    global.addrs.text_jmp = dst;
    global.addrs.text_next = p + 6;

    char buff[6];
    buff[0] = opjmp;
    buff[5] = opnop;
    jmp = (byte*)asm_::asm_text - (p + 5);
    *(int*)(buff + 1) = jmp;

    DWORD dwProtect, dwProtect2;
    if (VirtualProtect(p, sizeof(buff), PAGE_EXECUTE_READWRITE, &dwProtect)) {
        std::memcpy(p, buff, sizeof(buff));
        VirtualProtect(p, sizeof(buff), dwProtect, &dwProtect2);
    }
}
DEFINE_PIECE_END(Text)

ADD_PIECES_BEGIN()
ADD_PIECE(Hwnd)
ADD_PIECE(Text)
ADD_PIECES_END()

DEFINE_GROUP_END()
}  // namespace

namespace startup {
DEFINE_STATIC_GET_GROUP(Tits);
}  // namespace startup
