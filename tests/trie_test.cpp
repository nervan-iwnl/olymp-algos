#include "data_structures/trie.hpp"

#include <gtest/gtest.h>

#include <random>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace {

std::string random_word(std::mt19937& generator) {
    std::uniform_int_distribution<int> length(1, 6);
    std::uniform_int_distribution<int> letter(0, 3);
    std::string result(length(generator), 'a');
    for (char& chr : result) chr = static_cast<char>('a' + letter(generator));
    return result;
}

bool naive_starts_with(const std::unordered_set<std::string>& words,
                       const std::string& prefix) {
    for (const std::string& word : words) {
        if (word.compare(0, prefix.size(), prefix) == 0) return true;
    }
    return false;
}

}  // namespace

TEST(Trie, SupportsPrefixesExactMatchesAndErase) {
    Trie trie;
    trie.insert("apple");
    trie.insert("app");

    EXPECT_TRUE(trie.contains("apple"));
    EXPECT_TRUE(trie.contains("app"));
    EXPECT_FALSE(trie.contains("ap"));
    EXPECT_TRUE(trie.starts_with("ap"));

    EXPECT_TRUE(trie.erase("app"));
    EXPECT_FALSE(trie.contains("app"));
    EXPECT_TRUE(trie.contains("apple"));
    EXPECT_FALSE(trie.erase("app"));
    EXPECT_THROW(trie.insert("App"), std::invalid_argument);
}

TEST(Trie, MatchesUnorderedSetOnRandomOperations) {
    std::mt19937 generator(0x7A1E);
    std::uniform_int_distribution<int> operation(0, 3);
    Trie trie;
    std::unordered_set<std::string> expected;

    for (int step = 0; step < 5000; ++step) {
        SCOPED_TRACE(step);
        std::string word = random_word(generator);
        switch (operation(generator)) {
            case 0:
                trie.insert(word);
                expected.insert(word);
                break;
            case 1:
                EXPECT_EQ(trie.erase(word), expected.erase(word) != 0);
                break;
            case 2:
                EXPECT_EQ(trie.contains(word), expected.count(word) != 0);
                break;
            case 3:
                EXPECT_EQ(trie.starts_with(word), naive_starts_with(expected, word));
                break;
        }
    }
}
