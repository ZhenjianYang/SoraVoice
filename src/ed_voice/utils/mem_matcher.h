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
    bool Match(const std::byte* buff, size_t length);

    inline static MemMatcher StringMatcher(std::string_view pattern) {
        return MemMatcher(pattern, Type::String);
    }

    inline static MemMatcher BytesMatcher(std::string_view pattern) {
        return MemMatcher(pattern, Type::Bytes);
    }

private:
    std::vector<std::pair<std::byte, std::byte>> pattern_;
};  // MemMatcher
}  // namespace utils

#endif  // __UTILS_MEM_MATCHER_H__
