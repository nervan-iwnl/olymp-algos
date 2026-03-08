#pragma once

#include <vector>
#include <algorithm>

// Longest Increasing Subsequence (O(n log n)).
// Возвращает длину наибольшей строго возрастающей подпоследовательности.
// Сложность: O(n log n) по времени, O(n) по памяти.

int lis(const std::vector<int>& nums) {
    std::vector<int> tails;
    for (int x : nums) {
        auto it = std::lower_bound(tails.begin(), tails.end(), x);
        if (it == tails.end()) {
            tails.push_back(x);
        } else {
            *it = x;
        }
    }

    return (int)tails.size();
}