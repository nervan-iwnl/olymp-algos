#pragma once

// KMP (Knuth–Morris–Pratt) pattern matching.
// prefix_function + поиск по pattern + '#' + text.
// Возвращаем 0-based позиции всех вхождений pattern в text.
// Сложность: O(n + m) по времени, O(n + m) по памяти.

#include <vector>
#include <string>

// prefix_function: pi[i] = длина наибольшего собственного суффикса s[0..i],
// который совпадает с её префиксом.
inline std::vector<int> prefix_function(const std::string &s) {
    int n = (int)s.length();
    std::vector<int> pi(n);
    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j]) {
            j = pi[j - 1];
        }
        if (s[i] == s[j]) {
            ++j;
        }
        pi[i] = j;
    }

    return pi;
}

inline std::vector<int> kmp_search(const std::string &text,
                                   const std::string &pattern) {

    if (pattern.empty()) return {};    
    auto s = pattern + '#' + text;
    auto pi = prefix_function(s);
    std::vector<int> pos;
    for (int i = (int)pattern.size() + 1; i < (int)s.size(); ++i) {
        if (pi[i] == (int)pattern.size()) {
            pos.push_back(i - 2 * (int)pattern.size());
        }
    }

    return pos;
}   
