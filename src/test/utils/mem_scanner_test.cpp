#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include "utils/mem_scanner.h"

namespace {
using std::placeholders::_1;
using std::placeholders::_2;
using ::testing::ElementsAre;
using ::testing::Return;
using Iter = const char*;
using MemScanner = ::utils::MemScanner<Iter>;
using IterType = MemScanner::IterType;
using ResultsType = MemScanner::ResultsType;
class MockFuncs {
public:
    MOCK_METHOD(bool, AdditionalMatcher, (IterType, const ResultsType&));
    MOCK_METHOD(bool, ResultsChecker, (const ResultsType&));
    MOCK_METHOD(void, Applier, (const ResultsType&));
};
}  // namespace

TEST(MemScannerTest, MatchedResults) {
    const char buff[] = "A0CA1CA2DA3CA4C";

    MemScanner scanner1(buff, sizeof(buff) - 1);
    MockFuncs fns1;
    EXPECT_CALL(fns1, ResultsChecker(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1);
    EXPECT_CALL(fns1, Applier(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1);
    scanner1.AddPiece("A?C?", MemScanner::PatternType::String,
                      nullptr,
                      std::bind(&MockFuncs::ResultsChecker, &fns1, _1),
                      std::bind(&MockFuncs::Applier, &fns1, _1));
    scanner1.Search();
    scanner1.CheckResult();
    scanner1.Apply();

    MemScanner scanner2(buff, sizeof(buff) - 1);
    MockFuncs fns2;
    EXPECT_CALL(fns2, ResultsChecker(ElementsAre()))
        .Times(1);
    EXPECT_CALL(fns2, Applier(ElementsAre()))
        .Times(1);
    scanner2.AddPiece("X?X?", MemScanner::PatternType::String,
                      nullptr,
                      std::bind(&MockFuncs::ResultsChecker, &fns2, _1),
                      std::bind(&MockFuncs::Applier, &fns2, _1));
    scanner2.Search();
    scanner2.CheckResult();
    scanner2.Apply();
}

TEST(MemScannerTest, AdditionalMatcher) {
    const char buff[] = "A0CA1CA2DA3CA4C";

    MemScanner scanner1(buff, sizeof(buff) - 1);
    MockFuncs fns1;
    EXPECT_CALL(fns1, AdditionalMatcher(buff, ElementsAre()))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(fns1, AdditionalMatcher(buff + 3, ElementsAre(buff)))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(fns1, AdditionalMatcher(buff + 9, ElementsAre(buff, buff + 3)))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(fns1, ResultsChecker(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1);
    EXPECT_CALL(fns1, Applier(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1);
    scanner1.AddPiece("A?C?", MemScanner::PatternType::String,
        std::bind(&MockFuncs::AdditionalMatcher, &fns1, _1, _2),
        std::bind(&MockFuncs::ResultsChecker, &fns1, _1),
        std::bind(&MockFuncs::Applier, &fns1, _1));
    scanner1.Search();
    scanner1.CheckResult();
    scanner1.Apply();

    MemScanner scanner2(buff, sizeof(buff) - 1);
    MockFuncs fns2;
    EXPECT_CALL(fns2, AdditionalMatcher(buff, ElementsAre()))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(fns2, AdditionalMatcher(buff + 3, ElementsAre(buff)))
        .Times(1)
        .WillRepeatedly(Return(false));
    EXPECT_CALL(fns2, AdditionalMatcher(buff + 9, ElementsAre(buff)))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(fns2, ResultsChecker(ElementsAre(buff, buff + 9)))
        .Times(1);
    EXPECT_CALL(fns2, Applier(ElementsAre(buff, buff + 9)))
        .Times(1);
    scanner2.AddPiece("A?C?", MemScanner::PatternType::String,
        std::bind(&MockFuncs::AdditionalMatcher, &fns2, _1, _2),
        std::bind(&MockFuncs::ResultsChecker, &fns2, _1),
        std::bind(&MockFuncs::Applier, &fns2, _1));
    scanner2.Search();
    scanner2.CheckResult();
    scanner2.Apply();
}

TEST(MemScannerTest, ResultsChecker) {
    const char buff[] = "A0CA1CA2DA3CA4C";

    MemScanner scanner1(buff, sizeof(buff) - 1);
    MockFuncs fns11;
    EXPECT_CALL(fns11, ResultsChecker(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1)
        .WillRepeatedly(Return(true));
    scanner1.AddPiece("A?C?", MemScanner::PatternType::String,
        nullptr,
        std::bind(&MockFuncs::ResultsChecker, &fns11, _1),
        nullptr);
    MockFuncs fns12;
    EXPECT_CALL(fns12, ResultsChecker(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1)
        .WillRepeatedly(Return(true));
    scanner1.AddPiece("A?C?", MemScanner::PatternType::String,
        nullptr,
        std::bind(&MockFuncs::ResultsChecker, &fns12, _1),
        nullptr);
    scanner1.Search();
    EXPECT_TRUE(scanner1.CheckResult());

    MemScanner scanner2(buff, sizeof(buff) - 1);
    MockFuncs fns21;
    EXPECT_CALL(fns21, ResultsChecker(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1)
        .WillRepeatedly(Return(true));
    scanner2.AddPiece("A?C?", MemScanner::PatternType::String,
        nullptr,
        std::bind(&MockFuncs::ResultsChecker, &fns21, _1),
        nullptr);
    MockFuncs fns22;
    EXPECT_CALL(fns22, ResultsChecker(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1)
        .WillRepeatedly(Return(false));
    scanner2.AddPiece("A?C?", MemScanner::PatternType::String,
        nullptr,
        std::bind(&MockFuncs::ResultsChecker, &fns22, _1),
        nullptr);
    scanner2.Search();
    EXPECT_FALSE(scanner2.CheckResult());
}

TEST(MemScannerTest, ResultsCheckerIsNull) {
    const char buff[] = "A0CA1CA2DA3CA4C";

    MemScanner scanner(buff, sizeof(buff) - 1);
    MockFuncs fns1;
    EXPECT_CALL(fns1, Applier(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1);
    scanner.AddPiece("A?C?", MemScanner::PatternType::String,
        nullptr,
        nullptr,
        std::bind(&MockFuncs::Applier, &fns1, _1));
    MockFuncs fns2;
    EXPECT_CALL(fns2, Applier(ElementsAre()))
        .Times(1);
    scanner.AddPiece("Y?Y?", MemScanner::PatternType::String,
        nullptr,
        nullptr,
        std::bind(&MockFuncs::Applier, &fns2, _1));
    scanner.Search();
    EXPECT_TRUE(scanner.CheckResult());
    scanner.Apply();
}
