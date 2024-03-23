#pragma once

#include <unordered_map>

#include "ECS/Lib/Component.h"
#include "ECS/Lib/ComponentManager.h"
#include "ECS/Lib/Entity.h"

namespace whal::ecs {

class SystemManager;

class SystemBase {
public:
    friend SystemManager;
    virtual ~SystemBase() = default;
    virtual void update(){};

protected:
    std::unordered_map<EntityID, Entity> getEntities() { return mEntities; }

private:
    std::unordered_map<EntityID, Entity> mEntities;
};

// each system has a set of entities it operates on
// currently their methods are called manually
template <typename... T>
class ISystem : public SystemBase {
public:
    ISystem() {
        std::vector<ComponentType> componentTypes;
        InitializeIDs<T...>(componentTypes);
        for (auto const& componentType : componentTypes) {
            mPattern.set(componentType);
        }
    }

    Pattern getPattern() { return mPattern; }

private:
    // need a First and Second, otherwise there is ambiguity when there's only one element (InitializeIDs<type> vs InitializeIDs<type, <>>)
    template <typename First, typename Second, typename... Rest>
    void InitializeIDs(std::vector<ComponentType>& componentTypes) {
        componentTypes.push_back(ComponentManager::getComponentID<First>());
        InitializeIDs<Second, Rest...>(componentTypes);
    }

    template <typename Last>
    void InitializeIDs(std::vector<ComponentType>& componentTypes) {
        componentTypes.push_back(ComponentManager::getComponentID<Last>());
    }

    Pattern mPattern;
};

}  // namespace whal::ecs
