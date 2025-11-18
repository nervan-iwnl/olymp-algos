#include <iostream>
#include <vector>

#include "number_theory/primes.hpp"

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Формат:
    //   n
    //
    // Пример:
    //   30
    //
    // Ожидаемый вывод:
    //   count: 10
    //   primes: 2 3 5 7 11 13 17 19 23 29

    int n;
    if (!(cin >> n)) {
        return 0;
    }

    vector<int> primes = sieve(n);  // простые < n

    cout << "count: " << primes.size() << "\n";
    cout << "primes: ";
    for (int p : primes) {
        cout << p << " ";
    }
    cout << "\n";

    return 0;
}
