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
            p, 6, asm_tits::text, &global.addrs.text_next, &global.addrs.text_jmp);
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
    bool rst = Group->BackupCode(p, 6, asm_tits::ldat, &global.addrs.ldat_next);
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
    bool rst = Group->BackupCode(p, 5, asm_tits::dcdat, &global.addrs.dcdat_next);
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
    bool rst = Group->BackupCode(p + 11, 6, asm_tits::textse, &global.addrs.textse_next);
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
    bool rst = Group->BackupCode(p + 11, 6, asm_tits::dlgse, &global.addrs.dlgse_next);
    LOG("dlgse_jmp = 0x%08X", (unsigned)global.addrs.dlgse_jmp);
    LOG("dlgse_next = 0x%08X", (unsigned)global.addrs.dlgse_next);
DEFINE_APPLY_END(rst)
DEFINE_PIECE_END(Dlgse)

DEFINE_PIECE_BEGIN(Tits, Strpatch, ".rdata", PatternType::Bytes, "")
    const startup::PatchingStrings strs_map_ = startup::LoadPatchingStrings();
    mutable std::unordered_map<byte*, typename decltype(strs_map_.cbegin())> to_patch_;
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = false;
    for (auto it = strs_map_.cbegin(); it != strs_map_.cend(); ++it) {
        if (int(it->first.length()) + 1 <= e - b && utils::StringMatch(b, it->first, true)) {
            to_patch_[b] = it;
            rst = true;
            LOG("%s:%s at 0x%08X",
                Group->Name().c_str(), Name.c_str(), (unsigned)(b));
            LOG("String old: %s", it->first.c_str());
            LOG("String new: %s", it->second.c_str());
        }
    }
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
DEFINE_CHECK_RESULTS_END(true)
DEFINE_APPLY_BEGIN()
    int count = 0;
    for (const auto& kv : to_patch_) {
        LOG("Patch at: 0x%08X", (unsigned)kv.first);
        LOG("String old: %s", kv.second->first.c_str());
        LOG("String new: %s", kv.second->second.c_str());
        bool rst = Group->DirectPatchString(kv.first, kv.second->second, true);
        if (rst) {
            LOG("Patch Succeeded.");
            count++;
        } else {
            LOG("Patch Failed.");
        }
    }
    LOG("%d Strings Patched.", count);
DEFINE_APPLY_END(true)
DEFINE_PIECE_END(Strpatch)

DEFINE_PIECE_BEGIN(Tits, Strpatch2, ".text", PatternType::Bytes, "68 ?? ?? ?? 00")
    const startup::RefPatchingStrings strs_map_
        = startup::LoadRefPatchingStrings("voice/scena/Z_POKER9._DT");
    mutable int diff = std::numeric_limits<int>::max();
DEFINE_ADDITIONAL_MATCH_BEGIN(b, e)
    bool rst = false;
    if (GetResults().empty()) {
        if (REF_STRING(".text", b + 1, ".rdata", std::get<1>(strs_map_))) {
            diff = (byte*)std::get<0>(strs_map_) - *(byte**)(b + 1);
            rst = true;
        }
    } else {
        rst = std::get<2>(strs_map_).count(*(int*)(b + 1) + diff);
    }
    if (rst) {
        LOG("%s:%s at 0x%08X",
            Group->Name().c_str(), Name.c_str(), (unsigned)(b));
        LOG("Ref: 0x%08X", *(unsigned*)(b + 1));
        LOG("String old: %s", *(char**)(b + 1));
        LOG("String new: %s", std::get<2>(strs_map_).find(*(int*)(b + 1) + diff)->second.c_str());
    }
DEFINE_ADDITIONAL_MATCH_END(rst)
DEFINE_CHECK_RESULTS_BEGIN()
DEFINE_CHECK_RESULTS_END(true)
DEFINE_APPLY_BEGIN()
    int count = 0;
    std::map<int, std::vector<unsigned>> p;
    for (const auto& kv : std::get<2>(strs_map_)) {
        p[kv.first] = {};
    }
    for (byte* b : GetResults()) {
        int offset = *(int*)(b + 1) + diff;
        const std::string& s = std::get<2>(strs_map_).find(offset)->second;
        LOG("Patch at: 0x%08X", (unsigned)b);
        LOG("String old: %s", *(char**)(b + 1));
        LOG("String new: %s", s.c_str());
        bool rst = Group->RefPatchString(b + 1, s);
        if (rst) {
            LOG("Patch Succeeded.");
            p[offset].push_back((unsigned)b);
            count++;
        } else {
            LOG("Patch Failed.");
        }
    }
    LOG("%d Strings Patched.", count);

    for (const auto& kv : p) {
        LOG("String old offset: 0x%08X", kv.first);
        LOG("String new: %s", std::get<2>(strs_map_).find(kv.first)->second.c_str());
        LOG("String Patched %d times.", kv.second.size());
    }
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
