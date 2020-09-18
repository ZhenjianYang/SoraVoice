#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include "utils/mem_matcher.h"

namespace {
using ::utils::MemMatcher;
}  // namespace

TEST(MemMatcherTest, IsSameMatcher) {
    // part 1
    auto matcher1 = MemMatcher::StringMatcher("ABCD");
    auto matcher2 = MemMatcher("ABCD", MemMatcher::Type::String);
    EXPECT_EQ(matcher1, matcher2);

    matcher1 = MemMatcher::StringMatcher("A??B");
    matcher2 = MemMatcher("A??B", MemMatcher::Type::String);
    EXPECT_EQ(matcher1, matcher2);

    matcher1 = MemMatcher::StringMatcher("??");
    matcher2 = MemMatcher("??", MemMatcher::Type::String);
    EXPECT_EQ(matcher1, matcher2);

    // part 2
    matcher1 = MemMatcher::BytesMatcher("41 42 43 44");
    matcher2 = MemMatcher("41 42 43 44", MemMatcher::Type::Bytes);
    EXPECT_EQ(matcher1, matcher2);

    matcher1 = MemMatcher::BytesMatcher("41 4? 43 ?4");
    matcher2 = MemMatcher("41 4? 43 ?4", MemMatcher::Type::Bytes);
    EXPECT_EQ(matcher1, matcher2);

    matcher1 = MemMatcher::BytesMatcher("?? ?? ?? ??");
    matcher2 = MemMatcher("?? ?? ?? ??", MemMatcher::Type::Bytes);
    EXPECT_EQ(matcher1, matcher2);

    matcher1 = MemMatcher::BytesMatcher("Ab 1C d2 E? ?f");
    matcher2 = MemMatcher("aB 1c D2 e? ?F", MemMatcher::Type::Bytes);
    EXPECT_EQ(matcher1, matcher2);

    // part 3
    matcher1 = MemMatcher::StringMatcher("ABCD");
    matcher2 = MemMatcher::BytesMatcher("41 42 43 44");
    EXPECT_EQ(matcher1, matcher2);

    matcher1 = MemMatcher::StringMatcher("A?C?");
    matcher2 = MemMatcher::BytesMatcher("41 ?? 43 ??");
    EXPECT_EQ(matcher1, matcher2);

    matcher1 = MemMatcher::StringMatcher("????");
    matcher2 = MemMatcher::BytesMatcher("?? ?? ?? ??");
    EXPECT_EQ(matcher1, matcher2);
}


TEST(MemMatcherTest, InputIsArray) {
    const char array[] = { 'A', 'B', 'C', 'D', 'E', 'F' };
    auto matcher = MemMatcher::StringMatcher("ABCD");

    EXPECT_TRUE(matcher.Match(array));
    EXPECT_TRUE(matcher.Match(array, array + 4));
    EXPECT_TRUE(matcher.Match(array, 4));
    EXPECT_FALSE(matcher.Match(array, array + 3));
    EXPECT_FALSE(matcher.Match(array, 3));
    EXPECT_FALSE(matcher.Match(array + 1, array + 1 + 4));
    EXPECT_FALSE(matcher.Match(array + 1, 4));
    EXPECT_FALSE(matcher.Match(array + 1));

    EXPECT_TRUE(MemMatcher::StringMatcher("ABCDEF").Match(array));
    EXPECT_FALSE(MemMatcher::StringMatcher("ABCDEF?").Match(array));
}

TEST(MemMatcherTest, InputIsContainer) {
    const std::vector<char> container{ 'A', 'B', 'C', 'D', 'E', 'F' };
    auto matcher = MemMatcher::StringMatcher("ABCD");

    EXPECT_TRUE(matcher.Match(container));
    EXPECT_TRUE(matcher.Match(container.begin()));
    EXPECT_TRUE(matcher.Match(container.begin(), container.end()));
    EXPECT_TRUE(matcher.Match(container.begin(), container.begin() + 4));
    EXPECT_TRUE(matcher.Match(container.begin(), 4));
    EXPECT_FALSE(matcher.Match(container.begin(), container.begin() + 3));
    EXPECT_FALSE(matcher.Match(container.begin(), 3));
    EXPECT_FALSE(matcher.Match(container.begin() + 1, container.begin() + 1 + 4));
    EXPECT_FALSE(matcher.Match(container.begin() + 1, 4));
    EXPECT_FALSE(matcher.Match(container.begin() + 1));

    EXPECT_TRUE(MemMatcher::StringMatcher("ABCDEF").Match(container));
    EXPECT_FALSE(MemMatcher::StringMatcher("ABCDEF?").Match(container));
}

TEST(MemMatcherTest, StringMatcher) {
    auto matcher = MemMatcher::StringMatcher("ABCD");
    EXPECT_TRUE(matcher.Match("ABCD", 4));
    EXPECT_FALSE(matcher.Match("ABCD", 3));
    EXPECT_FALSE(matcher.Match("ABCE", 4));

    matcher = MemMatcher::StringMatcher("AB?D");
    EXPECT_TRUE(matcher.Match("ABCD", 4));
    EXPECT_TRUE(matcher.Match("ABDD", 4));
    EXPECT_FALSE(matcher.Match("ACCD", 4));

    matcher = MemMatcher::StringMatcher("?B?D");
    EXPECT_TRUE(matcher.Match("ABCD", 4));
    EXPECT_TRUE(matcher.Match("ZB\0D", 4));
    EXPECT_FALSE(matcher.Match("ACCD", 4));
    EXPECT_FALSE(matcher.Match("ABCE", 4));

    matcher = MemMatcher::StringMatcher("");
    EXPECT_TRUE(matcher.Match("ABCD", 4));
    EXPECT_TRUE(matcher.Match("ABDD", 4));
    EXPECT_TRUE(matcher.Match("AB\0D", 4));
}

TEST(MemMatcherTest, BytesMatcher) {
    auto matcher = MemMatcher::BytesMatcher("49 4A 4B 4C");
    EXPECT_TRUE(matcher.Match("\x49\x4A\x4B\x4C", 4));
    EXPECT_FALSE(matcher.Match("\x49\x4A\x4B\x4C", 3));
    EXPECT_FALSE(matcher.Match("\x49\x4A\x4F\x4C", 4));

    matcher = MemMatcher::BytesMatcher("49 4A ?? 4C");
    EXPECT_TRUE(matcher.Match("\x49\x4A\x4B\x4C", 4));
    EXPECT_TRUE(matcher.Match("\x49\x4A\x3D\x4C", 4));
    EXPECT_FALSE(matcher.Match("\x49\x4B\x4B\x4C", 4));
    EXPECT_FALSE(matcher.Match("\x49\x5A\x4B\x4C", 4));

    matcher = MemMatcher::BytesMatcher("49 4A ?B 4C");
    EXPECT_TRUE(matcher.Match("\x49\x4A\x4B\x4C", 4));
    EXPECT_TRUE(matcher.Match("\x49\x4A\x3B\x4C", 4));
    EXPECT_FALSE(matcher.Match("\x4A\x4A\x4B\x4C", 4));
    EXPECT_FALSE(matcher.Match("\x49\x4A\x4C\x4C", 4));

    matcher = MemMatcher::BytesMatcher("?? 4A ?B 4C");
    EXPECT_TRUE(matcher.Match("\x49\x4A\x4B\x4C", 4));
    EXPECT_TRUE(matcher.Match("\x7B\x4A\x3B\x4C", 4));
    EXPECT_FALSE(matcher.Match("\x4A\x4A\x4B\x5C", 4));
    EXPECT_FALSE(matcher.Match("\x5C\x4A\x4C\x4C", 4));
}
