#ifndef ECS_COMPONENT_ARRAY_H
#define ECS_COMPONENT_ARRAY_H
#include <unordered_map>

#include "components.h"
#include "entity.h"
#include <vector>

using namespace std;

struct IComponentArray {
    virtual ~IComponentArray() = default;

    virtual void remove(Entity e) = 0;
};

template<typename>
ComponentTypeID getComponentTypeID() {
    static ComponentTypeID id = nextComponentTypeID++;
    return id;
}

template<typename T>
class ComponentArray final : public IComponentArray {
    vector<T> components;
    vector<Entity> entities;
    unordered_map<Entity, size_t> entityToIndex;

public:
    void insert(const Entity entity, const T &component) {
        if (!entityToIndex.contains(entity)) {
            entityToIndex[entity] = components.size();
            entities.push_back(entity);
            components.push_back(component);
            return;
        }
        size_t index = entityToIndex[entity];
        components[index] = component;
    }

    void remove(const Entity entity) override {
        if (!entityToIndex.contains(entity)) return;
        size_t index = entityToIndex[entity];

        // Swap and pop
        entities[index] = entities.back();
        components[index] = components.back();

        // Update map
        entityToIndex[entities[index]] = index;
        entityToIndex.erase(entity);

        // Remove last element
        entities.pop_back();
        components.pop_back();
    }

    T *get(const Entity entity) {
        if (!entityToIndex.contains(entity)) return nullptr;
        size_t index = entityToIndex[entity];
        return &components[index];
    }

    const vector<T> &getAll() { return components; }
    const vector<Entity> &getEntities() { return entities; }
};

#endif //ECS_COMPONENT_ARRAY_H
