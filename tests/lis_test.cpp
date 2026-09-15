#include "dp/lis.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <vector>

namespace {

int quadratic_lis(const std::vector<int>& values) {
    std::vector<int> best(values.size(), 1);
    int answer = 0;
    for (int i = 0; i < static_cast<int>(values.size()); ++i) {
        for (int j = 0; j < i; ++j) {
            if (values[j] < values[i]) {
                best[i] = std::max(best[i], best[j] + 1);
            }
        }
        answer = std::max(answer, best[i]);
    }
    return answer;
}

}  // namespace

TEST(LIS, HandlesEdgeCasesAndStrictOrdering) {
    EXPECT_EQ(lis({}), 0);
    EXPECT_EQ(lis({1, 2, 3, 4}), 4);
    EXPECT_EQ(lis({4, 3, 2, 1}), 1);
    EXPECT_EQ(lis({2, 2, 2}), 1);
    EXPECT_EQ(lis({10, 9, 2, 5, 3, 7, 101, 18}), 4);
}

TEST(LIS, MatchesQuadraticDynamicProgrammingOnRandomArrays) {
    std::mt19937 generator(0x115115);
    std::uniform_int_distribution<int> length(0, 50);
    std::uniform_int_distribution<int> value(-20, 20);

    for (int trial = 0; trial < 500; ++trial) {
        SCOPED_TRACE(trial);
        std::vector<int> values(length(generator));
        for (int& item : values) item = value(generator);
        EXPECT_EQ(lis(values), quadratic_lis(values));
    }
}
