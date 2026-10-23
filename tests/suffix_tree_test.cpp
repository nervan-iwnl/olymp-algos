#include "strings/suffix_tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <map>
#include <random>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace {

std::vector<int> naive_positions(std::string_view text,
                                 std::string_view pattern) {
    std::vector<int> result;
    for (std::size_t i = 0; i + pattern.size() <= text.size(); ++i) {
        if (text.substr(i, pattern.size()) == pattern) {
            result.push_back(static_cast<int>(i));
        }
    }
    return result;
}

void expect_query(const SuffixTree& tree, std::string_view text,
                  std::string_view pattern) {
    SCOPED_TRACE(std::string(pattern));
    const auto expected = naive_positions(text, pattern);
    auto actual = tree.find_all(pattern);
    std::sort(actual.begin(), actual.end());
    EXPECT_EQ(actual, expected);
    EXPECT_EQ(tree.contains(pattern), !expected.empty());
    EXPECT_EQ(tree.count(pattern), static_cast<int>(expected.size()));
}

void expect_matches_naive(const std::string& text) {
    SCOPED_TRACE(text);
    const SuffixTree tree(text);
    std::map<std::string, int> frequencies;
    for (std::size_t i = 0; i < text.size(); ++i) {
        for (std::size_t length = 1; i + length <= text.size(); ++length) {
            ++frequencies[text.substr(i, length)];
        }
    }

    expect_query(tree, text, "");
    expect_query(tree, text, "z");
    expect_query(tree, text, text + "z");

    std::size_t best_length = 0;
    for (const auto& [pattern, count] : frequencies) {
        expect_query(tree, text, pattern);
        if (count >= 2) {
            best_length = std::max(best_length, pattern.size());
        }
    }

    EXPECT_EQ(tree.count_distinct_substrings(),
              static_cast<long long>(frequencies.size()));
    const auto repeated = tree.longest_repeated_substring();
    EXPECT_EQ(repeated.size(), best_length);
    if (!repeated.empty()) {
        EXPECT_GE(naive_positions(text, repeated).size(), std::size_t{2});
        EXPECT_EQ(repeated.find('$'), std::string_view::npos);
    }
}

}  // namespace

TEST(SuffixTree, FindsMatchesIncludingEndInsideEdge) {
    const SuffixTree tree("banana");
    for (std::string_view pattern : {"b", "ban", "an", "ana", "na",
                                     "nana", "banana", "ann", "bananas"}) {
        expect_query(tree, "banana", pattern);
    }
    EXPECT_EQ(tree.longest_repeated_substring(), "ana");
    EXPECT_EQ(tree.count_distinct_substrings(), 15);
}

TEST(SuffixTree, HandlesEmptyAndSingleCharacterStrings) {
    expect_matches_naive("");
    expect_matches_naive("a");
    const SuffixTree empty("");
    EXPECT_EQ(empty.find_all(""), (std::vector<int>{0}));
    EXPECT_TRUE(empty.longest_repeated_substring().empty());
}

TEST(SuffixTree, HandlesOverlapsAndPendingSuffixes) {
    for (const std::string text : {"aaaaaa", "abababab", "mississippi",
                                   "abcabxabcd", "abac", "abcdef"}) {
        expect_matches_naive(text);
    }
}

TEST(SuffixTree, AcceptsAnyLongestRepeatWhenThereIsATie) {
    const SuffixTree tree("abXabYcdZcd");
    const auto answer = tree.longest_repeated_substring();
    EXPECT_TRUE(answer == "ab" || answer == "cd");
}

TEST(SuffixTree, RejectsTerminalInInputAndSupportsCustomTerminal) {
    EXPECT_THROW((SuffixTree("a$b")), std::invalid_argument);
    EXPECT_THROW((SuffixTree("a#b", '#')), std::invalid_argument);
    const SuffixTree tree("a$a$", '#');
    expect_query(tree, "a$a$", "$");
    expect_query(tree, "a$a$", "a$");
    EXPECT_EQ(tree.longest_repeated_substring(), "a$");
    EXPECT_EQ(tree.count_distinct_substrings(), 7);
}

TEST(SuffixTree, SupportsNullBytesAndHighBytes) {
    const std::string text = std::string("a\0", 2) + char(0xFF) + "a";
    const SuffixTree tree(text);
    for (std::size_t i = 0; i < text.size(); ++i) {
        for (std::size_t length = 1; i + length <= text.size(); ++length) {
            expect_query(tree, text, std::string_view(text).substr(i, length));
        }
    }
    const SuffixTree custom("abab", '\0');
    expect_query(custom, "abab", "ab");
    EXPECT_EQ(custom.longest_repeated_substring(), "ab");
}

TEST(SuffixTree, MatchesNaiveAlgorithmsOnRandomStrings) {
    std::mt19937 generator(0x5AFF17);
    std::uniform_int_distribution<int> length(0, 24);
    std::uniform_int_distribution<int> letter(0, 2);
    for (int trial = 0; trial < 250; ++trial) {
        SCOPED_TRACE(trial);
        std::string text(static_cast<std::size_t>(length(generator)), 'a');
        for (char& ch : text) {
            ch = static_cast<char>('a' + letter(generator));
        }
        expect_matches_naive(text);
    }
}

TEST(SuffixTree, HandlesLongRepeatedInput) {
    const std::string text(2000, 'a');
    const SuffixTree tree(text);
    EXPECT_EQ(tree.count("aaa"), 1998);
    EXPECT_EQ(tree.count_distinct_substrings(), 2000);
    const auto answer = tree.longest_repeated_substring();
    EXPECT_EQ(answer, std::string_view(text).substr(1));
    // A view into the tree remains valid after another const query.
    EXPECT_TRUE(tree.contains("aaaa"));
    EXPECT_EQ(answer.size(), std::size_t{1999});
}

static_assert(!std::is_copy_constructible_v<SuffixTree>);
static_assert(!std::is_copy_assignable_v<SuffixTree>);
