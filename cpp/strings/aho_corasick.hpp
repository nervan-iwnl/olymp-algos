#pragma once

#include <array>
#include <cstddef>
#include <queue>
#include <stdexcept>
#include <string_view>
#include <vector>

// Aho-Corasick multiple-pattern matching for lowercase English letters.
// Сначала добавляются все образцы, затем один раз вызывается build().
// После построения добавлять новые образцы нельзя.
//
// Операции:
//   - add_pattern(pattern): добавить образец и получить его id
//   - build(): построить суффиксные ссылки и переходы автомата
//   - find_all(text): получить позиции всех вхождений
//   - count_occurrences(text): посчитать вхождения каждого образца
//
// Сложность:
//   - add_pattern: O(|pattern|)
//   - build: O(nodes * ALPHABET)
//   - find_all: O(|text| + matches)
//   - count_occurrences: O(|text| + nodes + patterns)
//   - память: O(nodes * ALPHABET + patterns)

class AhoCorasick {
public:
    static constexpr int ALPHABET = 26;

    struct Match {
        int pattern_id;
        int start_position;
        int end_position;
    };

    // Добавляет непустой образец и возвращает его id: 0, 1, 2, ...
    // Сложность: O(|pattern|).
    int add_pattern(std::string_view pattern) {
        if (built_) {
            throw std::logic_error("cannot add patterns after build");
        }
        if (pattern.empty()) {
            throw std::invalid_argument("pattern must not be empty");
        }

        int v = ROOT;

        for (char chr : pattern) {
            int c = index_of(chr);

            if (nodes_[v].next[c] == -1) {
                int new_node = static_cast<int>(nodes_.size());
                nodes_.emplace_back();
                nodes_[v].next[c] = new_node;
            }
            v = nodes_[v].next[c];
        }

        int pattern_id = static_cast<int>(pattern_vertex_.size());
        nodes_[v].direct_pattern_ids.push_back(pattern_id);
        pattern_vertex_.push_back(v);
        pattern_lengths_.push_back(static_cast<int>(pattern.size()));

        return pattern_id;
    }

    // Строит полные переходы, суффиксные и терминальные ссылки.
    // Вызывается один раз после добавления всех образцов.
    // Сложность: O(nodes * ALPHABET).
    void build() {
        if (built_) {
            throw std::logic_error("build can be called only once");
        }

        std::queue<int> q;
        bfs_order_.clear();

        for (int c = 0; c < ALPHABET; ++c) {
            if (nodes_[ROOT].next[c] == -1) {
                nodes_[ROOT].next[c] = ROOT;
            } else {
                int child = nodes_[ROOT].next[c];
                nodes_[child].suffix_link = ROOT;
                nodes_[child].terminal_link = -1;
                q.push(child);
                bfs_order_.push_back(child);
            }
        }

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            for (int c = 0; c < ALPHABET; ++c) {
                if (nodes_[v].next[c] == -1) {
                    nodes_[v].next[c] =
                        nodes_[nodes_[v].suffix_link].next[c];
                } else {
                    int u = nodes_[v].next[c];

                    nodes_[u].suffix_link =
                        nodes_[nodes_[v].suffix_link].next[c];

                    int candidate = nodes_[u].suffix_link;
                    if (nodes_[candidate].is_terminal()) {
                        nodes_[u].terminal_link = candidate;
                    } else {
                        nodes_[u].terminal_link =
                            nodes_[candidate].terminal_link;
                    }
                    q.push(u);
                    bfs_order_.push_back(u);
                }
            }
        }

        built_ = true;
    }

    // Перечисляет каждое вхождение каждого образца.
    // Сложность: O(|text| + количество возвращённых совпадений).
    std::vector<Match> find_all(std::string_view text) const {
        require_built();
        int state = ROOT;
        std::vector<Match> result;

        for (int idx = 0; idx < static_cast<int>(text.size()); ++idx) {
            int c = index_of(text[idx]);
            state = nodes_[state].next[c];

            for (int id : nodes_[state].direct_pattern_ids) {
                result.push_back({
                    id,
                    idx - pattern_lengths_[id] + 1,
                    idx
                });
            }

            int v = nodes_[state].terminal_link;
            while (v != -1) {
                for (int id : nodes_[v].direct_pattern_ids) {
                    result.push_back({
                        id,
                        idx - pattern_lengths_[id] + 1,
                        idx
                    });
                }
                v = nodes_[v].terminal_link;
            }
        }

        return result;
    }

    // Возвращает answer[id] = число вхождений образца id в тексте.
    // Совпадения не перечисляются.
    // Сложность: O(|text| + количество вершин + количество образцов).
    std::vector<long long> count_occurrences(std::string_view text) const {
        require_built();

        std::vector<long long> visits(nodes_.size(), 0);
        int state = ROOT;

        for (char chr : text) {
            int c = index_of(chr);
            state = nodes_[state].next[c];
            ++visits[state];
        }

        for (auto it = bfs_order_.rbegin(); it != bfs_order_.rend(); ++it) {
            int v = *it;
            visits[nodes_[v].suffix_link] += visits[v];
        }

        std::vector<long long> answer(pattern_vertex_.size(), 0);
        for (std::size_t id = 0; id < answer.size(); ++id) {
            answer[id] = visits[pattern_vertex_[id]];
        }

        return answer;
    }

    std::size_t node_count() const noexcept {
        return nodes_.size();
    }

    std::size_t pattern_count() const noexcept {
        return pattern_vertex_.size();
    }

    // Необязательная оптимизация: вызвать до add_pattern(), если заранее
    // известна сумма длин всех образцов.
    void reserve_nodes(std::size_t expected_node_count) {
        if (built_) {
            throw std::logic_error("cannot reserve after build");
        }
        nodes_.reserve(expected_node_count);
    }

private:
    static constexpr int ROOT = 0;

    struct Node {
        // До build(): настоящее ребро бора или -1.
        // После build(): полный переход автомата, -1 не остаётся.
        std::array<int, ALPHABET> next{};

        // Самый длинный собственный суффикс, присутствующий в боре.
        int suffix_link = 0;

        // Ближайший терминальный суффикс; -1, если такого нет.
        int terminal_link = -1;

        // Только образцы, заканчивающиеся непосредственно в этой вершине.
        // Результаты по ссылкам сюда не копируются.
        std::vector<int> direct_pattern_ids;

        Node() {
            next.fill(-1);
        }

        bool is_terminal() const noexcept {
            return !direct_pattern_ids.empty();
        }
    };

    std::vector<Node> nodes_{1};       // nodes_[0] -- корень
    std::vector<int> bfs_order_;       // корень можно не добавлять
    std::vector<int> pattern_vertex_;  // конечная вершина каждого образца
    std::vector<int> pattern_lengths_;
    bool built_ = false;

    static int index_of(char c) {
        if (c < 'a' || c > 'z') {
            throw std::invalid_argument(
                "AhoCorasick supports only lowercase latin letters: a-z");
        }
        return c - 'a';
    }

    void require_built() const {
        if (!built_) {
            throw std::logic_error("call build() before searching");
        }
    }
};
