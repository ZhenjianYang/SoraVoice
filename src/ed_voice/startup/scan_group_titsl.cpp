#include "startup/scan_group_common.h"

#include <map>

#include "asm/asm.h"
#include "global/global.h"
#include "startup/string_patch.h"
#include "utils/log.h"
#include "utils/mem.h"
#include "utils/section_info.h"

namespace {

DEFINE_GROUP_BEGIN(Titsl)

DEFINE_PIECE_BEGIN(Titsl, Hwnd, ".text", PatternType::Bytes,
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

DEFINE_PIECE_BEGIN(Titsl, Text, ".text", PatternType::Bytes,
                   "80 F9 23 0F 85 ?? ?? ?? ?? ?? ?? ?? ??")
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
            p, 6, asm_titsl::text, &global.addrs.text_next, &global.addrs.text_jmp);
    if (*(p + 9) == 0x43) {
        global.info.game = GameTitsFC;
    } else {
        global.info.game = GameTitsSC3rd;
    }
    LOG("Apply at 0x%08X", unsigned(p));
    LOG("text_next = 0x%08X", (unsigned)global.addrs.text_next);
    LOG("text_jmp = 0x%08X", (unsigned)global.addrs.text_jmp);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Text)

DEFINE_PIECE_BEGIN(Titsl, Ldat, ".text", PatternType::Bytes,
                   "68 ?? ?? ?? ??")
    const utils::MemMatcher matcher_cn = utils::MemMatcher(
        "E9 ?? ?? ?? ?? 90", PatternType::Bytes);
    const utils::MemMatcher matcher_en = utils::MemMatcher(
        "81 EC 84 02 00 00", PatternType::Bytes);
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = (matcher_cn.Match(b - 0x19) || matcher_en.Match(b - 0x19))
               && REF_STRING(".text", b + 1, ".rdata", L"ED6_DT%02x.DAT");
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front() - 0x19;
    bool rst = false;
    if (*p == 0x81) {
        rst = Group->BackupCode(p, 6, asm_titsl::ldat, &global.addrs.ldat_next);
    } else {
        rst = Group->RedirectWithJmp(p, 5, asm_titsl::ldat, nullptr, &global.addrs.ldat_next);
    }
    LOG("Apply at 0x%08X", unsigned(p));
    LOG("ldat_next = 0x%08X", (unsigned)global.addrs.ldat_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Ldat)

DEFINE_PIECE_BEGIN(Titsl, Dcdat, ".text", PatternType::Bytes,
                   "89 44 24 04 8B 07 56")
    const utils::MemMatcher matcher_cn = utils::MemMatcher(
        "E9 ?? ?? ?? ??", PatternType::Bytes);
    const utils::MemMatcher matcher_en = utils::MemMatcher(
        "83 EC 18 8B 03", PatternType::Bytes);
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = matcher_cn.Match(b - 5) || matcher_en.Match(b - 5);
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front() - 5;
    bool rst = false;
    if (*p == 0x83) {
        rst = Group->BackupCode(p, 5, asm_titsl::dcdat, &global.addrs.dcdat_next);
    } else {
        rst = Group->RedirectWithJmp(p, 5, asm_titsl::dcdat, nullptr, &global.addrs.dcdat_next);
    }
    LOG("Apply at 0x%08X", unsigned(p));
    LOG("dcdat_next = 0x%08X", (unsigned)global.addrs.dcdat_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Dcdat)

DEFINE_PIECE_BEGIN(Titsl, Pdirs, ".text", PatternType::Bytes,
                   "C1 E9 10 8B 3C 8D ?? ?? ?? 00")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = Group->InSection(".data", *(byte**)(b + 6), sizeof(byte*) * 0x20)
               && REF_STRING(".data", *(byte**)(b + 6) + 6 * sizeof(byte*), "", "CH20000 ._CH")
               && REF_STRING(".data", *(byte**)(b + 6) + 7 * sizeof(byte*), "", "CH00000 ._CH")
               && REF_STRING(".data", *(byte**)(b + 6) + 9 * sizeof(byte*), "", "CH10000 ._CH");
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front();
    global.addrs.pdirs = *(byte**)(p + 6);
    LOG("pdirs = 0x%08X", (unsigned)global.addrs.pdirs);
    bool rst = true;
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Pdirs)

DEFINE_PIECE_BEGIN(Titsl, Textse, ".text", PatternType::Bytes,
                   "57 " "FF D2 " "80 3D ?? ?? ?? ?? 00 " "75 11")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = Group->InSection(".data", *(byte**)(b + 5), sizeof(byte*))
               && Group->InSection(".text", b + 0x2C, sizeof(byte*))
               && *(b + 0x2C) == 0xC3;
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front();
    global.addrs.textse_jmp = utils::GetCallJmpDest(p + 10, 2);
    bool rst = Group->BackupCode(p + 12, 6, asm_titsl::textse, &global.addrs.textse_next);
    LOG("Apply at 0x%08X", unsigned(p + 12));
    LOG("textse_jmp = 0x%08X", (unsigned)global.addrs.textse_jmp);
    LOG("textse_next = 0x%08X", (unsigned)global.addrs.textse_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Textse)

DEFINE_PIECE_BEGIN(Titsl, Dlgse, ".text", PatternType::Bytes,
                   "57 " "FF D0 " "80 3D ?? ?? ?? ?? 00 " "75 11")
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = Group->InSection(".data", *(byte**)(b + 5), sizeof(byte*));
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
    bool rst = !GetResults().empty();
DEFINE_CHECK_RESULTS_END(rst)
DEFINE_APPLY_BEGIN()
    const auto& results = GetResults();
    byte* p = results.front();
    global.addrs.dlgse_jmp = utils::GetCallJmpDest(p + 10, 2);
    bool rst = Group->BackupCode(p + 3, 7, asm_titsl::dlgse, &global.addrs.dlgse_next);
    LOG("Apply at 0x%08X", unsigned(p + 3));
    LOG("dlgse_jmp = 0x%08X", (unsigned)global.addrs.dlgse_jmp);
    LOG("dlgse_next = 0x%08X", (unsigned)global.addrs.dlgse_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Dlgse)

DEFINE_PIECE_BEGIN(Titsl, Strpatch, ".text", PatternType::Bytes, "68 ?? ?? ?? 00")
    const startup::PatchingStrings strs_map_ = startup::LoadPatchingStrings();
    mutable std::unordered_map<byte*, typename decltype(strs_map_.cbegin())> to_patch_;
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

DEFINE_PIECE_BEGIN(Titsl, Strpatch2, ".text", PatternType::Bytes, "68 ?? ?? ?? 00")
    const startup::RefPatchingStrings strs_map_
        = startup::LoadRefPatchingStrings("voice/scena/Z_POKER9._DT");
    mutable int diff = std::numeric_limits<int>::max();
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = false;
    if (!std::get<2>(strs_map_).empty()) {
        if (GetResults().empty()) {
            if (REF_STRING(".text", b + 1, ".rdata", std::get<1>(strs_map_))) {
                diff = (byte*)std::get<0>(strs_map_) - *(byte**)(b + 1);
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
    for (byte* b : GetResults()) {
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
        std::string zero;
        for (const auto& kv : cnts) {
            if (kv.second == 0) {
                zero += std::to_string(kv.first) + ",";
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
DEFINE_STATIC_GET_GROUP(Titsl);
}  // namespace startup
