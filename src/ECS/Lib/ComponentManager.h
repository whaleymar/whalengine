#pragma once

#include <algorithm>
#include <cassert>
// #include <iostream>
#include <memory>
#include <vector>

#include "ECS/Lib/Component.h"

namespace whal::ecs {

class ComponentManager {
public:
    template <typename T>
    void registerComponent() {
        const ComponentType type = getComponentID<T>();
        assert(std::find(mComponentTypes.begin(), mComponentTypes.end(), type) == mComponentTypes.end());
        // std::cout << "assigned id " << type << std::endl;
        mComponentTypes.push_back(type);
        mComponentArrays.push_back(std::make_shared<ComponentArray<T>>());
    }

    template <typename T>
    std::optional<ComponentType> getComponentType() const {
        const ComponentType type = getComponentID<T>();
        auto it = std::find(mComponentTypes.begin(), mComponentTypes.end(), type);
        if (it == mComponentTypes.end()) {
            return std::nullopt;
        }
        return type;
    }

    template <typename T>
    void addComponent(const Entity entity, T component) {
        const ComponentType type = getComponentID<T>();
        auto it = std::find(mComponentTypes.begin(), mComponentTypes.end(), type);
        int ix;
        if (it == mComponentTypes.end()) {
            registerComponent<T>();
            ix = mComponentTypes.size() - 1;
        } else {
            ix = std::distance(mComponentTypes.begin(), it);
        }
        getComponentArray<T>(ix)->addData(entity, component);
    }

    template <typename T>
    void removeComponent(const Entity entity) const {
        const ComponentType type = getComponentID<T>();
        auto it = std::find(mComponentTypes.begin(), mComponentTypes.end(), type);
        if (it == mComponentTypes.end()) {
            return;
        }
        int ix = std::distance(mComponentTypes.begin(), it);
        getComponentArray<T>(ix)->removeData(entity);
    }

    template <typename T>
    std::optional<T*> getComponent(const Entity entity) const {
        const ComponentType type = getComponentID<T>();
        auto it = std::find(mComponentTypes.begin(), mComponentTypes.end(), type);
        if (it == mComponentTypes.end()) {
            return std::nullopt;
        }
        int ix = std::distance(mComponentTypes.begin(), it);
        return getComponentArray<T>(ix)->getData(entity);
    }

    void entityDestroyed(const Entity entity) {
        for (auto const& componentArray : mComponentArrays) {
            componentArray->entityDestroyed(entity);
        }
    }

    // assign unique IDs to each component type
    static inline ComponentType ComponentID = 0;
    template <typename T>
    static inline ComponentType getComponentID() {
        static ComponentType id = ComponentID++;
        return id;
    }

private:
    template <typename T>
    std::shared_ptr<ComponentArray<T>> getComponentArray(int ix) const {
        return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[ix]);
    }

    std::vector<ComponentType> mComponentTypes;
    std::vector<std::shared_ptr<IComponentArray>> mComponentArrays;
};

}  // namespace whal::ecs
