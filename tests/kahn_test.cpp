#include "graph/kahn.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <random>
#include <vector>

namespace {

bool is_topological_order(const std::vector<std::vector<int>>& graph,
                          const std::vector<int>& order) {
    if (order.size() != graph.size()) return false;

    std::vector<int> position(order.size(), -1);
    for (int i = 0; i < static_cast<int>(order.size()); ++i) {
        if (order[i] < 0 || order[i] >= static_cast<int>(order.size())) return false;
        if (position[order[i]] != -1) return false;
        position[order[i]] = i;
    }
    for (int from = 0; from < static_cast<int>(graph.size()); ++from) {
        for (int to : graph[from]) {
            if (position[from] >= position[to]) return false;
        }
    }
    return true;
}

bool has_cycle(const std::vector<std::vector<int>>& graph) {
    std::vector<int> color(graph.size(), 0);
    std::function<bool(int)> dfs = [&](int v) {
        color[v] = 1;
        for (int to : graph[v]) {
            if (color[to] == 1) return true;
            if (color[to] == 0 && dfs(to)) return true;
        }
        color[v] = 2;
        return false;
    };
    for (int v = 0; v < static_cast<int>(graph.size()); ++v) {
        if (color[v] == 0 && dfs(v)) return true;
    }
    return false;
}

}  // namespace

TEST(Kahn, ReturnsAValidOrderForDag) {
    std::vector<std::vector<int>> graph = {{1, 2}, {3}, {3}, {}};
    EXPECT_TRUE(is_topological_order(graph, topo_sort(graph)));
}

TEST(Kahn, ReturnsEmptyOrderForCycle) {
    std::vector<std::vector<int>> graph = {{1}, {2}, {0}};
    EXPECT_TRUE(topo_sort(graph).empty());
}

TEST(Kahn, MatchesIndependentCycleDetectionOnRandomGraphs) {
    std::mt19937 generator(0xCA44);
    std::uniform_int_distribution<int> vertex_count(1, 10);
    std::bernoulli_distribution has_edge(0.2);

    for (int trial = 0; trial < 400; ++trial) {
        SCOPED_TRACE(trial);
        int n = vertex_count(generator);
        std::vector<std::vector<int>> graph(n);
        for (int from = 0; from < n; ++from) {
            for (int to = 0; to < n; ++to) {
                if (from != to && has_edge(generator)) graph[from].push_back(to);
            }
        }

        const bool cyclic = has_cycle(graph);
        const auto order = topo_sort(graph);
        if (cyclic) {
            EXPECT_TRUE(order.empty());
        } else {
            EXPECT_TRUE(is_topological_order(graph, order));
        }
    }
}
