#include "startup/scan_group_common.h"

#include "global/global.h"
#include "utils/section_info.h"

namespace {
DEFINE_GROUP_BEGIN(Tits)

DEFINE_PIECE_BEGIN(Tits, HWND, ".text", PatternType::Bytes,
        "68 ?? ?? ?? 00 "
        "68 00 00 04 00 "
        "FF ?? ?? ?? ?? 00 "
        "A3 ?? ?? ?? 00 "
        "85 C0")
DEFINE_ADDITIONAL_MATCH(begin, end) {
    byte* addr = *(byte**)(begin + 1);
    constexpr char Falcom[] = "46 00 61 00 6c 00 63 00 6f 00 6d 00";
    if (!group->InSections(".rdata", addr, 12)) {
        return false;
    }
    return MemMatch::BytesMatcher(Falcom).Match(addr, 12);
}
DEFINE_APPLY() {
    const auto& results = GetResults();
    void** addr = *(void***)(results.front() + 17);
    global.addrs.pHwnd = addr;
}
DEFINE_PIECE_END()

ADD_PIECES_BEGIN()
ADD_PIECE(HWND)
ADD_PIECES_END()

DEFINE_GROUP_END()
}  // namespace

namespace startup {
DEFINE_STATIC_GET_GROUP(Tits);
}  // namespace startup
