#include "strings/kmp.hpp"

#include <gtest/gtest.h>

#include <random>
#include <string>
#include <vector>

namespace {

std::vector<int> naive_search(const std::string& text, const std::string& pattern) {
    std::vector<int> positions;
    if (pattern.empty()) return positions;
    for (int i = 0;
         i + static_cast<int>(pattern.size()) <= static_cast<int>(text.size());
         ++i) {
        if (text.compare(i, pattern.size(), pattern) == 0) positions.push_back(i);
    }
    return positions;
}

std::vector<int> naive_prefix_function(const std::string& text) {
    std::vector<int> result(text.size(), 0);
    for (int i = 0; i < static_cast<int>(text.size()); ++i) {
        for (int length = 1; length <= i; ++length) {
            if (text.substr(0, length) == text.substr(i - length + 1, length)) {
                result[i] = length;
            }
        }
    }
    return result;
}

std::string random_string(std::mt19937& generator, int length) {
    std::uniform_int_distribution<int> letter(0, 2);
    std::string result(length, 'a');
    for (char& chr : result) chr = static_cast<char>('a' + letter(generator));
    return result;
}

}  // namespace

TEST(KMP, FindsKnownAndOverlappingMatches) {
    EXPECT_EQ(kmp_search("ababa", "aba"), (std::vector<int>{0, 2}));
    EXPECT_EQ(kmp_search("aaaa", "aa"), (std::vector<int>{0, 1, 2}));
    EXPECT_TRUE(kmp_search("abcdef", "xyz").empty());
    EXPECT_TRUE(kmp_search("abc", "").empty());
}

TEST(KMP, MatchesNaiveAlgorithmsOnRandomStrings) {
    std::mt19937 generator(0x4B4D50);
    std::uniform_int_distribution<int> text_length(0, 35);
    std::uniform_int_distribution<int> pattern_length(0, 8);

    for (int trial = 0; trial < 500; ++trial) {
        SCOPED_TRACE(trial);
        std::string text = random_string(generator, text_length(generator));
        std::string pattern = random_string(generator, pattern_length(generator));
        EXPECT_EQ(prefix_function(text), naive_prefix_function(text));
        EXPECT_EQ(kmp_search(text, pattern), naive_search(text, pattern));
    }
}
