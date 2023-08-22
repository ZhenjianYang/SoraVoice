#ifndef __UTILS_MEM_SCANNER_H__
#define __UTILS_MEM_SCANNER_H__

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

#include "utils/mem_matcher.h"

namespace utils {
template<typename Iterator>
class MemScanner {
public:
    using IterType = Iterator;
    using PatternType = MemMatcher::Type;
    using Results = std::vector<Iterator>;
    using FunAdditionalMatch = std::function<bool(const Results&, Iterator, Iterator)>;
    using FunCheckResults = std::function<bool(const Results&)>;
    using FunApply = std::function<bool(const Results&)>;

    class Piece {
    public:
        virtual void Match(Iterator begin, Iterator end) {
            if (BasicMatch(begin, end) && AdditionalMatch(begin, end)) {
                GetResultsWritable().push_back(begin);
            }
        }
        virtual const Results& GetResults() const = 0;
        virtual bool BasicMatch(Iterator begin, Iterator end) const = 0;
        virtual bool AdditionalMatch(Iterator /*begin*/, Iterator /*end*/) const {
            return true;
        }
        virtual bool CheckResults() const {
            return !GetResults().empty();
        }
        virtual bool Apply() const {
            return true;
        }
        virtual ~Piece() = default;
    protected:
        virtual Results& GetResultsWritable() = 0;
    };  // Piece

    class BasicPiece : public Piece {
    public:
        BasicPiece(std::string_view pattern, PatternType pattern_type)
            : matcher_(pattern, pattern_type) {
        }
        const Results& GetResults() const override {
            return results_;
        }
        Results& GetResultsWritable() override {
            return results_;
        }
        bool BasicMatch(Iterator begin, Iterator end) const override {
            return matcher_.Match(begin, end);
        }

    protected:
        const MemMatcher matcher_;
        Results results_;
    };  // BasicPiece

    class StandardPiece : public BasicPiece {
    public:
        StandardPiece(std::string_view pattern, PatternType pattern_type,
                      FunAdditionalMatch fun_additional_match = nullptr,
                      FunCheckResults fun_results_check = nullptr,
                      FunApply fun_apply = nullptr)
            : BasicPiece(pattern, pattern_type),
              fun_additional_match_{ fun_additional_match },
              fun_results_check_{ fun_results_check },
              fun_apply_{ fun_apply } {
        }

        bool AdditionalMatch(Iterator begin, Iterator end) const override {
            return fun_additional_match_ == nullptr || fun_additional_match_(results_, begin, end);
        }
        bool CheckResults() const override {
            return fun_results_check_ == nullptr || fun_results_check_(results_);
        }
        bool Apply() const override {
            if (fun_apply_ != nullptr) {
                return fun_apply_(results_);
            }
            return true;
        }

    protected:
        const FunAdditionalMatch fun_additional_match_ = nullptr;
        const FunCheckResults fun_results_check_ = nullptr;
        const FunApply fun_apply_ = nullptr;
    };  // StandardPiece

    MemScanner(Iterator begin, int length)
        : begin_{ begin }, end_{ begin + length } {
    }
    MemScanner(Iterator begin, Iterator end)
        : begin_{ begin }, end_{ end } {
    }

    void AddPiece(Piece* piece) {
        pieces_.push_back(piece);
    }
    void AddPiece(const std::unique_ptr<Piece>& piece) {
        pieces_.push_back(piece.get());
    }
    void AddPiece(std::unique_ptr<Piece>&& piece) {
        pieces_.push_back(piece.get());
        pieces_hold_.push_back(std::move(piece));
    }
    void AddBasicPiece(std::string_view pattern, PatternType pattern_type) {
        AddPiece(GetBasicPiece(pattern, pattern_type));
    }
    void AddStandardPiece(std::string_view pattern, PatternType pattern_type,
                          FunAdditionalMatch fun_additional_match = nullptr,
                          FunCheckResults fun_results_check = nullptr,
                          FunApply fun_apply = nullptr) {
        AddPiece(GetStandardPiece(pattern, pattern_type,
                                  fun_additional_match, fun_results_check, fun_apply));
    }
    void Search() const {
        for (Iterator it = begin_; it != end_; ++it) {
            for (const auto piece : pieces_) {
                piece->Match(it, end_);
            }
        }
    }

    bool CheckResult() const {
        bool result = true;
        for (const auto piece : pieces_) {
            result = result && piece->CheckResults();
        }
        return result;
    }

    bool Apply() const {
        bool result = true;
        for (const auto piece : pieces_) {
            result = result && piece->Apply();
        }
        return result;
    }

    static std::unique_ptr<Piece> GetBasicPiece(std::string_view pattern, PatternType pattern_type) {
        return std::make_unique<BasicPiece>(pattern, pattern_type);
    }

    static std::unique_ptr<Piece> GetStandardPiece(std::string_view pattern, PatternType pattern_type,
                                                   FunAdditionalMatch fun_additional_match = nullptr,
                                                   FunCheckResults fun_results_check = nullptr,
                                                   FunApply fun_apply = nullptr) {
        return std::make_unique<StandardPiece>(pattern, pattern_type,
                                               fun_additional_match, fun_results_check, fun_apply);
    }

protected:
    std::vector<Piece*> pieces_;
    std::vector<std::unique_ptr<Piece>> pieces_hold_;
    const Iterator begin_;
    const Iterator end_;
};  // MemScanner

}  // namespace utils

#endif  // __UTILS_MEM_SCANNER_H__
