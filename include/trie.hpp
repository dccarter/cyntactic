//
// Created by Mpho Mbotho on 2021-08-13.
//

#pragma once

#include <functional>
#include <memory>
#include <stack>
#include <unordered_map>

#include <exceptions.hpp>

namespace cyntactic {

    template <typename T, bool Override = false>
    class Trie {
        struct Node {
            using Ptr = std::unique_ptr<Node>;
            Node() = default;
            Node(T&& data)
                : data{std::move(data)}
            {}

            Node(Node&&) = default;
            Node& operator=(Node&&) = default;

            Node(const Node&) = delete;
            Node& operator=(const Node) = delete;

            ~Node() = default;

            std::optional<T> data{};
            std::unordered_map<char, Ptr> children;
        };

    public:
        using Iterator = std::function<bool(const std::string& key, const T& data)>;

        Trie() = default;

        Trie(std::initializer_list<std::pair<std::string_view, T&&>> init)
        {
            for (auto& [k, v] : init) {
                emplace(k, std::move(v));
            }
        }

        void emplace(std::string_view key, T&& data)
        {
            emplace(mHead, key, std::forward<T>(data), key);
        }

        const std::optional<T>& find(const std::string_view& key) const
        {
            return find(mHead, key);
        }

        const T& operator[](const std::string_view& key) const
        {
            const auto& data = find(mHead, key);
            if (!data) {
                throw TrieOperationError(TrieOperationError::KeyNotFound,
                             key, "key does not exist in trie");
            }
            return *data;
        }

        std::optional<T> erase(const std::string_view& key)
        {
            auto [_, data] = erase(mHead, key);
            return std::move(data);
        }

        bool empty() const
        {
            return mHead.children.empty() || !mHead.data.has_value();
        }

        void operator|(Iterator func)
        {
            enumerateInternal(mHead, "", func);
        }

        void enumerate(Iterator func, const std::string_view& prefix)
        {
            if (prefix.empty()) {
                enumerateInternal(mHead, "", func);
            }
            else {
                Node* node = &mHead;
                for (const auto& c: prefix) {
                    auto it = node->children.find(c);
                    if (it == node->children.end()) {
                        return;
                    }
                    node = it->second.get();
                }
                enumerateInternal(*node, std::string{prefix}, func);
            }
        }

    private:
        void enumerateInternal(Node& node, std::string key, Iterator& func)
        {
            if (node.data) {
                if (!func(key, *node.data)) {
                    return;
                }
            }

            if (!node.children.empty()) {
                for (auto& [c, child]: node.children) {
                    enumerateInternal(*child, key + c, func);
                }
            }
        }

        std::pair<bool, std::optional<T>> erase(Node& node, const std::string_view& key)
        {
            if (key.empty()) {
                // reset the data
                auto data = std::move(node.data);
                return {true, std::move(data)};
            }
            else {
                auto it = node.children.find(key[0]);
                if (it == node.children.end()) {
                    // does not exist
                    return {true, {}};
                }
                auto [found, data] = erase(*(it->second), key.substr(1));
                if (found) {
                    if (it->second->children.empty()) {
                        // remove this node be
                        node.children.erase(it);
                    }
                    return std::move(data);
                }
                return {false, {}};
            }
        }

        const std::optional<T>& find(const Node& node, std::string_view& key)
        {
            if (key.empty()) {
                return node.data;
            }
            auto it = node.children.find(key[0]);
            if (it == node.children.end()) {
                // key does not exist
                static const std::optional<T> DoesNotExist{};
                return DoesNotExist;
            }
            return find(*(it->second), key.substr(1));
        }

        void emplace(Node& node, const std::string_view& key, T&& data, const std::string_view& fkey)
        {
            if (key.empty()) {
                if constexpr (!Override) {
                    if (node.data.has_value()) {
                        throw TrieOperationError(
                                TrieOperationError::KeyAlreadyExists,
                                fkey, " trie does not support key override");
                    }
                }

                node.data = std::move(data);
                return;
            }
            auto ch = key[0];
            auto it = node.children.find(ch);
            if (it == node.children.end()) {
                // no word with this character
                auto inserted = node.children.emplace(ch, std::make_unique<Node>());
                emplace(*(inserted.first->second), key.substr(1), std::forward<T>(data), fkey);
            }
            else {
                emplace(*(it->second), key.substr(1), std::forward<T>(data), fkey);
            }
        }
        Node mHead{};
    };
}