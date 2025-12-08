#pragma once
#include <vector>

// Disjoint Set Union (DSU) / Union-Find.
// Структура данных для поддержки семейств непересекающихся множеств.
// Операции:
//   - find(v): найти представителя множества, где лежит v
//   - unite(a, b): объединить множества, где лежат a и b
//   - size(v): размер множества, где лежит v
//
// Реализация:
//   - сжатие пути (path compression) в find
//   - объединение по размеру (union by size)
// Сложность (амортизированная):
//   - ~O(1) на операцию)

class DSU {
public:
    DSU(int n) {
        parent.resize(n);
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
        sz.assign(n, 1);
    }
    int find(int v) {
        if (parent[v] != v) {
            parent[v] = find(parent[v]);
        }
        return parent[v];
    }
    void unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a != b) {
            if (sz[a] < sz[b]) {
                std::swap(a, b);
            }
            parent[b] = a;
            sz[a] += sz[b];
        }
    }
    int size(int v) {
        return sz[find(v)];
    }
private:
    std::vector<int> parent;
    std::vector<int> sz;
};