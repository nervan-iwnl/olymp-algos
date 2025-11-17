#include <iostream>
#include <vector>

#include "strings/kmp.hpp"

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Формат:
    //   pattern text
    // Пример:
    //   aba abacaba
    //
    // Ожидаемый вывод:
    //   occurrences: 2
    //   positions: 0 4

    string pattern, text;
    if (!(cin >> pattern >> text)) {
        return 0;
    }

    vector<int> positions = kmp_search(text, pattern);

    cout << "occurrences: " << positions.size() << "\n";
    cout << "positions: ";
    for (int p : positions) {
        cout << p << " ";
    }
    cout << "\n";

    return 0;
}


