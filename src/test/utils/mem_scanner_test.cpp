#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include "utils/mem_scanner.h"

namespace {
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using ::testing::ElementsAre;
using ::testing::Return;
using Iter = const char*;
using MemScanner = ::utils::MemScanner<Iter>;
using IterType = MemScanner::IterType;
using Results = MemScanner::Results;
class MockFuncs {
public:
    MOCK_METHOD(bool, AdditionalMatch, (const Results&, IterType, IterType));
    MOCK_METHOD(bool, CheckResults, (const Results&));
    MOCK_METHOD(bool, Apply, (const Results&));
};
}  // namespace

TEST(MemScannerTest, BasicPiece) {
    const char buff[] = "A0CA1CA2DA3CA4C";
    const char* buff_end = std::end(buff) - 1;

    auto piece = MemScanner::GetBasicPiece("A?C?", MemScanner::PatternType::String);

    piece->Match(buff + 2, buff_end);
    EXPECT_TRUE(piece->AdditionalMatch(buff + 2, buff_end));
    EXPECT_FALSE(piece->CheckResults());
    EXPECT_THAT(piece->GetResults(), ElementsAre());

    piece->Match(buff, buff_end);
    EXPECT_TRUE(piece->AdditionalMatch(buff, buff_end));
    EXPECT_TRUE(piece->CheckResults());
    EXPECT_THAT(piece->GetResults(), ElementsAre(buff));

    piece->Match(buff + 1, buff_end);
    EXPECT_TRUE(piece->AdditionalMatch(buff + 1, buff_end));
    EXPECT_TRUE(piece->CheckResults());
    EXPECT_THAT(piece->GetResults(), ElementsAre(buff));

    piece->Match(buff + 3, buff_end);
    EXPECT_TRUE(piece->AdditionalMatch(buff + 3, buff_end));
    EXPECT_TRUE(piece->CheckResults());
    EXPECT_THAT(piece->GetResults(), ElementsAre(buff, buff + 3));
}


TEST(MemScannerTest, StandardPiece) {
    const char buff[] = "A0CA1CA2DA3CA4C";

    MemScanner scanner1(buff, sizeof(buff) - 1);
    MockFuncs fns1;
    EXPECT_CALL(fns1, CheckResults(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1);
    EXPECT_CALL(fns1, Apply(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1);
    scanner1.AddStandardPiece("A?C?", MemScanner::PatternType::String,
                              nullptr,
                              std::bind(&MockFuncs::CheckResults, &fns1, _1),
                              std::bind(&MockFuncs::Apply, &fns1, _1));
    scanner1.Search();
    scanner1.CheckResult();
    scanner1.Apply();

    MemScanner scanner2(buff, sizeof(buff) - 1);
    MockFuncs fns2;
    EXPECT_CALL(fns2, CheckResults(ElementsAre()))
        .Times(1);
    EXPECT_CALL(fns2, Apply(ElementsAre()))
        .Times(1);
    scanner2.AddStandardPiece("X?X?", MemScanner::PatternType::String,
                              nullptr,
                              std::bind(&MockFuncs::CheckResults, &fns2, _1),
                              std::bind(&MockFuncs::Apply, &fns2, _1));
    scanner2.Search();
    scanner2.CheckResult();
    scanner2.Apply();
}

TEST(MemScannerTest, AdditionalMatch) {
    const char buff[] = "A0CA1CA2DA3CA4C";
    const auto buff_end = std::end(buff) - 1;

    MemScanner scanner1(buff, sizeof(buff) - 1);
    MockFuncs fns1;
    EXPECT_CALL(fns1, AdditionalMatch(ElementsAre(), buff, buff_end))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(fns1, AdditionalMatch(ElementsAre(buff), buff + 3, buff_end))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(fns1, AdditionalMatch(ElementsAre(buff, buff + 3), buff + 9, buff_end))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(fns1, CheckResults(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1);
    EXPECT_CALL(fns1, Apply(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1);
    scanner1.AddStandardPiece("A?C?", MemScanner::PatternType::String,
                              std::bind(&MockFuncs::AdditionalMatch, &fns1, _1, _2, _3),
                              std::bind(&MockFuncs::CheckResults, &fns1, _1),
                              std::bind(&MockFuncs::Apply, &fns1, _1));
    scanner1.Search();
    scanner1.CheckResult();
    scanner1.Apply();

    MemScanner scanner2(buff, sizeof(buff) - 1);
    MockFuncs fns2;
    EXPECT_CALL(fns2, AdditionalMatch(ElementsAre(), buff, buff_end))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(fns2, AdditionalMatch(ElementsAre(buff), buff + 3, buff_end))
        .Times(1)
        .WillRepeatedly(Return(false));
    EXPECT_CALL(fns2, AdditionalMatch(ElementsAre(buff), buff + 9, buff_end))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(fns2, CheckResults(ElementsAre(buff, buff + 9)))
        .Times(1);
    EXPECT_CALL(fns2, Apply(ElementsAre(buff, buff + 9)))
        .Times(1);
    scanner2.AddStandardPiece("A?C?", MemScanner::PatternType::String,
                              std::bind(&MockFuncs::AdditionalMatch, &fns2, _1, _2, _3),
                              std::bind(&MockFuncs::CheckResults, &fns2, _1),
                              std::bind(&MockFuncs::Apply, &fns2, _1));
    scanner2.Search();
    scanner2.CheckResult();
    scanner2.Apply();
}

TEST(MemScannerTest, CheckResults) {
    const char buff[] = "A0CA1CA2DA3CA4C";

    MemScanner scanner1(buff, sizeof(buff) - 1);
    MockFuncs fns11;
    EXPECT_CALL(fns11, CheckResults(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1)
        .WillRepeatedly(Return(true));
    scanner1.AddStandardPiece("A?C?", MemScanner::PatternType::String,
                              nullptr,
                              std::bind(&MockFuncs::CheckResults, &fns11, _1),
                              nullptr);
    MockFuncs fns12;
    EXPECT_CALL(fns12, CheckResults(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1)
        .WillRepeatedly(Return(true));
    scanner1.AddStandardPiece("A?C?", MemScanner::PatternType::String,
                              nullptr,
                              std::bind(&MockFuncs::CheckResults, &fns12, _1),
                              nullptr);
    scanner1.Search();
    EXPECT_TRUE(scanner1.CheckResult());

    MemScanner scanner2(buff, sizeof(buff) - 1);
    MockFuncs fns21;
    EXPECT_CALL(fns21, CheckResults(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1)
        .WillRepeatedly(Return(true));
    scanner2.AddStandardPiece("A?C?", MemScanner::PatternType::String,
                              nullptr,
                              std::bind(&MockFuncs::CheckResults, &fns21, _1),
                              nullptr);
    MockFuncs fns22;
    EXPECT_CALL(fns22, CheckResults(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1)
        .WillRepeatedly(Return(false));
    scanner2.AddStandardPiece("A?C?", MemScanner::PatternType::String,
                              nullptr,
                              std::bind(&MockFuncs::CheckResults, &fns22, _1),
                              nullptr);
    scanner2.Search();
    EXPECT_FALSE(scanner2.CheckResult());
}

TEST(MemScannerTest, CheckResultsIsNull) {
    const char buff[] = "A0CA1CA2DA3CA4C";

    MemScanner scanner(buff, sizeof(buff) - 1);
    MockFuncs fns1;
    EXPECT_CALL(fns1, Apply(ElementsAre(buff, buff + 3, buff + 9)))
        .Times(1);
    scanner.AddStandardPiece("A?C?", MemScanner::PatternType::String,
                             nullptr,
                             nullptr,
                             std::bind(&MockFuncs::Apply, &fns1, _1));
    MockFuncs fns2;
    EXPECT_CALL(fns2, Apply(ElementsAre()))
        .Times(0);
    scanner.AddStandardPiece("Y?Y?", MemScanner::PatternType::String,
                             nullptr,
                             nullptr,
                             std::bind(&MockFuncs::Apply, &fns2, _1));
    scanner.Search();
    EXPECT_TRUE(scanner.CheckResult());
    scanner.Apply();
}
