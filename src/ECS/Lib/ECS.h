#pragma once

#include <memory>

#include "Util/Expected.h"

#include "ECS/Lib/ComponentManager.h"
#include "ECS/Lib/EntityManager.h"
#include "ECS/Lib/SystemManager.h"

namespace whal::ecs {

class ECS {
public:
    static ECS& getInstance() {
        static ECS instance;
        return instance;
    }

    // ENTITY
    Expected<Entity> entity() const;
    void kill(Entity entity) const;

    // COMPONENT
    template <typename T>
    void add(const Entity entity, T component) {
        mComponentManager->addComponent(entity, component);

        auto pattern = mEntityManager->getPattern(entity);
        std::optional<ComponentType> componentType = mComponentManager->getComponentType<T>();
        if (!componentType) {
            return;
        }
        pattern.set(componentType.value(), true);
        mEntityManager->setPattern(entity, pattern);
        mSystemManager->entityPatternChanged(entity, pattern);
    }

    template <typename T>
    void removeComponent(const Entity entity) {
        mComponentManager->removeComponent<T>(entity);

        auto pattern = mEntityManager->getPattern(entity);
        std::optional<ComponentType> componentType = mComponentManager->getComponentType<T>();
        if (!componentType) {
            return;
        }
        pattern.set(componentType.value(), false);
        mEntityManager->setPattern(entity, pattern);
        mSystemManager->entityPatternChanged(entity, pattern);
    }

    template <typename T>
    std::optional<T*> getComponent(const Entity entity) const {
        return mComponentManager->getComponent<T>(entity);
    }

    template <typename T>
    std::optional<ComponentType> getComponentType() const {
        return mComponentManager->getComponentType<T>();
    }

    // SYSTEM
    template <typename T>
    std::shared_ptr<T> registerSystem() const {
        return mSystemManager->registerSystem<T>();
    }

    template <typename T>
    void setSystemPattern(Pattern pattern) const {
        mSystemManager->setPattern<T>(pattern);
    }

private:
    ECS();

    ECS(const ECS&) = delete;
    void operator=(const ECS&) = delete;

    std::unique_ptr<EntityManager> mEntityManager;
    std::unique_ptr<ComponentManager> mComponentManager;
    std::unique_ptr<SystemManager> mSystemManager;
};

}  // namespace whal::ecs
