#include "graph/dijkstra.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <random>
#include <utility>
#include <vector>

namespace {

constexpr long long INF = std::numeric_limits<long long>::max();

std::vector<long long> floyd_warshall_from(
    const std::vector<std::vector<std::pair<int, int>>>& graph,
    int start) {
    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<long long>> dist(n, std::vector<long long>(n, INF));
    for (int v = 0; v < n; ++v) {
        dist[v][v] = 0;
        for (const auto& [to, weight] : graph[v]) {
            dist[v][to] = std::min(dist[v][to], static_cast<long long>(weight));
        }
    }

    for (int middle = 0; middle < n; ++middle) {
        for (int from = 0; from < n; ++from) {
            for (int to = 0; to < n; ++to) {
                if (dist[from][middle] == INF || dist[middle][to] == INF) continue;
                dist[from][to] = std::min(
                    dist[from][to],
                    dist[from][middle] + dist[middle][to]);
            }
        }
    }
    return dist[start];
}

}  // namespace

TEST(Dijkstra, FindsShortestPathsAndUnreachableVertices) {
    std::vector<std::vector<std::pair<int, int>>> graph(6);
    graph[0] = {{1, 7}, {2, 2}};
    graph[1] = {{3, 1}};
    graph[2] = {{1, 2}, {3, 8}};
    graph[3] = {{4, 3}};

    EXPECT_EQ(dijkstra(graph, 0), (std::vector<long long>{0, 4, 2, 5, 8, INF}));
}

TEST(Dijkstra, HandlesDistancesLargerThanInt) {
    std::vector<std::vector<std::pair<int, int>>> graph(4);
    graph[0].push_back({1, 1'000'000'000});
    graph[1].push_back({2, 1'000'000'000});
    graph[2].push_back({3, 1'000'000'000});
    EXPECT_EQ(dijkstra(graph, 0)[3], 3'000'000'000LL);
}

TEST(Dijkstra, MatchesFloydWarshallOnRandomGraphs) {
    std::mt19937 generator(0xD1A57A);
    std::uniform_int_distribution<int> vertex_count(1, 9);
    std::uniform_int_distribution<int> weight(0, 30);
    std::bernoulli_distribution has_edge(0.3);

    for (int trial = 0; trial < 300; ++trial) {
        SCOPED_TRACE(trial);
        const int n = vertex_count(generator);
        std::vector<std::vector<std::pair<int, int>>> graph(n);
        for (int from = 0; from < n; ++from) {
            for (int to = 0; to < n; ++to) {
                if (from != to && has_edge(generator)) {
                    graph[from].push_back({to, weight(generator)});
                }
            }
        }
        std::uniform_int_distribution<int> start_vertex(0, n - 1);
        const int start = start_vertex(generator);
        EXPECT_EQ(dijkstra(graph, start), floyd_warshall_from(graph, start));
    }
}
