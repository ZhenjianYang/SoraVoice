#include "startup/scan_group_common.h"

#include <Windows.h>

#include "asm/asm.h"
#include "global/global.h"
#include "utils/log.h"
#include "utils/mem.h"
#include "utils/section_info.h"

namespace {

DEFINE_GROUP_BEGIN(Tits)

DEFINE_PIECE_BEGIN(Tits, Hwnd, ".text", PatternType::Bytes,
                   "68 ?? ?? ?? 00 "
                   "68 00 00 04 00 "
                   "FF ?? ?? ?? ?? 00 "
                   "A3 ?? ?? ?? 00 "
                   "85 C0")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = REF_STRING(".text", b + 1, ".rdata", L"Falcom");
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    void** addr = *(void***)(results.front() + 17);
    global.addrs.pHwnd = addr;
    LOG("pHwnd = 0x%08X", (unsigned)global.addrs.pHwnd);
DEFINE_APPLY_END(true)
DEFINE_PIECE_END(Hwnd)

DEFINE_PIECE_BEGIN(Tits, Text, ".text", PatternType::Bytes,
                   "80 F9 23 0F 85 ?? ?? ?? ??")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    byte* dst = utils::GetCallJmpDest(b + 3, 6);
    bool rst = Group->InSection(".text", dst, 1);
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front() + 3;
    bool rst = Group->RedirectWithJmp(
            p, 6, asm_::text, &global.addrs.text_next, &global.addrs.text_jmp);
    LOG("text_next = 0x%08X", (unsigned)global.addrs.text_next);
    LOG("text_jmp = 0x%08X", (unsigned)global.addrs.text_jmp);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Text)

DEFINE_PIECE_BEGIN(Tits, Ldat, ".text", PatternType::Bytes,
                   "81 EC 84 02 00 00")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = REF_STRING(".text", b + 0x1A, ".rdata", L"ED6_DT%02x.DAT") && *(b + 0x19) == 0x68;
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front();
    bool rst = Group->BackupCode(p, 6, asm_::ldat, &global.addrs.ldat_next);
    LOG("ldat_next = 0x%08X", (unsigned)global.addrs.ldat_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Ldat)

DEFINE_PIECE_BEGIN(Tits, Dcdat, ".text", PatternType::Bytes,
                   "83 EC 18 8B 03")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = true;
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front();
    bool rst = Group->BackupCode(p, 5, asm_::dcdat, &global.addrs.dcdat_next);
    LOG("dcdat_next = 0x%08X", (unsigned)global.addrs.dcdat_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Dcdat)

DEFINE_PIECE_BEGIN(Tits, Pdirs, ".text", PatternType::Bytes,
                   "C1 E9 10 8B 3C 8D ?? ?? ?? 00")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = Group->InSection(".data", *(byte**)(b + 6), sizeof(byte*) * 0x20)
        && REF_STRING(".data", *(byte**)(b + 6) + 6 * sizeof(byte*), "", "CH20000 ._CH")
        && REF_STRING(".data", *(byte**)(b + 6) + 7 * sizeof(byte*), "", "CH00000 ._CH")
        && REF_STRING(".data", *(byte**)(b + 6) + 9 * sizeof(byte*), "", "CH10000 ._CH");
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front();
    global.addrs.pdirs = *(byte**)(p + 6);
    LOG("pdirs = 0x%08X", (unsigned)global.addrs.pdirs);
    bool rst = true;
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Ldat)

DEFINE_PIECE_BEGIN(Tits, Textse, ".text", PatternType::Bytes,
                   "FF D2 " "80 3D ?? ?? ?? ?? 00 " "75 11")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = Group->InSection(".data", *(byte**)(b + 4), sizeof(byte*))
               && Group->InSection(".text", b + 0x2B, sizeof(byte*))
               && *(b + 0x2B) == 0xC3;
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front();
    global.addrs.textse_jmp = utils::GetCallJmpDest(p + 9, 2);
    bool rst = Group->BackupCode(p + 11, 6, asm_::textse, &global.addrs.textse_next);
    LOG("textse_jmp = 0x%08X", (unsigned)global.addrs.textse_jmp);
    LOG("textse_next = 0x%08X", (unsigned)global.addrs.textse_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Textse)

DEFINE_PIECE_BEGIN(Tits, Dlgse, ".text", PatternType::Bytes,
                   "FF D0 " "80 3D ?? ?? ?? ?? 00 " "75 11")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = Group->InSection(".data", *(byte**)(b + 4), sizeof(byte*));
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_APPLY_BEGIN()
const auto& results = GetResults();
byte* p = results.front();
global.addrs.dlgse_jmp = utils::GetCallJmpDest(p + 9, 2);
bool rst = Group->BackupCode(p + 11, 6, asm_::dlgse, &global.addrs.dlgse_next);
LOG("dlgse_jmp = 0x%08X", (unsigned)global.addrs.dlgse_jmp);
LOG("dlgse_next = 0x%08X", (unsigned)global.addrs.dlgse_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Dlgse)

ADD_PIECES_BEGIN()
ADD_PIECE(Hwnd)
ADD_PIECE(Text)
ADD_PIECE(Ldat)
ADD_PIECE(Dcdat)
ADD_PIECE(Pdirs)
ADD_PIECE(Textse)
ADD_PIECE(Dlgse)
ADD_PIECES_END()

DEFINE_GROUP_END()
}  // namespace

namespace startup {
DEFINE_STATIC_GET_GROUP(Tits);
}  // namespace startup
