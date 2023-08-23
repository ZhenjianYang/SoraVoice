#include "startup/scan_group_common.h"

#include <map>

#include "asm/asm.h"
#include "global/global.h"
#include "startup/string_patch.h"
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
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    void** addr = *(void***)(results.front() + 17);
    global.addrs.pHwnd = addr;
    LOG("pHwnd = 0x%08X", (unsigned)global.addrs.pHwnd);
    DEFINE_APPLY_END(true)
        DEFINE_PIECE_END(Hwnd)

        DEFINE_PIECE_BEGIN(Tits, Text, ".text", PatternType::Bytes,
            "23 " "0F 85 ?? ?? ?? ??")
        const utils::MemMatcher matcher_fc = utils::MemMatcher(
            "80 F9 23", PatternType::Bytes);
        const utils::MemMatcher matcher_sc = utils::MemMatcher(
            "8B 74 24 20 E9 ?? ?? ?? ?? 3C 23", PatternType::Bytes);
        const utils::MemMatcher matcher_3rd = utils::MemMatcher(
            "89 B3 ?? ?? ?? ?? E9 ?? ?? ?? ?? 3C 23", PatternType::Bytes);
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    uint8_t* dst = utils::GetCallJmpDest(b + 1, 6);
    bool rst = Group->InSection(".text", dst, 1)
        && (matcher_fc.Match(b - 2) || matcher_sc.Match(b - 10) || matcher_3rd.Match(b - 12));
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    uint8_t* p = results.back() + 1;
    bool rst = Group->RedirectWithJmp(
            p, 6, asm_tits::text, &global.addrs.text_next, &global.addrs.text_jmp);
    if (*(p + 9) == 0x46) {
        global.info.game = GameTitsFC;
    } else {
        global.info.game = GameTitsSC3rd;
    }
    LOG("Apply at 0x%08X", unsigned(p));
    LOG("text_next = 0x%08X", (unsigned)global.addrs.text_next);
    LOG("text_jmp = 0x%08X", (unsigned)global.addrs.text_jmp);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Text)

DEFINE_PIECE_BEGIN(Tits, Ldat, ".text", PatternType::Bytes,
                   "68 ?? ?? ?? ??")
    const utils::MemMatcher matcher_cn = utils::MemMatcher(
        "E9 ?? ?? ?? ?? 90", PatternType::Bytes);
    const utils::MemMatcher matcher_en = utils::MemMatcher(
        "81 EC 84 02 00 00", PatternType::Bytes);
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = REF_STRING(".text", b + 1, ".rdata", L"ED6_DT%02x.DAT");
    if (rst) {
        for (int i = 5; i <= 0x20; i++) {
            if (matcher_cn.Match(b - i) || matcher_en.Match(b - i)) {
                rst = true;
                break;
            }
        }
    }
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    uint8_t* p = results.front();
    bool rst = false;
    for (int i = 5; i <= 0x20; i++) {
        if (matcher_cn.Match(p - i)) {
            rst = Group->RedirectWithJmp(p - i, 5, asm_tits::ldat, nullptr, &global.addrs.ldat_next);
            LOG("Apply at 0x%08X", unsigned(p - i));
            LOG("ldat_next = 0x%08X", (unsigned)global.addrs.ldat_next);
            break;
        } else if (matcher_en.Match(p - i)) {
            rst = Group->BackupCode(p - i, 6, asm_tits::ldat, &global.addrs.ldat_next);
            LOG("Apply at 0x%08X", unsigned(p - i));
            LOG("ldat_next = 0x%08X", (unsigned)global.addrs.ldat_next);
            break;
        }
    }
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Ldat)

DEFINE_PIECE_BEGIN(Tits, Dcdat, ".text", PatternType::Bytes,
                   "57 8B D9 C7 44 24 18 00 00 00 00")
    const utils::MemMatcher matcher_cn = utils::MemMatcher(
        "E9 ?? ?? ?? ??", PatternType::Bytes);
    const utils::MemMatcher matcher_en = utils::MemMatcher(
        "83 EC 18 53 56", PatternType::Bytes);
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = matcher_cn.Match(b - 5) || matcher_en.Match(b - 5);
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    uint8_t* p = results.front() - 5;
    bool rst = false;
    if (*p == 0x83) {
        rst = Group->BackupCode(p, 5, asm_tits::dcdat, &global.addrs.dcdat_next);
    } else {
        rst = Group->RedirectWithJmp(p, 5, asm_tits::dcdat, nullptr, &global.addrs.dcdat_next);
    }
    LOG("Apply at 0x%08X", unsigned(p));
    LOG("dcdat_next = 0x%08X", (unsigned)global.addrs.dcdat_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Dcdat)

DEFINE_PIECE_BEGIN(Tits, Pdirs, ".text", PatternType::Bytes,
                   "8D 0C C0 8B 04 95 ?? ?? ?? 00")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = Group->InSection(".data", *(uint8_t**)(b + 6), sizeof(uint8_t*) * 0x20)
               && REF_STRING(".data", *(uint8_t**)(b + 6) + 6 * sizeof(uint8_t*), "", "CH20000 ._CH")
               && REF_STRING(".data", *(uint8_t**)(b + 6) + 7 * sizeof(uint8_t*), "", "CH00000 ._CH")
               && REF_STRING(".data", *(uint8_t**)(b + 6) + 9 * sizeof(uint8_t*), "", "CH10000 ._CH");
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    uint8_t* p = results.front();
    global.addrs.pdirs = *(uint8_t**)(p + 6);
    LOG("pdirs = 0x%08X", (unsigned)global.addrs.pdirs);
    bool rst = true;
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Pdirs)

DEFINE_PIECE_BEGIN(Tits, Textse, ".text", PatternType::Bytes,
                   "74 1E " "80 3D ?? ?? ?? ?? 00 " "75 ?? " "6A 00")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = Group->InSection(".data", *(uint8_t**)(b + 4), sizeof(uint8_t*));
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    uint8_t* p = results.front();
    global.addrs.textse_jmp = utils::GetCallJmpDest(p + 9, 2);
    bool rst = Group->BackupCode(p + 2, 7, asm_tits::textse, &global.addrs.textse_next);
    LOG("Apply at 0x%08X", unsigned(p + 2));
    LOG("textse_jmp = 0x%08X", (unsigned)global.addrs.textse_jmp);
    LOG("textse_next = 0x%08X", (unsigned)global.addrs.textse_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Textse)

DEFINE_PIECE_BEGIN(Tits, Dlgse, ".text", PatternType::Bytes,
                   "6A 00 " "89 86 ?? ?? ?? ?? " "E8 ?? ?? ?? ?? " "83 C4 0C")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = true;
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    uint8_t* p = results.front();
    bool rst = Group->RedirectWithCall(p + 8, 5, asm_tits::dlgse, &global.addrs.dlgse_next, &global.addrs.dlgse_jmp);
    LOG("Apply at 0x%08X", unsigned(p + 8));
    LOG("dlgse_jmp = 0x%08X", (unsigned)global.addrs.dlgse_jmp);
    LOG("dlgse_next = 0x%08X", (unsigned)global.addrs.dlgse_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Dlgse)

DEFINE_PIECE_BEGIN(Tits, Strpatch, ".text", PatternType::Bytes, "?? ?? ?? ?? 00")
    const startup::PatchingStrings strs_map_ = startup::LoadPatchingStrings();
    mutable std::unordered_map<uint8_t*, typename decltype(strs_map_.cbegin())> to_patch_;
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = false;
    for (auto it = strs_map_.cbegin(); it != strs_map_.cend(); ++it) {
        if (REF_STRING(".text", b + 1, ".rdata", it->first)) {
            to_patch_[b] = it;
            rst = true;
        }
    }
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = true;
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    int count = 0;
    for (const auto& kv : to_patch_) {
        LOG("Patch at: 0x%08X", (unsigned)kv.first);
        LOG("String old: %s", kv.second->first.c_str());
        LOG("String new: %s", kv.second->second.c_str());
        if (Group->RefPatchString(kv.first + 1, kv.second->second)) {
            LOG("Patch Succeeded.");
            count++;
        } else {
            LOG("Patch Failed.");
        }
    }
    LOG("%d Strings Patched.", count);
DEFINE_APPLY_END(true)
DEFINE_PIECE_END(Strpatch)

DEFINE_PIECE_BEGIN(Tits, Strpatch2, ".text", PatternType::Bytes, "?? ?? ?? ?? 00")
    const startup::RefPatchingStrings strs_map_
        = startup::LoadRefPatchingStrings("voice/scena/Z_POKER9._DT");
    mutable int diff = std::numeric_limits<int>::max();
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = false;
    if (!std::get<2>(strs_map_).empty()) {
        if (GetResults().empty()) {
            if (REF_STRING(".text", b + 1, ".rdata", std::get<1>(strs_map_))) {
                diff = (uint8_t*)std::get<0>(strs_map_) - *(uint8_t**)(b + 1);
                rst = true;
            }
        } else {
            rst = std::get<2>(strs_map_).count(*(int*)(b + 1) + diff);
        }
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
        int offset = *(int*)(b + 1) + diff;
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
        std::string zero, mul;
        for (const auto& kv : cnts) {
            if (kv.second == 0) {
                zero += std::to_string(kv.first) + ",";
            } else if (kv.second > 1) {
                mul += std::to_string(kv.first) + ",";
            }
        }
        if (!zero.empty()) {
            LOG("These strings loaded, but not used: \n%s", zero.c_str());
        }
        if (!mul.empty()) {
            LOG("These strings used more than once: \n%s", mul.c_str());
        }
    }
#endif
DEFINE_APPLY_END(true)
DEFINE_PIECE_END(Strpatch2)

ADD_PIECES_BEGIN()
ADD_PIECE(Hwnd)
ADD_PIECE(Text)
ADD_PIECE(Ldat)
ADD_PIECE(Dcdat)
ADD_PIECE(Pdirs)
ADD_PIECE(Textse)
ADD_PIECE(Dlgse)
ADD_PIECE(Strpatch)
ADD_PIECE(Strpatch2)
ADD_PIECES_END()

DEFINE_GROUP_END()
}  // namespace

namespace startup {
DEFINE_STATIC_GET_GROUP(Tits);
}  // namespace startup
