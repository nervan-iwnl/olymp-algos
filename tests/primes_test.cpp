#include "number_theory/primes.hpp"

#include <gtest/gtest.h>

#include <vector>

namespace {

bool is_prime(int value) {
    if (value < 2) return false;
    for (int divisor = 2; divisor * divisor <= value; ++divisor) {
        if (value % divisor == 0) return false;
    }
    return true;
}

std::vector<int> naive_primes(int limit) {
    std::vector<int> result;
    for (int value = 2; value < limit; ++value) {
        if (is_prime(value)) result.push_back(value);
    }
    return result;
}

}  // namespace

TEST(Sieve, HandlesBoundariesAndKnownPrimes) {
    EXPECT_TRUE(sieve(0).empty());
    EXPECT_TRUE(sieve(2).empty());
    EXPECT_EQ(sieve(3), (std::vector<int>{2}));
    EXPECT_EQ(sieve(20), (std::vector<int>{2, 3, 5, 7, 11, 13, 17, 19}));
}

TEST(Sieve, MatchesTrialDivisionForEverySmallLimit) {
    for (int limit = 0; limit <= 1000; ++limit) {
        SCOPED_TRACE(limit);
        EXPECT_EQ(sieve(limit), naive_primes(limit));
    }
}
