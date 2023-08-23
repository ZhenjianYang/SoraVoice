#include "startup/scan_group_common.h"

#include <map>

#include "asm/asm.h"
#include "global/global.h"
#include "startup/string_patch.h"
#include "utils/log.h"
#include "utils/mem.h"
#include "utils/section_info.h"

namespace {

DEFINE_GROUP_BEGIN(Sora)

DEFINE_PIECE_BEGIN(Sora, Hwnd, ".text", PatternType::Bytes,
                   "8B ?? ?? ?? ?? ?? "
                   "8D 86 ?? ?? ?? ?? "
                   "50 "
                   "5? "
                   "FF 15 ?? ?? ?? ?? "
                   "8B ?? ?? ?? ?? ?? "
                   "8D 86 ?? ?? ?? ?? "
                   "50 "
                   "5? "
                   "FF 15 ?? ?? ?? ?? ")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    unsigned addr1 = *(unsigned*)(b + 2);
    unsigned addr2 = *(unsigned*)(b + 22);
    bool rst = addr1 == addr2;
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    void** addr = *(void***)(results.front() + 2);
    global.addrs.pHwnd = addr;
    global.info.game = GameSora;
    LOG("pHwnd = 0x%08X", (unsigned)global.addrs.pHwnd);
DEFINE_APPLY_END(true)
DEFINE_PIECE_END(Hwnd)

DEFINE_PIECE_BEGIN(Sora, Text, ".text", PatternType::Bytes,
                   "3C 23 0F 85 ?? ?? ?? ?? 42")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    uint8_t* dst = utils::GetCallJmpDest(b + 2, 6);
    bool rst = Group->InSection(".text", dst, 1);
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    uint8_t* p = results.front() + 2;
    bool rst = Group->RedirectWithJmp(
            p, 6, asm_sora::text, &global.addrs.text_next, &global.addrs.text_jmp);
    LOG("Apply at 0x%08X", unsigned(p));
    LOG("text_next = 0x%08X", (unsigned)global.addrs.text_next);
    LOG("text_jmp = 0x%08X", (unsigned)global.addrs.text_jmp);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Text)

DEFINE_PIECE_BEGIN(Sora, Dcdat, ".text", PatternType::Bytes,
                   "83 EC 10 53 8B 5C 24 1C 56 57 8B 7C 24 20 33 F6")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = true;
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    uint8_t* p = results.front();
    bool rst = Group->BackupCode(p, 8, asm_sora::dcdat, &global.addrs.dcdat_next);
    LOG("Apply at 0x%08X", unsigned(p));
    LOG("dcdat_next = 0x%08X", (unsigned)global.addrs.dcdat_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Dcdat)

DEFINE_PIECE_BEGIN(Sora, Ldat, ".text", PatternType::Bytes,
                   "8B ?? ?5 ?? ?? ?? ??")
    const utils::MemMatcher matcher_fscs = utils::MemMatcher(
            "56 57 8B 7C 24 10", PatternType::Bytes);
    const utils::MemMatcher matcher_3rd = utils::MemMatcher(
            "8B 4C 24 08 56", PatternType::Bytes);
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = (   (*(b + 1) == 0x0C && *(b + 2) == 0xB5 && matcher_fscs.Match(b - 0x11))
                || (*(b + 1) == 0x14 && *(b + 2) == 0x85 && matcher_3rd.Match(b - 0x13)))
               && Group->InSection(".data", *(uint8_t**)(b + 3), sizeof(uint8_t*) * 0x20)
               && REF_STRING(".data", *(uint8_t**)(b + 3) + 6 * sizeof(uint8_t*), "", "CH20000 ._CH")
               && REF_STRING(".data", *(uint8_t**)(b + 3) + 7 * sizeof(uint8_t*), "", "CH00000 ._CH")
               && REF_STRING(".data", *(uint8_t**)(b + 3) + 9 * sizeof(uint8_t*), "", "CH10000 ._CH");
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    uint8_t* p = results.front();
    global.addrs.pdirs = *(uint8_t**)(p + 3);
    bool rst = false;
    if (*(p + 1) == 0x0C) {
        rst = Group->BackupCode(p - 0x11, 6, asm_sora::ldat, &global.addrs.ldat_next);
        LOG("Apply at 0x%08X: ", (unsigned)(p - 0x11));
    } else {
        rst = Group->BackupCode(p - 0x13, 5, asm_sora::ldat, &global.addrs.ldat_next);
        LOG("Apply at 0x%08X: ", (unsigned)(p - 0x13));
    }
    LOG("pdirs = 0x%08X", (unsigned)global.addrs.pdirs);
    LOG("ldat_next = 0x%08X", (unsigned)global.addrs.ldat_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Ldat)

DEFINE_PIECE_BEGIN(Sora, Textse, ".text", PatternType::Bytes,
                   "F6 C4 40 " "5B " "74 0E " "55 55 55 55 " "6A 05 " "E8 ?? ?? ?? ??")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = true;
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    uint8_t* p = results.front() + 12;
    bool rst = Group->RedirectWithCall(p, 5, asm_sora::textse,
                                       nullptr, &global.addrs.textse_jmp);
    LOG("Apply at 0x%08X", unsigned(p));
    LOG("textse_jmp = 0x%08X", (unsigned)global.addrs.textse_jmp);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Textse)

DEFINE_PIECE_BEGIN(Sora, Dlgse, ".text", PatternType::Bytes,
                   "55 55 55 55 55 " "89 8E ?? ?? 00 00 " "E8 ?? ?? ?? ?? " "83 C4 14 ")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = true;
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    uint8_t* p = results.front() + 11;
    bool rst = Group->RedirectWithCall(p, 5, asm_sora::dlgse,
                                       nullptr, &global.addrs.dlgse_jmp);
    LOG("Apply at 0x%08X", unsigned(p));
    LOG("dlgse_next = 0x%08X", (unsigned)global.addrs.dlgse_jmp);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Dlgse)

DEFINE_PIECE_BEGIN(Sora, Strpatch2, ".text", PatternType::Bytes, "68 ?? ?? ?? 00")
    const startup::RefPatchingStrings strs_map_
        = startup::LoadRefPatchingStrings("voice/scena/Z_POKERC._DT");
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = false;
    if (!std::get<2>(strs_map_).empty()) {
        rst = std::get<2>(strs_map_).count(*(int*)(b + 1));
    }
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = true;
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    int count = 0;
#ifdef LOG
    std::unordered_map<int, int> cnts;
    for (const auto& kv : std::get<2>(strs_map_)) {
        cnts[kv.first] = 0;
    }
#endif
    for (uint8_t* b : GetResults()) {
        int offset = *(int*)(b + 1);
        const std::string& s = std::get<2>(strs_map_).find(offset)->second;
        if (Group->RefPatchString(b + 1, s)) {
            count++;
#ifdef LOG
            cnts[offset]++;
#endif
        }
    }
    LOG("%d Strings Patched.", count);
#ifdef LOG
    if (!GetResults().empty()) {
        std::string zero;
        char buff[32];
        for (const auto& kv : cnts) {
            if (kv.second == 0) {
                sprintf(buff, "%08X", kv.first);
                zero += buff;
                zero += ",";
            }
        }
        if (!zero.empty()) {
            LOG("These strings loaded, but not used: \n%s", zero.c_str());
        }
    }
#endif
DEFINE_APPLY_END(true)
DEFINE_PIECE_END(Strpatch2)

ADD_PIECES_BEGIN()
ADD_PIECE(Hwnd)
ADD_PIECE(Ldat)
ADD_PIECE(Dcdat)
ADD_PIECE(Text)
ADD_PIECE(Textse)
ADD_PIECE(Dlgse)
ADD_PIECE(Strpatch2)
ADD_PIECES_END()

DEFINE_GROUP_END()
}  // namespace

namespace startup {
DEFINE_STATIC_GET_GROUP(Sora);
}  // namespace startup
