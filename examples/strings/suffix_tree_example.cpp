#include "strings/suffix_tree.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    /* Пример ввода (строка, число запросов, затем команды):
    banana
    7
    contains an
    contains apple
    count ana
    find_all na
    find_all xyz
    longest_repeated
    distinct

    Ожидаемый вывод:
    YES
    NO
    2
    2 4
    NONE
    ana
    15

    Строка и образцы вводятся без пробелов и без терминатора '$'.
    longest_repeated выводит NONE, если повторений нет.
    */

    std::string text;
    int queries;
    if (!(std::cin >> text >> queries) || queries < 0) {
        return 1;
    }

    const SuffixTree tree(text);

    for (int i = 0; i < queries; ++i) {
        std::string command;
        if (!(std::cin >> command)) {
            return 1;
        }

        if (command == "longest_repeated") {
            const auto answer = tree.longest_repeated_substring();
            std::cout << (answer.empty() ? std::string_view("NONE") : answer)
                      << '\n';
        } else if (command == "distinct") {
            std::cout << tree.count_distinct_substrings() << '\n';
        } else if (command == "contains" || command == "count" ||
                   command == "find_all") {
            std::string pattern;
            if (!(std::cin >> pattern)) {
                return 1;
            }

            if (command == "contains") {
                std::cout << (tree.contains(pattern) ? "YES" : "NO") << '\n';
            } else if (command == "count") {
                std::cout << tree.count(pattern) << '\n';
            } else {
                auto positions = tree.find_all(pattern);
                std::sort(positions.begin(), positions.end());
                if (positions.empty()) {
                    std::cout << "NONE";
                }
                for (std::size_t j = 0; j < positions.size(); ++j) {
                    if (j != 0) {
                        std::cout << ' ';
                    }
                    std::cout << positions[j];
                }
                std::cout << '\n';
            }
        } else {
            std::cerr << "Unknown command: " << command << '\n';
            return 1;
        }
    }

    return 0;
}
