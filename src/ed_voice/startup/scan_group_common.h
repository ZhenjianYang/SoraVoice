#ifndef __STARTUP_SCAN_GROUP_COMMON_H__
#define __STARTUP_SCAN_GROUP_COMMON_H__

#include "scan_group.h"

#include <unordered_map>

#include "base/byte.h"
#include "utils/mem_scanner.h"
#include "utils/section_info.h"

namespace startup {
class ScanGroupCommon : public ScanGroup {
public:
    using MemScanner = utils::MemScanner<byte*>;
    using MemMatch = utils::MemMatcher;
    using BasicPiece = MemScanner::BasicPiece;
    using PatternType = MemScanner::PatternType;

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
        for (const auto& scanner : scanners_) {
            scanner->Apply();
        }
        return true;
    }

    ScanGroupCommon(const char* name, const std::vector<utils::SectionInfo>& section_info)
            : name_{ name } {
        for (const auto& sec : section_info) {
            secs_[sec.name] = sec;
        }
        std::unordered_map<std::string, std::unique_ptr<MemScanner>> scanners;
    }

    bool InSection(const char* sec_name, byte* begin, std::size_t length) const {
        auto it = secs_.find(sec_name);
        if (it == secs_.end()) {
            return false;
        }
        const auto& sec = it->second;
        return begin >= sec.start && begin + length <= sec.end;
    }

protected:
    bool is_valid_ = false;
    std::string name_;
    std::string title_;
    std::string date_;
    std::vector<std::unique_ptr<char[]>> strings_;

    std::vector<std::unique_ptr<MemScanner>> scanners_;
    std::unordered_map<std::string, utils::SectionInfo> secs_;

    virtual bool AddPieces() = 0;

private:
    ScanGroupCommon(const ScanGroupCommon&) = delete;
    ScanGroupCommon& operator=(const ScanGroupCommon&) = delete;
};  // ScanGroupCommon
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

#define DEFINE_ADDITIONAL_MATCH(begin, end) \
            bool AdditionalMatch(byte* begin, byte* end) const override

#define DEFINE_CHECK_RESULTS() \
            bool CheckResults() const override

#define DEFINE_APPLY() \
            void Apply() const override

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

#endif  // __STARTUP_SCAN_GROUP_COMMON_H__
