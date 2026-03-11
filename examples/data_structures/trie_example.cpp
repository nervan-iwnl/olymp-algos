#include "data_structures/trie.hpp"
#include <iostream>
#include <string>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int q;
    /* Пример ввода:
    8
    insert apple
    insert app
    contains app
    contains apple
    contains ap
    starts_with ap
    erase app
    contains app

    Ожидаемый вывод:
    YES
    YES
    NO
    YES
    YES
    NO
    */
    cin >> q;

    Trie trie;

    while (q--) {
        string type, s;
        cin >> type >> s;

        if (type == "insert") {
            trie.insert(s);
        } else if (type == "contains") {
            cout << (trie.contains(s) ? "YES\n" : "NO\n");
        } else if (type == "starts_with") {
            cout << (trie.starts_with(s) ? "YES\n" : "NO\n");
        } else if (type == "erase") {
            cout << (trie.erase(s) ? "YES\n" : "NO\n");
        }
    }

    return 0;
}