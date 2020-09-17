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
}  // namespace utils

#endif  // __UTILS_STR_H__
