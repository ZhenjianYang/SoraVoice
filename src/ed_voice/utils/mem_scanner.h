#ifndef __UTILS_MEM_SCANNER_H__
#define __UTILS_MEM_SCANNER_H__

#include <functional>
#include <string_view>
#include <vector>

#include "utils/mem_matcher.h"

namespace utils {

template<typename Iterator>
class MemScanner {
public:
    using IterType = Iterator;
    using ResultsType = std::vector<Iterator>;
    using PatternType = MemMatcher::Type;
    using FuncAdditionalMatcher = std::function<bool(Iterator, const ResultsType&)>;
    using FuncResultsChecker = std::function<bool(const ResultsType&)>;
    using FuncApplier = std::function<void(const ResultsType&)>;

    MemScanner(Iterator begin, std::size_t length)
        : begin_{ begin }, end_{ begin + length } {
    }
    MemScanner(Iterator begin, Iterator end)
        : begin_{ begin }, end_{ end } {
    }

    void AddPiece(std::string_view pattern, PatternType pattern_type,
                  FuncAdditionalMatcher additional_matcher = nullptr,
                  FuncResultsChecker results_checker = nullptr,
                  FuncApplier applier = nullptr) {
        pieces_.emplace_back(pattern, pattern_type,
                             additional_matcher, results_checker, applier);
    }
    void Search() const {
        for (Iterator it = begin_; it != end_; ++it) {
            for (const Piece& piece : pieces_) {
                piece.Match(it, end_);
            }
        }
    }

    bool CheckResult() const {
        bool result = true;
        for (const Piece& piece : pieces_) {
            result = result && piece.CheckResult();
        }
        return result;
    }

    void Apply() const {
        for (const Piece& piece : pieces_) {
            piece.Apply();
        }
    }

private:
    class Piece {
    public:
        Piece(std::string_view pattern, PatternType pattern_type,
              FuncAdditionalMatcher additional_matcher = nullptr,
              FuncResultsChecker results_checker = nullptr,
              FuncApplier applier = nullptr)
            : matcher_(pattern, pattern_type), additional_matcher_{ additional_matcher },
              results_checker_{ results_checker }, applier_{ applier } {
        }
        void Match(Iterator begin, Iterator end) const {
            if (!matcher_.Match(begin, end)) {
                return;
            }
            if (additional_matcher_ == nullptr || additional_matcher_(begin, results_)) {
                results_.push_back(begin);
            }
        }
        void Match(Iterator begin, std::size_t length) const {
            if (!matcher_.Match(begin, length)) {
                return;
            }
            if (additional_matcher_ == nullptr || additional_matcher_(begin, results_)) {
                results_.push_back(begin);
            }
        }
        bool CheckResult() const {
            return results_checker_ == nullptr || results_checker_(results_);
        }
        void Apply() const {
            if (applier_ != nullptr) {
                applier_(results_);
            }
        }

    private:
        const MemMatcher matcher_;
        const FuncAdditionalMatcher additional_matcher_;
        const FuncResultsChecker results_checker_;
        const FuncApplier applier_;

        mutable ResultsType results_;
    };
    std::vector<Piece> pieces_;
    const Iterator begin_;
    const Iterator end_;
};  // MemScanner

}  // namespace utils

#endif  // __UTILS_MEM_SCANNER_H__
