#include <iostream>
#include <vector>

#include "graph/kahn.hpp"

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Формат ввода:
    // n m
    // далее m строк: u v  (ребро u -> v)
    //
    // Пример:
    // 4 3
    // 0 1
    // 0 2
    // 1 3
    //
    // Возможный вывод:
    // topo order: 0 1 2 3

    int n, m;
    if (!(cin >> n >> m)) {
        return 0;
    }

    vector<vector<int>> graph(n);

    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        graph[u].push_back(v);
    }

    vector<int> order = topo_sort(graph);

    if (order.empty()) {
        cout << "cycle detected\n";
    } else {
        cout << "topo order: ";
        for (int v : order) {
            cout << v << " ";
        }
        cout << "\n";
    }

    return 0;
}