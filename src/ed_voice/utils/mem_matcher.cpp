#include "mem_matcher.h"

#include <cassert>
#include <string>

#include <utils/str.h>

namespace {
    constexpr std::byte kZero{ 0x00 };
    constexpr std::byte kAllBits{ 0xFF };
    constexpr std::byte kHi4Bits{ 0xF0 };
    constexpr std::byte kLo4Bits{ 0x0F };
}  // namespace

utils::MemMatcher::MemMatcher(std::string_view pattern, Type type) {
    if (type == Type::String) {
        for (char ch : pattern) {
            if (ch == '?') {
                pattern_.emplace_back(static_cast<std::byte>(ch), kZero);
            } else {
                pattern_.emplace_back(static_cast<std::byte>(ch), kAllBits);
            }
        }
    } else {
        std::vector<std::string> bytes = utils::StrSplit(pattern, ' ');
        for (const std::string& byte : bytes) {
            assert(byte.length() == 2);
            std::byte mask { kAllBits };
            std::byte value{ kZero };
            if (byte[0] == '?') {
                mask &= kLo4Bits;
            } else {
                assert(byte[0] >= '0' && byte[0] <= '9');
                value |= std::byte{ byte[0] - '0' } << 4;
            }
            if (byte[1] == '?') {
                mask &= kHi4Bits;
            } else {
                assert(byte[1] >= '0' && byte[1] <= '9');
                value |= std::byte{ byte[1] - '0' } << 4;
            }
            pattern_.emplace_back(value, mask);
        }
    }
}

bool utils::MemMatcher::Match(const std::byte* buff, size_t length) {
    if (length < pattern_.size()) {
        return false;
    }
    for (size_t i = 0; i <= pattern_.size(); i++) {
        if (((pattern_[i].first ^ buff[i]) & pattern_[i].second) != kZero){
            return false;
        }
    }
    return true;
}
