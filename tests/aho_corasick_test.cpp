#include "strings/aho_corasick.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace {

using MatchTuple = std::tuple<int, int, int>;

std::vector<MatchTuple> naive_matches(
    const std::vector<std::string>& patterns,
    const std::string& text) {
    std::vector<MatchTuple> result;
    for (int id = 0; id < static_cast<int>(patterns.size()); ++id) {
        const std::string& pattern = patterns[id];
        for (int start = 0;
             start + static_cast<int>(pattern.size()) <=
                 static_cast<int>(text.size());
             ++start) {
            if (text.compare(start, pattern.size(), pattern) == 0) {
                result.emplace_back(
                    id,
                    start,
                    start + static_cast<int>(pattern.size()) - 1);
            }
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::string random_string(std::mt19937& generator, int length) {
    std::uniform_int_distribution<int> letter(0, 2);
    std::string result(length, 'a');
    for (char& chr : result) {
        chr = static_cast<char>('a' + letter(generator));
    }
    return result;
}

}  // namespace

TEST(AhoCorasick, FindsKnownMatchesAndPositions) {
    AhoCorasick aho;
    std::vector<std::string> patterns = {"he", "she", "his", "hers"};
    for (const std::string& pattern : patterns) {
        aho.add_pattern(pattern);
    }
    aho.build();

    std::vector<MatchTuple> actual;
    for (const auto& match : aho.find_all("ushers")) {
        actual.emplace_back(
            match.pattern_id,
            match.start_position,
            match.end_position);
    }
    std::sort(actual.begin(), actual.end());

    EXPECT_EQ(actual, naive_matches(patterns, "ushers"));
    EXPECT_EQ(
        aho.count_occurrences("ushers"),
        (std::vector<long long>{1, 1, 0, 1}));
}

TEST(AhoCorasick, HandlesOverlapsAndDuplicatePatterns) {
    AhoCorasick aho;
    std::vector<std::string> patterns = {"a", "aa", "aaa", "aa"};
    for (const std::string& pattern : patterns) {
        aho.add_pattern(pattern);
    }
    aho.build();

    EXPECT_EQ(
        aho.count_occurrences("aaaa"),
        (std::vector<long long>{4, 3, 2, 3}));
}

TEST(AhoCorasick, EnforcesBuildLifecycleAndAlphabet) {
    AhoCorasick aho;
    EXPECT_THROW(aho.add_pattern(""), std::invalid_argument);
    EXPECT_THROW(aho.add_pattern("A"), std::invalid_argument);
    EXPECT_THROW(aho.find_all("abc"), std::logic_error);

    aho.add_pattern("abc");
    aho.build();
    EXPECT_THROW(aho.build(), std::logic_error);
    EXPECT_THROW(aho.add_pattern("bc"), std::logic_error);
    EXPECT_THROW(aho.find_all("abC"), std::invalid_argument);
}

TEST(AhoCorasick, MatchesNaiveSearchOnRandomInputs) {
    std::mt19937 generator(0xA110C0DE);
    std::uniform_int_distribution<int> pattern_count(1, 7);
    std::uniform_int_distribution<int> pattern_length(1, 5);
    std::uniform_int_distribution<int> text_length(0, 30);

    for (int trial = 0; trial < 400; ++trial) {
        SCOPED_TRACE(trial);
        std::vector<std::string> patterns;
        for (int i = 0; i < pattern_count(generator); ++i) {
            patterns.push_back(random_string(generator, pattern_length(generator)));
        }
        std::string text = random_string(generator, text_length(generator));

        AhoCorasick aho;
        for (const std::string& pattern : patterns) {
            aho.add_pattern(pattern);
        }
        aho.build();

        std::vector<MatchTuple> actual;
        for (const auto& match : aho.find_all(text)) {
            actual.emplace_back(
                match.pattern_id,
                match.start_position,
                match.end_position);
        }
        std::sort(actual.begin(), actual.end());
        const auto expected = naive_matches(patterns, text);
        EXPECT_EQ(actual, expected);

        std::vector<long long> expected_counts(patterns.size(), 0);
        for (const auto& [id, start, end] : expected) {
            (void)start;
            (void)end;
            ++expected_counts[id];
        }
        EXPECT_EQ(aho.count_occurrences(text), expected_counts);
    }
}
