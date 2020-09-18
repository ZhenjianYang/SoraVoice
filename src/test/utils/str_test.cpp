#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "utils/str.h"

namespace {
using std::string;
using ::testing::ElementsAre;
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
