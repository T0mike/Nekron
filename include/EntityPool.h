#ifndef ENTITY_POOL_H
#define ENTITY_POOL_H
#include <vector>
#include <memory>
#include <ostream>
#include <algorithm>

template<typename T>
class EntityPool {
    std::vector<std::unique_ptr<T>> entities;
public:
    EntityPool() = default;

    void add(std::unique_ptr<T> e) {
        entities.push_back(std::move(e));
    }

    std::size_t size() const { return entities.size(); }
    bool empty() const { return entities.empty(); }

    void removeDead() {
        entities.erase(
            std::remove_if(entities.begin(), entities.end(),
                [](const std::unique_ptr<T>& e) { return !e->isAlive(); }),
            entities.end());
    }

    template<typename Func>
    void forEach(Func f) {
        for (auto& e : entities)
            f(*e);
    }

    auto begin() { return entities.begin(); }
    auto end() { return entities.end(); }
    auto begin() const { return entities.begin(); }
    auto end() const { return entities.end(); }

    friend std::ostream& operator<<(std::ostream& os, const EntityPool<T>& pool) {
        for (const auto& e : pool.entities)
            os << *e;
        return os;
    }
};

template<typename Derived, typename Base>
Derived* findFirstOf(const EntityPool<Base>& pool) {
    for (const auto& e : pool) {
        Derived* d = dynamic_cast<Derived*>(e.get());
        if (d != nullptr)
            return d;
    }
    return nullptr;
}

#endif
