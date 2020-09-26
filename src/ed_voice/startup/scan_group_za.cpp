#include "startup/scan_group_common.h"

#include <map>

#include "asm/asm.h"
#include "global/global.h"
#include "startup/string_patch.h"
#include "utils/log.h"
#include "utils/mem.h"
#include "utils/section_info.h"

namespace {

DEFINE_GROUP_BEGIN(Za)

DEFINE_PIECE_BEGIN(Za, Hwnd, ".text", PatternType::Bytes,
                   "50 "
                   "8B 4D F4 "
                   "8B 91 ?? ?? 00 00 "
                   "52 "
                   "B9 ?? ?? ?? 00 "
                   "E8 ?? ?? ?? FF "
                   "6A 20")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = Group->InSection(".data", *(byte**)(b + 12), 4);
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    void** addr = *(void***)(results.front() + 12);
    global.addrs.pHwnd = addr + 1;
    LOG("pHwnd = 0x%08X", (unsigned)global.addrs.pHwnd);
DEFINE_APPLY_END(true)
DEFINE_PIECE_END(Hwnd)

DEFINE_PIECE_BEGIN(Za, Text, ".text", PatternType::Bytes,
                   "83 F9 23 "
                   "0F 85 ?? ?? 00 00 "
                   "8B 45 08 "
                   "83 C0 01")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    byte* dst = utils::GetCallJmpDest(b + 3, 6);
    bool rst = Group->InSection(".text", dst, 1);
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front() + 3;
    bool rst = Group->RedirectWithJmp(
            p, 6, asm_za::text, &global.addrs.text_next, &global.addrs.text_jmp);
    LOG("Apply at 0x%08X", (unsigned)p);
    LOG("text_next = 0x%08X", (unsigned)global.addrs.text_next);
    LOG("text_jmp = 0x%08X", (unsigned)global.addrs.text_jmp);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Text)

DEFINE_PIECE_BEGIN(Za, Scnp, ".text", PatternType::Bytes,
                   "68 ?? ?? ?? ?? "
                   "8B 55 08 "
                   "52 "
                   "E8 ?? ?? ?? ??")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    byte* dst = utils::GetCallJmpDest(b + 9, 5);
    bool rst = Group->InSection(".text", dst, 1)
               && REF_STRING(".text", b + 1, ".rdata", "%s/%s.bin");
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front() + 9;
    bool rst = Group->RedirectWithCall(p, 5, asm_za::scnp,
                                       nullptr, &global.addrs.scnp_jmp);
    LOG("Apply at 0x%08X", (unsigned)p);
    LOG("scnp_jmp = 0x%08X", (unsigned)global.addrs.scnp_jmp);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Scnp)

DEFINE_PIECE_BEGIN(Za, Textse, ".text", PatternType::Bytes,
                   "6A 05 "
                   "8B ?? ?? "
                   "?? ?? ?? "
                   "81 C1 ?? ?? ?? ?? "
                   "E8 ?? ?? ?? ?? "
                   "8B 45 F8")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    byte* dst = utils::GetCallJmpDest(b + 14, 5);
    bool rst = Group->InSection(".text", dst, 1);
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* t = results.front() + 3;
    global.info.game = *t == 0x0D ? GameZero : GameAo;
    byte* p = results.front() + 14;
    bool rst = Group->RedirectWithCall(p, 5, asm_za::textse,
                                       nullptr, &global.addrs.textse_jmp);
    LOG("Apply at 0x%08X", (unsigned)p);
    LOG("textse_jmp = 0x%08X", (unsigned)global.addrs.textse_jmp);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Textse)

DEFINE_PIECE_BEGIN(Za, Dlgse, ".text", PatternType::Bytes,
                   "6A 00 "
                   "8B ?? ?? "
                   "?? ?? ?? "
                   "81 C1 ?? ?? ?? ?? "
                   "E8 ?? ?? ?? ?? "
                   "83 7D EC 00")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    byte* dst = utils::GetCallJmpDest(b + 14, 5);
    bool rst = Group->InSection(".text", dst, 1);
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front() + 14;
    bool rst = Group->RedirectWithCall(p, 5, asm_za::dlgse,
                                       nullptr, &global.addrs.dlgse_jmp);
    LOG("Apply at 0x%08X", (unsigned)p);
    LOG("dlgse_jmp = 0x%08X", (unsigned)global.addrs.dlgse_jmp);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Dlgse)

ADD_PIECES_BEGIN()
ADD_PIECE(Hwnd)
ADD_PIECE(Text)
ADD_PIECE(Scnp)
ADD_PIECE(Textse)
ADD_PIECE(Dlgse)
ADD_PIECES_END()

DEFINE_GROUP_END()
}  // namespace

namespace startup {
DEFINE_STATIC_GET_GROUP(Za);
}  // namespace startup
