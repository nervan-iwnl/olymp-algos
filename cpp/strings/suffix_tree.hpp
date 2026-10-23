#pragma once

#include <string>
#include <utility>
#include <unordered_map>
#include <stdexcept>
#include <string_view>
#include <queue>
#include <vector>

// Suffix Tree / Суффиксное дерево. Построение алгоритмом Укконена.
// Индексирует одну строку; после построения строка не изменяется.
// Символы трактуются как байты. Терминатор не должен встречаться в строке,
// а образцы для поиска не должны содержать терминатор.
//
// Операции:
//   - contains(pattern): проверить наличие подстроки
//   - find_all(pattern): получить 0-based позиции всех вхождений
//   - count(pattern): получить число вхождений, включая пересекающиеся
//   - longest_repeated_substring(): самая длинная повторяющаяся подстрока
//   - count_distinct_substrings(): число различных непустых подстрок
//
// Соглашения:
//   - find_all возвращает позиции в произвольном порядке
//   - пустой образец встречается на всех n + 1 границах строки
//   - при нескольких самых длинных повторениях возвращается любое
//   - string_view результата действителен только при жизни дерева
//   - метки рёбер: [l, r); r == -1 означает общий текущий конец листьев
//
// Сложность (n = длина строки, m = длина образца, k = число вхождений):
//   - построение: ожидаемое O(n), переходы через unordered_map
//   - contains: ожидаемое O(m)
//   - find_all / count: ожидаемое O(m + k), дополнительная память O(k)
//   - longest_repeated_substring / count_distinct_substrings: O(n)
//   - память дерева: O(n); рекурсивное удаление использует до O(n) стека

struct Node {
    int l = 0;
    int r = 0; // r == -1 means opened edge

    std::unordered_map<char, Node*> children;
    
    Node* parent = nullptr;
    Node* suff_link = nullptr;

    int suffix_start = -1;
};


class SuffixTree {
public:
    explicit SuffixTree(std::string input, char terminal='$')
        : str_(std::move(input)),
          terminal_(terminal) 
    {
        if (str_.find(terminal_) != std::string::npos) {
            throw std::invalid_argument(
                "Terminal must not occur in the input"
            );
        }

        root_ = new Node();
        root_->suff_link = root_;
        active_node_ = root_;

        try {
            build();
        } catch (...) {
            destroy(root_);
            throw;
        }
    }

    ~SuffixTree() {
        destroy(root_);
    }

    SuffixTree(const SuffixTree&) = delete;
    SuffixTree& operator=(const SuffixTree&) = delete;

    bool contains(std::string_view pattern) const {
        return find_node(pattern) != nullptr;
    }

    std::vector<int> find_all(std::string_view pattern) const {
        const Node* node = find_node(pattern);
        if (node == nullptr) {
            return {};
        }
        std::vector<int> ans;
        std::queue<const Node*> q;
        q.push(node);

        while (!q.empty()) {
            const Node* curr = q.front();
            q.pop();
            if (curr->children.empty()) {
                ans.push_back(curr->suffix_start);
                continue;
            }
            for (auto& [k, v] : curr->children) {
                q.push(v);
            }
        }
        return ans; 
    }

    int count(std::string_view pattern) const {
        return static_cast<int>(find_all(pattern).size());
    }

    std::string_view longest_repeated_substring() const {
        const Node* node = root_; 
        std::queue<std::pair<int, const Node*>> q;
        q.emplace(0, node);
        std::pair<int, const Node*> ans = {0, nullptr};
        while (!q.empty()) {
            auto [deep, curr] = q.front();
            q.pop();
            if (curr->children.empty()) {
                continue;
            }
            if (deep > ans.first) {
                ans = {deep, curr};
            }
            for (auto& [k, v] : curr->children) {
                q.emplace(deep + edge_length(v), v);
            }
        }
        
        if (ans.second == nullptr) {
            return {};
        }

        const Node* leaf = ans.second;
        
        while (!leaf->children.empty()) {
            leaf = leaf->children.begin()->second;
        }

        return std::string_view(str_).substr(
            leaf->suffix_start,
            ans.first
        );
    }

    long long count_distinct_substrings() const {
        const Node* node = root_; 
        std::queue<const Node*> q;
        q.push(node);
        
        long long cnt = 0;

        while (!q.empty()) {
            const Node* curr = q.front();
            q.pop();            
            for (auto& [chr, child] : curr->children) {
                cnt += static_cast<long long>(edge_length(child));
                q.push(child);
            }
        }
        return cnt - static_cast<long long>(str_.size());
    }

private: 
    static void destroy(Node* node) noexcept {
        if (node == nullptr) {
            return;
        }

        for (const auto& [character, child] : node->children) {
            destroy(child);
        }

        delete node;
    }

    void build() {
        str_.push_back(terminal_);

        for (int i = 0; i < static_cast<int>(str_.size()); ++i) {
            extend(i);
        }
    }

    void extend(int i) {
        position_ = i;
        remainder_++;
        
        Node* last = nullptr;
        while (remainder_ > 0) {
            if (active_length_ == 0) active_edge_ = i;

            auto key = str_[active_edge_];
            auto it = active_node_->children.find(key);
            if (it == active_node_->children.end()) {
                Node* leaf = make_node(
                    i, -1, i - remainder_ + 1
                );
                active_node_->children[key] = leaf; 
                if (last) {
                    last->suff_link = active_node_;
                    last = nullptr;
                }
            } else {
                auto nxt = it->second;
                auto length = edge_length(nxt);

                if (active_length_ >= length) {
                    active_edge_ += length;
                    active_length_ -= length;
                    active_node_ = nxt;
                    continue;
                }
                
                auto next_chr = str_[nxt->l + active_length_];

                if (next_chr == str_[i]) {
                    if (last && active_node_ != root_) {
                        last->suff_link = active_node_;
                    }   
                    active_length_++;
                    break;
                }

                Node* split = make_node(
                    nxt->l, nxt->l + active_length_
                );
                active_node_->children[key] = split;
                
                Node* leaf = make_node(
                    i, -1, i - remainder_ + 1
                );
                
                split->children[str_[i]] = leaf;
            
                nxt->l += active_length_;
                split->children[str_[nxt->l]] = nxt;
                
                if (last) {
                    last->suff_link = split;
                }
                last = split;
            }
            
            --remainder_;

            if (active_node_ == root_ && active_length_ > 0) {
                --active_length_; 
                active_edge_ = i - remainder_ + 1;
            } else if (active_node_ != root_) {
                active_node_ = active_node_->suff_link;
            }
        }
    }

    Node* make_node(int l, int r, int suffix_start = -1) {
        Node* node = new Node();

        node->l = l;
        node->r = r;
        node->suffix_start = suffix_start;
        node->suff_link = root_;

        return node;
    }

    int edge_length(const Node* node) const {
        int end = (node->r == -1)
            ? position_ + 1
            : node->r;
        
        return end - node->l;
    }

    const Node* find_node(std::string_view pattern) const {
        const Node* node = root_;
        int j = 0;

        while (j < static_cast<int>(pattern.size())) {
            auto it = node->children.find(pattern[j]);

            if (it == node->children.end()) {
                return nullptr;
            }

            const Node* next = it->second;

            int end = next->l + edge_length(next);
            int k = next->l;

            while (
                k < end &&
                j < static_cast<int>(pattern.size())
            ) {
                if (str_[k] != pattern[j]) {
                    return nullptr;
                }

                ++k;
                ++j;
            }

            node = next;
        }

        return node;
    }

    std::string str_;
    char terminal_ = '$';

    Node* root_ = nullptr;
    Node* active_node_ = nullptr;

    int active_edge_ = -1;
    int active_length_ = 0;

    int remainder_ = 0;
    int position_ = -1;
};
