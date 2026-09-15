#include "strings/aho_corasick.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    /*
    Режим find:
      find
      4
      he
      she
      his
      hers
      ushers

    Ожидаемый вывод (порядок совпадений с одинаковым концом может меняться):
      she 1 3
      he 2 3
      hers 2 5

    Режим count:
      count
      3
      a
      aa
      aaa
      aaaa

    Ожидаемый вывод:
      a 4
      aa 3
      aaa 2

    Пока TODO в aho_corasick.hpp не реализованы, программа завершится
    исключением. Заглушки нужны, чтобы каркас компилировался.
    */

    string mode;
    int n;
    cin >> mode >> n;

    vector<string> patterns(n);
    size_t total_length = 0;
    for (string& pattern : patterns) {
        cin >> pattern;
        total_length += pattern.size();
    }

    AhoCorasick automaton;
    automaton.reserve_nodes(total_length + 1);

    for (const string& pattern : patterns) {
        automaton.add_pattern(pattern);
    }
    automaton.build();

    string text;
    cin >> text;

    if (mode == "find") {
        for (const auto& match : automaton.find_all(text)) {
            cout << patterns[match.pattern_id] << ' '
                 << match.start_position << ' '
                 << match.end_position << '\n';
        }
    } else if (mode == "count") {
        const auto counts = automaton.count_occurrences(text);
        for (int id = 0; id < n; ++id) {
            cout << patterns[id] << ' ' << counts[id] << '\n';
        }
    } else {
        cerr << "Unknown mode: use find or count\n";
        return 1;
    }

    return 0;
}

