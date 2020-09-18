#ifndef __UTILS_MEM_MATCHER_H__
#define __UTILS_MEM_MATCHER_H__

#include <string_view>
#include <vector>

namespace utils {
class MemMatcher {
public:
    enum class Type {
        String,
        Bytes
    };
    MemMatcher(std::string_view pattern, Type type);

    template<typename Value, std::size_t N>
    bool Match(const Value (&array)[N]) const {
        return Match(array, N);
    }

    template<typename Iterator,
             typename Value = decltype(*std::declval<Iterator>())>
    bool Match(Iterator begin, std::size_t length = std::numeric_limits<std::size_t>::max()) const {
        if (length < pattern_.size()) {
            return false;
        }
        for (std::size_t i = 0; i < pattern_.size(); ++i, ++begin) {
            if (((pattern_[i].first ^ *begin) & pattern_[i].second) != 0) {
                return false;
            }
        }
        return true;
    }

    template<typename Iterator,
             typename Value = decltype(*std::declval<Iterator>())>
    bool Match(Iterator begin, Iterator end) const {
        std::size_t i = 0;
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

private:
    std::vector<std::pair<int, int>> pattern_;
};  // MemMatcher
}  // namespace utils

#endif  // __UTILS_MEM_MATCHER_H__
