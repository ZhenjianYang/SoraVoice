#include "mem_matcher.h"

#include <cassert>
#include <string>

#include "utils/str.h"

namespace {
    constexpr int kZero{ 0x00 };
    constexpr int kAllBits{ 0xFF };
    constexpr int kHi4Bits{ 0xF0 };
    constexpr int kLo4Bits{ 0x0F };
}  // namespace

utils::MemMatcher::MemMatcher(std::string_view pattern, Type type) {
    if (type == Type::String) {
        for (char ch : pattern) {
            if (ch == '?') {
                pattern_.emplace_back(kZero, kZero);
            } else {
                pattern_.emplace_back(static_cast<int>(ch), kAllBits);
            }
        }
    } else {
        std::vector<std::string> bytes = utils::StrSplit(pattern, ' ');
        for (const std::string& byte : bytes) {
            assert(byte.length() == 2);
            int mask { kAllBits };
            int value{ kZero };
            if (byte[0] == '?') {
                mask &= kLo4Bits;
            } else {
                if (byte[0] >= '0' && byte[0] <= '9') {
                    value |= static_cast<int>(byte[0] - '0') << 4;
                } else if (byte[0] >= 'A' && byte[0] <= 'F') {
                    value |= static_cast<int>(byte[0] - 'A' + 0x0A) << 4;
                } else if (byte[0] >= 'a' && byte[0] <= 'f') {
                    value |= static_cast<int>(byte[0] - 'a' + 0x0A) << 4;
                } else {
                    assert(false);  // shold not go here
                }
            }
            if (byte[1] == '?') {
                mask &= kHi4Bits;
            } else {
                if (byte[1] >= '0' && byte[1] <= '9') {
                    value |= static_cast<int>(byte[1] - '0');
                } else if (byte[1] >= 'A' && byte[1] <= 'F') {
                    value |= static_cast<int>(byte[1] - 'A' + 0x0A);
                } else if (byte[1] >= 'a' && byte[1] <= 'f') {
                    value |= static_cast<int>(byte[1] - 'a' + 0x0A);
                } else {
                    assert(false);  // shold not go here
                }
            }
            pattern_.emplace_back(value, mask);
        }
    }
}
