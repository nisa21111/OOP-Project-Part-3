#pragma once
#include <unordered_map>
#include <vector>
#include <optional>
#include <functional>
#include <stdexcept>
#include <string>
#include <concepts>
#include <memory>

template<typename T>
concept Identifiable = requires(T t) {
    { t.id() } -> std::convertible_to<std::string>;
};

template<typename T>
concept Displayable = requires(T t) {
    { t.display() } -> std::same_as<void>;
};

template<typename T>
class Repository {
    std::unordered_map<std::string, std::shared_ptr<T>> store_;

public:
    void add(const std::string& key, std::shared_ptr<T> value) {
        if (value != nullptr) {
            store_[key] = value;
        }
    }

    [[nodiscard]] std::optional<std::shared_ptr<T>> find(const std::string& key) const {
        auto it = store_.find(key);
        if (it == store_.end()) return std::nullopt;
        return it->second;
    }

    [[nodiscard]] std::shared_ptr<T> get(const std::string& key) const {
        auto it = store_.find(key);
        if (it == store_.end())
            throw std::out_of_range("Repository: cheie negasita: " + key);
        return it->second;
    }

    [[nodiscard]] bool   contains(const std::string& key) const { return store_.count(key) > 0; }
    [[nodiscard]] size_t size()  const { return store_.size(); }
    [[nodiscard]] bool   empty() const { return store_.empty(); }

    void remove(const std::string& key) { store_.erase(key); }

    [[nodiscard]] std::vector<std::string> keys() const {
        std::vector<std::string> result;
        result.reserve(store_.size());
        for (auto& [k, v] : store_) result.push_back(k);
        return result;
    }

    [[nodiscard]] std::vector<std::shared_ptr<T>> values() const {
        std::vector<std::shared_ptr<T>> result;
        result.reserve(store_.size());
        for (auto& [k, v] : store_) result.push_back(v);
        return result;
    }

    void forEach(const std::function<void(const std::string&, std::shared_ptr<T>)>& fn) const {
        for (auto& [k, v] : store_) fn(k, v);
    }

    void displayAll() const requires Displayable<T> {
        for (auto& [k, v] : store_) v->display();
    }
};

template<typename T>
class Repository<std::vector<T>> {
    std::unordered_map<std::string, std::vector<T>> store_;

public:
    void append(const std::string& key, T value) {
        store_[key].push_back(std::move(value));
    }

    [[nodiscard]] std::optional<std::vector<T>> find(const std::string& key) const {
        auto it = store_.find(key);
        if (it == store_.end()) return std::nullopt;
        return it->second;
    }

    [[nodiscard]] size_t size() const { return store_.size(); }
    void clear(const std::string& key) { store_.erase(key); }
};

template<typename T>
[[nodiscard]] std::optional<std::shared_ptr<T>> findIf(
    const Repository<T>& repo,
    const std::function<bool(const std::shared_ptr<T>&)>& pred)
{
    for (auto& val : repo.values()) {
        if (pred(val)) return val;
    }
    return std::nullopt;
}

template<typename T>
[[nodiscard]] std::vector<std::shared_ptr<T>> filterAll(
    const Repository<T>& repo,
    const std::function<bool(const std::shared_ptr<T>&)>& pred)
{
    std::vector<std::shared_ptr<T>> result;
    for (auto& val : repo.values()) {
        if (pred(val)) result.push_back(val);
    }
    return result;
}