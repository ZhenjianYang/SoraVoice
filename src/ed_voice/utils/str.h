#ifndef __UTILS_STR_H__
#define __UTILS_STR_H__

#include <string_view>
#include <vector>

namespace utils {
inline static std::vector<std::string> StrSplit(std::string_view s, char delimiter) {
    std::vector<std::string> splits;
    size_t pos = 0;
    while (pos < s.length()) {
        size_t pos_next = s.find(delimiter, pos);
        splits.emplace_back(s.substr(pos, pos_next - pos));
        if (pos_next == std::string_view::npos) {
            break;
        }
        pos = pos_next + 1;
    }
    return splits;
}
}  // namespace utils

#endif  // __UTILS_STR_H__
