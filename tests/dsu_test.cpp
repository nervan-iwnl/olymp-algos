#include "data_structures/dsu.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <vector>

TEST(DSU, UnitesComponentsAndTracksSizes) {
    DSU dsu(6);
    dsu.unite(0, 1);
    dsu.unite(1, 2);
    dsu.unite(3, 4);

    EXPECT_EQ(dsu.find(0), dsu.find(2));
    EXPECT_NE(dsu.find(0), dsu.find(3));
    EXPECT_EQ(dsu.size(1), 3);
    EXPECT_EQ(dsu.size(4), 2);

    dsu.unite(2, 4);
    dsu.unite(0, 4);
    EXPECT_EQ(dsu.size(3), 5);
    EXPECT_EQ(dsu.size(5), 1);
}

TEST(DSU, MatchesNaiveComponentsOnRandomOperations) {
    std::mt19937 generator(0xD500D5);

    for (int trial = 0; trial < 200; ++trial) {
        SCOPED_TRACE(trial);
        constexpr int n = 20;
        DSU dsu(n);
        std::vector<int> component(n);
        for (int i = 0; i < n; ++i) component[i] = i;
        std::uniform_int_distribution<int> vertex(0, n - 1);

        for (int operation = 0; operation < 100; ++operation) {
            int a = vertex(generator);
            int b = vertex(generator);
            int old_component = component[b];
            int new_component = component[a];
            dsu.unite(a, b);
            for (int& id : component) {
                if (id == old_component) id = new_component;
            }

            for (int v = 0; v < n; ++v) {
                int expected_size = static_cast<int>(std::count(
                    component.begin(), component.end(), component[v]));
                EXPECT_EQ(dsu.size(v), expected_size);
                EXPECT_EQ(dsu.find(v) == dsu.find(a), component[v] == component[a]);
            }
        }
    }
}
