#pragma once

#include <vector>
#include <queue>
#include <limits>
#include <utility>
#include <functional>

// Dijkstra's Algorithm.
// Принимает взвешенный граф в виде adjacency list:
// graph[u] = { {v, w}, ... }, где
// v — соседняя вершина,
// w — вес ребра u -> v (w >= 0).
//
// Возвращает массив dist, где dist[v] — кратчайшее расстояние от start до v.
// Если вершина недостижима, dist[v] = INF.
// Сложность: O((V + E) log V) по времени, O(V) по памяти.

std::vector<long long> dijkstra(const std::vector<std::vector<std::pair<int, int>>>& graph, int start) {
    const long long INF = std::numeric_limits<long long>::max();
    
    std::vector<long long> dist(graph.size(), INF);
    std::priority_queue<std::pair<long long, int>> pq;

    dist[start] = 0;
    pq.emplace(0, start);

    while (!pq.empty()) {
        auto [curr_dist, v] = pq.top();
        pq.pop();
        if (curr_dist != dist[v]) continue;

        for (auto& [u, w] : graph[v]) {
            if (w + dist[v] < dist[u]) {
                dist[u] = w + dist[v];
                pq.emplace(dist[u], u);
            }
        }
    }
    return dist;
}