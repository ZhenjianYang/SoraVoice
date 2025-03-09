#ifndef __UTILS_MEM_MATCHER_H__
#define __UTILS_MEM_MATCHER_H__

#include <string_view>
#include <vector>
#include <string>

namespace utils {
class MemMatcher {
public:
    enum class Type {
        String,
        Bytes
    };
    MemMatcher(std::string_view pattern, Type type);

    Type GetType() const {
        return type_;
    }
    const std::string& Pattern() const {
        return pattern_str_;
    }

    template<typename Value, int N>
    bool Match(const Value (&array)[N]) const {
        return Match(array, N);
    }

    template<typename Iterator,
             typename Value = decltype(*std::declval<Iterator>())>
    bool Match(Iterator begin, int length = std::numeric_limits<int>::max()) const {
        if (length < pattern_.size()) {
            return false;
        }
        for (int i = 0; i < pattern_.size(); ++i, ++begin) {
            if (((pattern_[i].first ^ *begin) & pattern_[i].second) != 0) {
                return false;
            }
        }
        return true;
    }

    template<typename Iterator,
             typename Value = decltype(*std::declval<Iterator>())>
    bool Match(Iterator begin, Iterator end) const {
        int i = 0;
        for (; i < pattern_.size() && begin != end; ++i, ++begin) {
            if (((pattern_[i].first ^ *begin) & pattern_[i].second) != 0) {
                return false;
            }
        }
        return i == pattern_.size();
    }

    template<typename Container,
             typename Value = decltype(*std::begin(std::declval<Container>()))>
    bool Match(const Container& container) const {
        return Match(std::begin(container), std::end(container));
    }

    inline static MemMatcher StringMatcher(std::string_view pattern) {
        return MemMatcher(pattern, Type::String);
    }

    inline static MemMatcher BytesMatcher(std::string_view pattern) {
        return MemMatcher(pattern, Type::Bytes);
    }

    bool operator==(const MemMatcher& other) const {
        return this->pattern_ == other.pattern_;
    }

    using InternalValueType = int;
private:
    std::string pattern_str_;
    Type type_;
    std::vector<std::pair<InternalValueType, InternalValueType>> pattern_;
};  // MemMatcher
}  // namespace utils

#endif  // __UTILS_MEM_MATCHER_H__
