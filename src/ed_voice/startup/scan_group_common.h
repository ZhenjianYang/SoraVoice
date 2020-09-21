#ifndef __STARTUP_SCAN_GROUP_COMMON_H__
#define __STARTUP_SCAN_GROUP_COMMON_H__

#include "scan_group.h"

#include <unordered_map>

#include "base/byte.h"
#include "utils/mem_scanner.h"

namespace startup {
class ScanGroupCommon : public ScanGroup {
public:
    ScanGroupCommon() = default;

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

protected:
    bool is_valid_ = false;
    std::string name_;
    std::string title_;
    std::string date_;
    std::vector<std::unique_ptr<char[]>> strings_;

    std::vector<std::unique_ptr<utils::MemScanner<byte*>>> scanners_;

private:
    ScanGroupCommon(const ScanGroupCommon&) = delete;
    ScanGroupCommon& operator=(const ScanGroupCommon&) = delete;
    ScanGroupCommon(ScanGroupCommon&&) = delete;
    ScanGroupCommon& operator=(ScanGroupCommon&&) = delete;
};  // ScanGroupCommon
}  // namespace startup

#endif  // __STARTUP_SCAN_GROUP_COMMON_H__
