#pragma once
#include <vector>


// Sieve of Eratosthenes.
// Возвращает все простые числа < n.
// Сложность: O(n log log n) по времени, O(n) по памяти.

std::vector<int> sieve(int n) {
    if (n < 2) return {};

    std::vector<bool> prime_flags(n, true);
    prime_flags[0] = false;
    prime_flags[1] = false;
    
    for (int num = 2; num * num < n; ++num) {
        if (prime_flags[num]) {
            for (int mul = num * num; mul < n; mul += num) {
                prime_flags[mul] = false;
            }
        }
    }
 
    std::vector<int> primes;
    for (int i = 2; i < n; ++i) {
        if (prime_flags[i]) {
            primes.push_back(i);
        }
    }
    return primes;
}