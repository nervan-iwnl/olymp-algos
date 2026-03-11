#pragma once
#include <array>
#include <string_view>
#include <stdexcept>

// Trie / Prefix Tree.
// Хранит строки из символов 'a'..'z'.
// Операции:
//   - insert(word): добавить слово
//   - contains(word): проверить точное наличие слова
//   - starts_with(prefix): проверить наличие слова с таким префиксом
//   - erase(word): удалить слово, если оно есть
//
// Реализация:
//   - у каждой вершины 26 переходов
//   - terminal = признак конца слова
//   - erase удаляет лишние вершины снизу вверх
//
// Сложность:
//   - insert / contains / starts_with / erase: O(|s|)
//   - память: O(sum of inserted strings))


class Trie {
public:
    Trie() : root_(new Node()) {}
    ~Trie() {
        destroy(root_);
    }

    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    void insert(std::string_view word) {
        Node* cur = root_;
        for (char chr : word) {
            Node*& next_node = cur->next[index_of(chr)];
            if (next_node == nullptr) {
                next_node = new Node();
            }
            cur = next_node;
        }
        cur->terminal = true;
    }

    bool contains(std::string_view word) const {
        Node* node = find_node(word);
        return node != nullptr && node->terminal;
    }

    bool starts_with(std::string_view prefix) const {
        return find_node(prefix) != nullptr;
    }

    bool erase(std::string_view word) {
        bool erased = false;
        erase(root_, word, 0, erased);
        return erased;
    }

private:
    struct Node {
        std::array<Node*, 26> next;
        bool terminal;
        Node() : terminal(false) {
            next.fill(nullptr);
        }
    };

    Node* root_;

    static int index_of(char c) {
        if (c > 'z' || c < 'a') throw std::invalid_argument("Trie supports only lowercase latin letters: a-z"); 
        return c - 'a';
    }

    void destroy(Node* node) {
        if (node == nullptr) return;
        for (Node* child : node->next) {
            destroy(child);
        }
        delete node;
    }

    Node* find_node(std::string_view s) const {
        Node* cur = root_;
        for (char chr : s) {
            Node* next_node = cur->next[index_of(chr)];
            if (next_node == nullptr) return nullptr;
            cur = next_node;
        }
        return cur;
    }

    bool erase(Node* node, std::string_view word, std::size_t pos, bool& erased) {
        if (pos == word.size()) {
            if (!node->terminal) return false;
            node->terminal = false;
            erased = true;
            return !has_children(node);
        }
        int idx = index_of(word[pos]);
        Node* child = node->next[idx];
        if (child == nullptr) return false;

        bool delete_child = erase(child, word, pos + 1, erased);

        if (delete_child) {
            delete child;
            node->next[idx] = nullptr;
        }
        return !node->terminal && !has_children(node);
    }

    static bool has_children(Node* node) {
        for (Node* child : node->next) {
            if (child != nullptr) return true;
        }
        return false;
    }
};