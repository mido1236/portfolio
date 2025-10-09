#ifndef ECS_ECS_H
#define ECS_ECS_H
#include <cassert>
#include <memory>
#include <ranges>
#include <typeindex>
#include <iostream>

#include "component_array.h"
#include "entity.h"

class ECS {
    Entity nextEntityID;
    unordered_map<type_index, unique_ptr<IComponentArray> > componentArrays;
    unordered_map<Entity, Signature> entitySignatures;

public:
    ECS() : nextEntityID(0) {}
    ~ECS() { clear(); }

    void clear() {
        componentArrays.clear();
    }

    Entity createEntity() {
        const Entity e = nextEntityID++;
        entitySignatures[e].reset();
        return e;
    }

    void destroyEntity(const Entity e) {
        for (const auto &arr: componentArrays | views::values) {
            arr->remove(e);
        }
        entitySignatures.erase(e);
    }

    template<typename T>
    void addComponent(const Entity &e, const T &component) {
        const auto type = type_index(typeid(T));
        if (!componentArrays.contains(type)) {
            componentArrays[type] = make_unique<ComponentArray<T> >();
        }
        auto *x = static_cast<ComponentArray<T> *>(componentArrays[type].get());
        x->insert(e, component);
        entitySignatures[e].set(getComponentTypeID<T>());
    }

    template<typename T>
    T *ensureComponent(Entity e, const T &defaultVal) {
        T *comp = getComponent<T>(e);
        if (!comp) addComponent(e, defaultVal), comp = getComponent<T>(e);
        return comp;
    }

    template<typename T>
    ComponentArray<T> *getComponentArray() {
        const auto type = type_index(typeid(T));
        if (!componentArrays.contains(type)) return nullptr;
        return static_cast<ComponentArray<T> *>(componentArrays[type].get());
    }

    template<typename T>
    T *getComponent(Entity e) {
        auto arr = getComponentArray<T>();
        auto comp = arr ? arr->get(e) : nullptr;
#ifndef NDEBUG
        if (entitySignatures.contains(e) && entitySignatures[e].test(getComponentTypeID<T>())) {
            assert(comp && "Unexpected empty component array entry");
        }
#endif
        return comp;
    }

    template<typename... Components>
    vector<Entity> queryEntities() {
        Signature required;
        (required.set(getComponentTypeID<Components>()), ...);
        vector<Entity> result;

        for (auto &[e,sig]: entitySignatures) {
            if ((sig & required) == required) {
#ifndef NDEBUG
                // sanity check: all required components must exist
                ((assert(getComponent<Components>(e) && "Signature mismatch in query!")), ...);
#endif
                result.push_back(e);
            }
        }

        return result;
    }

    void printPositions() {
        for (const auto &e: queryEntities<Position>()) {
            const auto *p = getComponent<Position>(e);
            cout << "Entity " << e << ": (" << p->x << ", " << p->y << ")\n";
        }
    }
};

#endif //ECS_ECS_H
