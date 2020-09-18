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

    template<typename Iterator,
             typename Value = decltype(*std::declval<Iterator>())>
    bool Match(Iterator begin, size_t length = std::numeric_limits<size_t>::max()) const {
        if (length < pattern_.size()) {
            return false;
        }
        for (size_t i = 0; i < pattern_.size(); ++i, ++begin) {
            if (((pattern_[i].first ^ *begin) & pattern_[i].second) != 0) {
                return false;
            }
        }
        return true;
    }

    template<typename Iterator,
             typename Value = decltype(*std::declval<Iterator>())>
    bool Match(Iterator begin, Iterator end) const {
        size_t i = 0;
        for (; i < pattern_.size() && begin != end; ++i, ++begin) {
            if (((pattern_[i].first ^ *begin) & pattern_[i].second) != 0) {
                return false;
            }
        }
        return i == pattern_.size();
    }

    template<typename Container,
             typename Value = decltype(*std::begin(std::declval<Container>())),
             typename std::enable_if_t<!std::is_array_v<Container>, int> = 0>
    bool Match(const Container& range) const {
        return Match(std::begin(range), std::end(range));
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
