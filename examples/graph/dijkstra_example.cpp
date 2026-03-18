#include <iostream>
#include <vector>
#include <limits>

#include "graph/dijkstra.hpp"
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Формат ввода:
    // n m
    // далее m строк: u v w  (ребро u -> v с весом w)
    // start
    //
    // Пример:
    // 5 6
    // 0 1 2
    // 0 2 4
    // 1 2 1
    // 1 3 7
    // 2 4 3
    // 3 4 1
    // 0
    //
    // Ожидаемый вывод:
    // dist: 0 2 3 9 6

    int n, m;
    if (!(cin >> n >> m)) {
        return 0;
    }

    vector<vector<pair<int,int>>> graph(n);
    const long long INF = std::numeric_limits<long long>::max();
    
    for (int i = 0; i < m; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        graph[u].push_back({v, w});
    }

    int start;
    cin >> start;

    vector<long long> dist = dijkstra(graph, start);

    cout << "dist: ";
    for (long long d : dist) {
        if (d == INF) {
            cout << "INF ";
        } else {
            cout << d << " ";
        }
    }
    cout << "\n";

    return 0;
}