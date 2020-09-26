#ifndef __STARTUP_SCAN_GROUP_COMMON_H__
#define __STARTUP_SCAN_GROUP_COMMON_H__

#include "scan_group.h"

#include <unordered_map>

#include "base/byte.h"
#include "utils/mem.h"
#include "utils/mem_scanner.h"
#include "utils/section_info.h"
#include "utils/str.h"

namespace startup {
class ScanGroupCommon : public ScanGroup {
public:
    using MemScanner = utils::MemScanner<byte*>;
    using MemMatch = utils::MemMatcher;
    using BasicPiece = MemScanner::BasicPiece;
    using PatternType = MemScanner::PatternType;

    static constexpr std::size_t kCodeBackupBlockSize = 0x1000;

    bool IsValid() const override {
        return is_valid_;
    }
    const std::string& Name() const override {
        return name_;
    }
    const std::string& GameTitle() const override {
        return title_;
    }
    const std::string& GameBuiltDate() const override {
        return date_;
    }
    std::vector<std::unique_ptr<char[]>>&& MovableStrings() override {
        return std::move(strings_);
    }

    bool Scan() override {
        bool result = true;
        for (const auto& scanner : scanners_) {
            scanner->Search();
            result = result && scanner->CheckResult();
        }
        return result;
    }
    bool Inject() override {
        bool result = true;
        for (const auto& scanner : scanners_) {
            result = result && scanner->Apply();
        }
        return result;
    }

    ScanGroupCommon(const char* name, const std::vector<utils::SectionInfo>& section_info)
        : name_{ name } {
        for (const auto& sec : section_info) {
            secs_[sec.name] = sec;
        }

        strings_.push_back(std::make_unique<char[]>(kCodeBackupBlockSize));
        code_bak_ = (byte*)strings_.back().get();
        code_bak_remian = kCodeBackupBlockSize;
    }

    bool InSection(const char* sec_name, byte* begin, std::size_t length) const {
        if (!sec_name || *sec_name == '\0') {
            return true;
        }
        auto it = secs_.find(sec_name);
        if (it == secs_.end()) {
            return false;
        }
        const auto& sec = it->second;
        return begin >= sec.start && begin + length <= sec.end;
    }

    byte* GetCodeBackupBlock(std::size_t length) {
        if (code_bak_remian < length) {
            return nullptr;
        }
        byte* ret = code_bak_;
        code_bak_ += length;
        code_bak_remian -= length;
        return ret;
    }

    template<typename String>
    bool DirectPatchString(byte* p, const String& s, bool null_termanate = false) {
        using Char = typename std::remove_const_t<std::remove_reference_t<decltype(s[0])>>;
        
        Char* pc = reinterpret_cast<Char*>(p);
        std::size_t len;
        if constexpr (std::is_pointer_v<String>) {
            len = std::char_traits<Char>::length(s);
        } else if (std::is_array_v<String>) {
            len = std::size(s) - 1;
        } else {
            len = std::size(s);
        }
        std::size_t chars_written = null_termanate ? (len + 1) * sizeof(Char) : len * sizeof(Char);

        utils::MemProtection proction_bak, proction_bak2;
        if (utils::ChangeMemProtection(p, chars_written, utils::kMemProtectionRWE, &proction_bak)) {
            if constexpr (std::is_pointer_v<String>) {
                std::copy_n(s, len, pc);
            } else {
                std::copy_n(std::begin(s), len, pc);
            }
            if (null_termanate) {
                pc[len] = 0;
            }
            utils::ChangeMemProtection(p, chars_written, proction_bak, &proction_bak2);
            return true;
        }
        return false;
    }

    template<typename String>
    bool RefPatchString(byte* p, const String& s) {
        using Char = typename std::remove_const_t<std::remove_reference_t<decltype(s[0])>>;
        std::size_t len;
        if constexpr (std::is_pointer_v<String>) {
            len = std::char_traits<Char>::length(s);
        } else if (std::is_array_v<String>) {
            len = std::size(s) - 1;
        } else {
            len = std::size(s);
        }
        strings_.push_back(std::make_unique<char[]>((len + 1) * sizeof(Char)));
        char* str_new = strings_.back().get();

        utils::MemProtection proction_bak, proction_bak2;
        if (utils::ChangeMemProtection(p, sizeof(str_new), utils::kMemProtectionRWE, &proction_bak)) {
            *(char**)p = str_new;
            utils::ChangeMemProtection(p, sizeof(str_new), proction_bak, &proction_bak2);
            return true;
        }
        return false;
    }

    bool BackupCode(byte* p, std::size_t len, void* jmp_dst, void** next) {
        byte* bak = GetCodeBackupBlock(len + 5);
        std::memcpy(bak, p, len);
        utils::FillWithJmp(bak + len, p + len);
        if (next) {
            *next = bak;
        }
        utils::MemProtection proction_bak, proction_bak2;
        if (utils::ChangeMemProtection(p, len, utils::kMemProtectionRWE, &proction_bak)) {
            utils::FillNop(p, len);
            utils::FillWithJmp(p, jmp_dst);
            utils::ChangeMemProtection(p, len, proction_bak, &proction_bak2);
            return true;
        }
        return false;
    }

    bool RedirectWithJmp(byte* p, std::size_t len, void* dst, void** next, void** dst_old) {
        if (dst_old) {
            *dst_old = utils::GetCallJmpDest(p, len);
        }
        if (next) {
            *next = p + len;
        }
        
        utils::MemProtection proction_bak, proction_bak2;
        if (utils::ChangeMemProtection(p, len, utils::kMemProtectionRWE, &proction_bak)) {
            utils::FillNop(p, len);
            utils::FillWithJmp(p, dst);
            utils::ChangeMemProtection(p, len, proction_bak, &proction_bak2);
            return true;
        }
        return false;
    }

    bool RedirectWithCall(byte* p, std::size_t len, void* dst, void** next, void** dst_old) {
        if (dst_old) {
            *dst_old = utils::GetCallJmpDest(p, len);
        }
        if (next) {
            *next = p + len;
        }
        utils::MemProtection proction_bak, proction_bak2;
        if (utils::ChangeMemProtection(p, len, utils::kMemProtectionRWE, &proction_bak)) {
            utils::FillNop(p, len);
            utils::FillWithCall(p, dst);
            utils::ChangeMemProtection(p, len, proction_bak, &proction_bak2);
            return true;
        }
        return false;
    }

protected:
    bool is_valid_ = false;
    std::string name_;
    std::string title_;
    std::string date_;
    std::vector<std::unique_ptr<char[]>> strings_;

    std::vector<std::unique_ptr<MemScanner>> scanners_;
    std::unordered_map<std::string, utils::SectionInfo> secs_;

    byte* code_bak_ = nullptr;
    std::size_t code_bak_remian = 0;

    virtual bool AddPieces() = 0;

private:
    ScanGroupCommon(const ScanGroupCommon&) = delete;
    ScanGroupCommon& operator=(const ScanGroupCommon&) = delete;
};  // ScanGroupCommon

class PieceCommon : public ScanGroupCommon::BasicPiece {
public:
    PieceCommon(std::string_view pattern, ScanGroupCommon::PatternType pattern_type,
                ScanGroupCommon* group)
        : ScanGroupCommon::BasicPiece(pattern, pattern_type), Group{ group } {
    }

    bool InSection(byte* p, std::size_t len) {

    }

protected:
    ScanGroupCommon* const Group;
};

}  // namespace startup

#define DEFINE_GROUP_BEGIN(GroupName_) \
class ScanGroup##GroupName_ : public startup::ScanGroupCommon {\
public:\
    explicit ScanGroup##GroupName_(const std::vector<utils::SectionInfo>& section_info)\
            : startup::ScanGroupCommon(#GroupName_, section_info) { \
        is_valid_ = AddPieces(); }

#define DEFINE_PIECE_BEGIN(GroupName_, PieceName_, Section_, ParttenType_, Partten_) \
        class Piece##PieceName_ : public BasicPiece { \
        public: \
            static constexpr char Section[] = Section_; \
            static constexpr char Partten[] = Partten_; \
            static constexpr  PatternType PatternType = ParttenType_; \
            Piece##PieceName_(ScanGroup##GroupName_* group) \
                : BasicPiece(Partten_, ParttenType_), \
                  Group { group }, Name { #PieceName_ } {  } \
        private: \
            ScanGroup##GroupName_* const Group; \
            const std::string Name;

#define DEFINE_ADDITIONAL_MATCH_BEGIN(b, e) \
            bool AdditionalMatch(byte* b, byte* e) const override {
#define DEFINE_ADDITIONAL_MATCH_END(rst) \
            return (rst); }

#define DEFINE_CHECK_RESULTS_BEGIN() \
            bool CheckResults() const override { \
                LOG("%s:%s, start check results...", \
                    Group->Name().c_str(), Name.c_str()); \
                LOG("%d results matched.", GetResults().size());
#define DEFINE_CHECK_RESULTS_END(rst) \
            if (rst) { \
                LOG("%s:%s check results passed!", Group->Name().c_str(), Name.c_str()); \
            } else { \
                LOG("%s:%s check results failed!", Group->Name().c_str(), Name.c_str()); \
            } return (rst); }

#define DEFINE_APPLY_BEGIN() \
            bool Apply() const override { \
                LOG("%s:%s, start apply...", \
                    Group->Name().c_str(), Name.c_str()); \
                LOG("%d results matched.", GetResults().size());
#define DEFINE_APPLY_END(rst) \
            if (rst) { \
                LOG("%s:%s apply succeeded!", Group->Name().c_str(), Name.c_str()); \
            } else { \
                LOG("%s:%s apply failed!", Group->Name().c_str(), Name.c_str()); \
            } return (rst); }

#define DEFINE_PIECE_END(PieceName_) };

#define ADD_PIECES_BEGIN() \
    bool AddPieces() override { \
        std::unordered_map<std::string, MemScanner*> scanners;

#define ADD_PIECE(PieceName_) { \
        auto piece = std::make_unique<Piece##PieceName_>(this); \
        if (!scanners.count(piece->Section)) { \
            auto it = secs_.find(piece->Section); \
            if (it == secs_.end()) { \
                return false; \
            } \
            auto& sec = it->second; \
            scanners_.push_back(std::make_unique<MemScanner>(sec.start, sec.end)); \
            scanners[piece->Section] = scanners_.back().get(); \
        } \
        scanners[piece->Section]->AddPiece(std::move(piece)); \
        }
#define ADD_PIECES_END() return true; }
#define DEFINE_GROUP_END() };

#define DEFINE_STATIC_GET_GROUP(GroupName_) \
std::unique_ptr<ScanGroup> \
GetScanGroup##GroupName_(const std::vector<utils::SectionInfo>& section_info) { \
    auto group = std::make_unique<ScanGroup##GroupName_>(section_info); \
    return group; \
}

#define REF_STRING(SectionCode, p, SectionData, STR) \
    Group->InSection(SectionCode, (byte*)(p), sizeof(byte*)) && \
    Group->InSection(SectionData, *(byte**)(p), sizeof(STR)) && \
    utils::StringMatch(*(byte**)(p), STR, true)

#endif  // __STARTUP_SCAN_GROUP_COMMON_H__
