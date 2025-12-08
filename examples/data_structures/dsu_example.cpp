#include "data_structures/dsu.hpp"
#include <iostream>
#include <string>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    /* Пример ввода:
    5 6
    union 0 1
    union 1 2
    same 0 2
    same 0 3
    union 3 4
    size 1
    Ожидаемый вывод:
    YES
    NO
    3*/
    cin >> n >> q;

    DSU dsu(n);

    while (q--) {
        string type;
        cin >> type;
        if (type == "union") {
            int a, b;
            cin >> a >> b;
            dsu.unite(a, b);
        } else if (type == "same") {
            int a, b;
            cin >> a >> b;
            if (dsu.find(a) == dsu.find(b)) {
                cout << "YES\n";
            } else {
                cout << "NO\n";
            }
        } else if (type == "size") {
            int a;
            cin >> a;
            cout << dsu.size(a) << "\n";
        }
    }

    return 0;
}
