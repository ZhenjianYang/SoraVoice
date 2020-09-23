#include "mem_matcher.h"

#include <cassert>
#include <string>

#include "utils/str.h"

namespace {
constexpr ::utils::MemMatcher::InternalValueType kZero = 0;
constexpr ::utils::MemMatcher::InternalValueType kAllBits = 0xFF;
constexpr ::utils::MemMatcher::InternalValueType kHalfByte = 0xF;
constexpr std::size_t kBitsHalfByte = 4;
constexpr std::size_t kNumHalfBytes = 2;
}  // namespace

utils::MemMatcher::MemMatcher(std::string_view pattern, Type type)
        : pattern_str_{ pattern }, type_{ type } {
    if (type_ == Type::String) {
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
            assert(byte.length() == kNumHalfBytes);
            InternalValueType mask  = kZero;
            InternalValueType value = kZero;
            for (char half : byte) {
                mask  <<= kBitsHalfByte;
                value <<= kBitsHalfByte;
                if (half != '?') {
                    mask |= kHalfByte;
                    if (half >= '0' && half <= '9') {
                        value |= static_cast<InternalValueType>(half - '0');
                    } else if (half >= 'A' && half <= 'F') {
                        value |= static_cast<InternalValueType>(half - 'A' + 0x0A);
                    } else if (half >= 'a' && half <= 'f') {
                        value |= static_cast<InternalValueType>(half - 'a' + 0x0A);
                    } else {
                        assert(false);  // shold not go here
                    }
                }
            }
            pattern_.emplace_back(value, mask);
        }
    }
}
