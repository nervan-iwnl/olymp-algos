#pragma once

#include <vector>
#include <queue>


// Topological Sort (Kahn's Algorithm).
// Принимает ориентированный граф в виде adjacency list.
// Возвращает один из возможных топологических порядков вершин.
// Сложность: O(V + E) по времени, O(V) по памяти.

std::vector<int> topo_sort(std::vector<std::vector<int>>& graph) {
    int n = graph.size();
    std::vector<int> indegree(n, 0);

    for (int u = 0; u < n; ++u)
        for (int v : graph[u])
            indegree[v]++;
    
    
    std::queue<int> q; 
    for (int i = 0; i < n; ++i)
        if (indegree[i] == 0)
            q.push(i);

    std::vector<int> order;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        order.push_back(u);
        for (int v : graph[u]) {
            if (--indegree[v] == 0) q.push(v);
        }
    }
    if (order.size() != n) return {};
    
    return order;
}