#ifndef __UTILS_STR_H__
#define __UTILS_STR_H__

#include <string_view>
#include <vector>

namespace utils {
inline static std::vector<std::string> StrSplit(std::string_view s, char delimiter, bool skip_empty = true) {
    if (s.empty() && !skip_empty) {
        return { "" };
    }

    std::vector<std::string> splits;
    size_t pos = 0;
    while (pos < s.length()) {
        size_t pos_next = s.find(delimiter, pos);
        if (!skip_empty || pos_next > pos) {
            splits.emplace_back(s.substr(pos, pos_next - pos));
        }
        if (pos_next == std::string_view::npos) {
            break;
        } else if (pos_next + 1 == s.length() && !skip_empty) {
            splits.emplace_back("");
        }
        pos = pos_next + 1;
    }
    return splits;
}

template<typename String>
inline static bool StringMatch(const void* p, const String& s, bool null_termanate = false) {
    using Char = typename std::remove_reference_t<decltype(s[0])>;
    Char* pc = reinterpret_cast<Char*>(p);
    if constexpr (std::is_pointer_v<String>) {
        int len = std::char_traits<Char>::length(s);
        return std::equal(pc, pc + len, s)
               && (!null_termanate || *(pc + len) == 0);
    } else if (std::is_array_v<String>) {
        int len = std::size(s) - 1;
        return std::equal(pc, pc + len, std::begin(s))
               && (!null_termanate || *(pc + len) == 0);
    } else {
        int len = std::size(s);
        return std::equal(pc, pc + len, std::begin(s))
               && (!null_termanate || *(pc + len) == 0);
    }
}

}  // namespace utils

#endif  // __UTILS_STR_H__
