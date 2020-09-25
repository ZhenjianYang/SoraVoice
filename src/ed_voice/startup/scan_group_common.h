#ifndef __STARTUP_SCAN_GROUP_COMMON_H__
#define __STARTUP_SCAN_GROUP_COMMON_H__

#include "scan_group.h"

#include <unordered_map>

#include "base/byte.h"
#include "utils/mem.h"
#include "utils/mem_scanner.h"
#include "utils/section_info.h"

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
            ScanGroupTits* const Group; \
            const std::string Name;

#define DEFINE_ADDITIONAL_MATCH_BEGIN(b, e) \
            bool AdditionalMatch(byte* b, byte* e) const override { \
                LOG("%s:%s matched at 0x%08X, start addtional check...", \
                    Group->Name().c_str(), Name.c_str(), (unsigned)(b)); 
#define DEFINE_ADDITIONAL_MATCH_END(rst) \
            if (rst) { \
                LOG("%s:%s addtional check passed!", Group->Name().c_str(), Name.c_str()); \
            } else { \
                LOG("%s:%s addtional check failed!", Group->Name().c_str(), Name.c_str()); \
            } return (rst); }

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
std::unique_ptr<ScanGroup> GetScanGroupTits(const std::vector<utils::SectionInfo>& section_info) { \
    auto group = std::make_unique<ScanGroupTits>(section_info); \
    return group; \
}

#define REF_STRING(SectionCode, p, SectionData, STR) \
    Group->InSection(SectionCode, (byte*)(p), sizeof(byte*)) && \
    Group->InSection(SectionData, *(byte**)(p), sizeof(STR)) && \
    std::equal((std::remove_reference_t<decltype(STR[0])>*)(*(byte**)(p)), \
               (std::remove_reference_t<decltype(STR[0])>*)(*(byte**)(p) + sizeof(STR)), \
               STR)

#endif  // __STARTUP_SCAN_GROUP_COMMON_H__
