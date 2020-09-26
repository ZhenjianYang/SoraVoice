#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "utils/str.h"

namespace {
using ::testing::ElementsAre;
using ::utils::StringMatch;
using ::utils::StrSplit;
}  // namespace

TEST(StrTest, StrSplitSkipEmpty) {
    EXPECT_THAT(StrSplit("12#34#56", '#'), ElementsAre("12", "34", "56"));
    EXPECT_THAT(StrSplit("123456", '#'), ElementsAre("123456"));

    EXPECT_THAT(StrSplit("#123456", '#'), ElementsAre("123456"));
    EXPECT_THAT(StrSplit("123456#", '#'), ElementsAre("123456"));
    EXPECT_THAT(StrSplit("123##456", '#'), ElementsAre("123", "456"));
    EXPECT_THAT(StrSplit("###", '#'), ElementsAre());
    EXPECT_THAT(StrSplit("", '#'), ElementsAre());
}

TEST(StrTest, StrSplitKeepEmpty) {
    EXPECT_THAT(StrSplit("12#34#56", '#', false), ElementsAre("12", "34", "56"));
    EXPECT_THAT(StrSplit("123456", '#', false), ElementsAre("123456"));

    EXPECT_THAT(StrSplit("#123456", '#', false), ElementsAre("", "123456"));
    EXPECT_THAT(StrSplit("123456#", '#', false), ElementsAre("123456", ""));
    EXPECT_THAT(StrSplit("123##456", '#', false), ElementsAre("123", "", "456"));
    EXPECT_THAT(StrSplit("###", '#', false), ElementsAre("", "", "", ""));
    EXPECT_THAT(StrSplit("", '#', false), ElementsAre(""));
}

TEST(StrTest, StringMatch) {
    using Char = char;
    using String = std::basic_string<Char>;
    using StringView = std::basic_string_view<Char>;

    const Char str0[] = "1234567890";

    const Char str1[] = "1234567890";
    const Char str2[] = "123456";
    const Char str3[] = "123456A";

    EXPECT_TRUE(StringMatch(str0, str1));
    EXPECT_TRUE(StringMatch(str0, str2));
    EXPECT_FALSE(StringMatch(str0, str3));
    EXPECT_TRUE(StringMatch(str0, str1, true));
    EXPECT_FALSE(StringMatch(str0, str2, true));
    EXPECT_FALSE(StringMatch(str0, str3, true));

    EXPECT_TRUE(StringMatch(str0, String(str1)));
    EXPECT_TRUE(StringMatch(str0, String(str2)));
    EXPECT_FALSE(StringMatch(str0, String(str3)));
    EXPECT_TRUE(StringMatch(str0, String(str1), true));
    EXPECT_FALSE(StringMatch(str0, String(str2), true));
    EXPECT_FALSE(StringMatch(str0, String(str3), true));

    EXPECT_TRUE(StringMatch(str0, StringView(str1)));
    EXPECT_TRUE(StringMatch(str0, StringView(str2)));
    EXPECT_FALSE(StringMatch(str0, StringView(str3)));
    EXPECT_TRUE(StringMatch(str0, StringView(str1), true));
    EXPECT_FALSE(StringMatch(str0, StringView(str2), true));
    EXPECT_FALSE(StringMatch(str0, StringView(str3), true));

    EXPECT_TRUE(StringMatch(str0, (const Char*)str1));
    EXPECT_TRUE(StringMatch(str0, (const Char*)str2));
    EXPECT_FALSE(StringMatch(str0, (const Char*)str3));
    EXPECT_TRUE(StringMatch(str0, (const Char*)str1, true));
    EXPECT_FALSE(StringMatch(str0, (const Char*)str2, true));
    EXPECT_FALSE(StringMatch(str0, (const Char*)str3, true));
}

TEST(StrTest, StringMatchW) {
    using Char = wchar_t;
    using String = std::basic_string<Char>;
    using StringView = std::basic_string_view<Char>;

    const Char str0[] = L"1234567890";

    const Char str1[] = L"1234567890";
    const Char str2[] = L"123456";
    const Char str3[] = L"123456A";

    EXPECT_TRUE(StringMatch(str0, str1));
    EXPECT_TRUE(StringMatch(str0, str2));
    EXPECT_FALSE(StringMatch(str0, str3));
    EXPECT_TRUE(StringMatch(str0, str1, true));
    EXPECT_FALSE(StringMatch(str0, str2, true));
    EXPECT_FALSE(StringMatch(str0, str3, true));

    EXPECT_TRUE(StringMatch(str0, String(str1)));
    EXPECT_TRUE(StringMatch(str0, String(str2)));
    EXPECT_FALSE(StringMatch(str0, String(str3)));
    EXPECT_TRUE(StringMatch(str0, String(str1), true));
    EXPECT_FALSE(StringMatch(str0, String(str2), true));
    EXPECT_FALSE(StringMatch(str0, String(str3), true));

    EXPECT_TRUE(StringMatch(str0, StringView(str1)));
    EXPECT_TRUE(StringMatch(str0, StringView(str2)));
    EXPECT_FALSE(StringMatch(str0, StringView(str3)));
    EXPECT_TRUE(StringMatch(str0, StringView(str1), true));
    EXPECT_FALSE(StringMatch(str0, StringView(str2), true));
    EXPECT_FALSE(StringMatch(str0, StringView(str3), true));

    EXPECT_TRUE(StringMatch(str0, (const Char*)str1));
    EXPECT_TRUE(StringMatch(str0, (const Char*)str2));
    EXPECT_FALSE(StringMatch(str0, (const Char*)str3));
    EXPECT_TRUE(StringMatch(str0, (const Char*)str1, true));
    EXPECT_FALSE(StringMatch(str0, (const Char*)str2, true));
    EXPECT_FALSE(StringMatch(str0, (const Char*)str3, true));
}

