#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <memory>
#include <optional>
#include <queue>
#include <unordered_map>
#include <vector>

#include "Util/Expected.h"
#include "Util/Traits.h"
#include "Util/Types.h"

namespace whal::ecs {

class EntityManager;
class SystemManager;

using EntityID = u32;
const u32 MAX_ENTITIES = 5000;

using ComponentType = u16;
const ComponentType MAX_COMPONENTS = 64;

using Pattern = std::bitset<MAX_COMPONENTS>;

using SystemId = u16;

class Entity {
public:
    Entity() = default;
    Entity(EntityID id);
    friend EntityManager;
    friend SystemManager;
    friend Expected<Entity>;  // create dummy id on error
    EntityID id() const { return mId; }

    EntityID operator()() const { return mId; }
    bool operator==(Entity other) const { return mId == other.mId; }

    template <typename T>
    void add(T component);

    template <typename T>
    void add();

    // set value of component that's been added
    template <typename T>
    void set(T component);

    template <typename T>
    void remove();

    template <typename T>
    std::optional<T*> tryGet() const;

    template <typename T>
    T& get() const;

    template <typename T>
    bool has() const;

    void kill();

private:
    EntityID mId = 0;
};

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void entityDestroyed(Entity entity) = 0;
};

// maintains dense component data
template <typename T>
class ComponentArray : public IComponentArray {
public:
    void addData(const Entity entity, T component) {
        if (mEntityToIndex.find(entity.id()) != mEntityToIndex.end()) {
            const u32 ix = mEntityToIndex[entity.id()];
            mComponentTable[ix] = component;
            return;
        }
        const u32 ix = mSize++;
        mEntityToIndex[entity.id()] = ix;
        mIndexToEntity[ix] = entity.id();
        mComponentTable[ix] = component;
    }

    void setData(const Entity entity, T component) { mComponentTable[mEntityToIndex[entity.id()]] = component; }

    void removeData(const Entity entity) {
        if (mEntityToIndex.find(entity.id()) == mEntityToIndex.end()) {
            return;
        }

        // maintain density of entities
        const u32 removeIx = mEntityToIndex[entity.id()];
        const u32 lastIx = --mSize;
        mComponentTable[removeIx] = mComponentTable[lastIx];

        Entity lastEntity = mIndexToEntity[lastIx];
        mEntityToIndex[lastEntity.id()] = removeIx;
        mIndexToEntity[removeIx] = lastEntity.id();

        mEntityToIndex.erase(entity.id());
        mIndexToEntity.erase(lastIx);
    }

    bool hasData(const Entity entity) const { return mEntityToIndex.find(entity.id()) != mEntityToIndex.end(); }

    std::optional<T*> tryGetData(const Entity entity) {
        if (mEntityToIndex.find(entity.id()) == mEntityToIndex.end()) {
            return std::nullopt;
        }
        const u32 ix = mEntityToIndex.at(entity.id());
        return &mComponentTable.at(ix);
    }

    T& getData(const Entity entity) {
        const u32 ix = mEntityToIndex.at(entity.id());
        return mComponentTable.at(ix);
    }

    // TODO does this need to be virtual?
    void entityDestroyed(const Entity entity) override {
        if (mEntityToIndex.find(entity.id()) == mEntityToIndex.end()) {
            return;
        }
        removeData(entity);
    }

private:
    std::array<T, MAX_ENTITIES> mComponentTable;
    // could use arrays here:
    std::unordered_map<EntityID, u32> mEntityToIndex;
    std::unordered_map<u32, EntityID> mIndexToEntity;
    u32 mSize;
};

// TODO a way to temporarily deactivate a component without changing the pattern?
class EntityManager {
public:
    EntityManager();

    Expected<Entity> createEntity();
    void destroyEntity(Entity entity);
    void setPattern(Entity entity, Pattern pattern);
    Pattern getPattern(Entity entity) const;

private:
    // ? priority queue to ensure entities are packed? makes insert + delete nlogn
    std::queue<EntityID> mAvailableIDs;
    std::array<Pattern, MAX_ENTITIES> mPatterns;
    u32 mEntityCount = 0;
};

class ComponentManager {
public:
    template <typename T>
    void registerComponent() {
        const ComponentType type = getComponentID<T>();
        assert(std::find(mComponentTypes.begin(), mComponentTypes.end(), type) == mComponentTypes.end());  // msg: component type already registered
        mComponentTypes.push_back(type);
        mComponentArrays.push_back(std::make_shared<ComponentArray<T>>());
    }

    template <typename T>
    std::optional<ComponentType> tryGetComponentType() const {
        const ComponentType type = getComponentID<T>();
        auto it = std::find(mComponentTypes.begin(), mComponentTypes.end(), type);
        if (it == mComponentTypes.end()) {
            return std::nullopt;
        }
        return type;
    }

    template <typename T>
    ComponentType getComponentType() const {
        return getComponentID<T>();
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
    void setComponent(const Entity entity, T component) {
        const ComponentType type = getComponentID<T>();
        auto it = std::find(mComponentTypes.begin(), mComponentTypes.end(), type);
        int ix = std::distance(mComponentTypes.begin(), it);
        getComponentArray<T>(ix)->setData(entity, component);
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
    bool hasComponent(const Entity entity) const {
        const ComponentType type = getComponentID<T>();
        auto it = std::find(mComponentTypes.begin(), mComponentTypes.end(), type);
        int ix = std::distance(mComponentTypes.begin(), it);
        return getComponentArray<T>(ix)->hasData(entity);
    }

    template <typename T>
    std::optional<T*> tryGetComponent(const Entity entity) const {
        const ComponentType type = getComponentID<T>();
        auto it = std::find(mComponentTypes.begin(), mComponentTypes.end(), type);
        if (it == mComponentTypes.end()) {
            return std::nullopt;
        }
        int ix = std::distance(mComponentTypes.begin(), it);
        return getComponentArray<T>(ix)->tryGetData(entity);
    }

    template <typename T>
    T& getComponent(const Entity entity) const {
        const ComponentType type = getComponentID<T>();
        auto it = std::find(mComponentTypes.begin(), mComponentTypes.end(), type);
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

class SystemBase {
public:
    friend SystemManager;
    virtual ~SystemBase() = default;
    virtual void update(){};
    virtual void onAdd(const Entity){};
    virtual void onRemove(const Entity){};

protected:
    std::unordered_map<EntityID, Entity> getEntities() const { return mEntities; }

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

class SystemManager {
public:
    template <class T>
    std::shared_ptr<T> registerSystem() {
        static_assert(is_base_of_template<ISystem, T>::value, "Cannot register class which doesn't inherit ISystem");
        const SystemId id = getSystemID<T>();
        auto it = std::find(mSystemIDs.begin(), mSystemIDs.end(), id);
        assert(it == mSystemIDs.end());  // msg: system already registered

        mSystemIDs.push_back(id);
        auto system = std::make_shared<T>();
        mPatterns.push_back(system->getPattern());
        mSystems.push_back(system);
        return system;
    }

    template <class T>
    void setPattern(const Pattern pattern) {
        const SystemId id = getSystemID<T>();
        auto it = std::find(mSystemIDs.begin(), mSystemIDs.end(), id);
        assert(it != mSystemIDs.end());  // msg: can't set pattern for unregistered system
        int ix = std::distance(mSystemIDs.begin(), it);
        mPatterns[ix] = pattern;
    }

    void entityDestroyed(const Entity entity) const {
        // TODO make thread safe
        for (const auto& system : mSystems) {
            auto const ix = system->mEntities.find(entity.id());
            if (ix != system->mEntities.end()) {
                system->mEntities.erase(entity.id());
                system->onRemove(entity);
            }
        }
    }

    void entityPatternChanged(const Entity entity, const Pattern newEntityPattern) const {
        // TODO make thread safe
        for (size_t i = 0; i < mSystemIDs.size(); i++) {
            auto const& systemPattern = mPatterns[i];
            auto const ix = mSystems[i]->mEntities.find(entity.id());
            if ((newEntityPattern & systemPattern) == systemPattern) {
                if (ix != mSystems[i]->mEntities.end()) {
                    // already in system
                    continue;
                }
                mSystems[i]->mEntities.insert({entity.id(), entity});
                mSystems[i]->onAdd(entity);
            } else if (ix != mSystems[i]->mEntities.end()) {
                mSystems[i]->mEntities.erase(entity.id());
                mSystems[i]->onRemove(entity);
            }
        }
    }

private:
    // assign unique IDs to each system type
    static inline SystemId SystemID = 0;
    template <class T>
    static inline SystemId getSystemID() {
        static SystemId id = SystemID++;
        return id;
    }

    std::vector<SystemId> mSystemIDs;
    std::vector<Pattern> mPatterns;
    std::vector<std::shared_ptr<SystemBase>> mSystems;
};

class ECS {
public:
    inline static ECS& getInstance() {
        static ECS instance;
        return instance;
    }

    // ENTITY
    Expected<Entity> entity() const;
    void kill(Entity entity) const;

    // COMPONENT
    template <typename T>
    void addComponent(const Entity entity, T component) {
        mComponentManager->addComponent(entity, component);

        auto pattern = mEntityManager->getPattern(entity);
        pattern.set(mComponentManager->getComponentType<T>(), true);
        mEntityManager->setPattern(entity, pattern);
        mSystemManager->entityPatternChanged(entity, pattern);
    }

    template <typename T>
    void setComponent(const Entity entity, T component) {
        mComponentManager->setComponent(entity, component);
    }

    template <typename T>
    void removeComponent(const Entity entity) {
        mComponentManager->removeComponent<T>(entity);

        auto pattern = mEntityManager->getPattern(entity);
        pattern.set(mComponentManager->getComponentType<T>(), false);
        mEntityManager->setPattern(entity, pattern);
        mSystemManager->entityPatternChanged(entity, pattern);
    }

    template <typename T>
    bool hasComponent(const Entity entity) {
        return mComponentManager->hasComponent<T>(entity);
    }

    template <typename T>
    std::optional<T*> tryGetComponent(const Entity entity) const {
        return mComponentManager->tryGetComponent<T>(entity);
    }

    template <typename T>
    T& getComponent(const Entity entity) const {
        return mComponentManager->getComponent<T>(entity);
    }

    template <typename T>
    ComponentType getComponentType() const {
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

    // no copy
    ECS(const ECS&) = delete;
    void operator=(const ECS&) = delete;

    std::unique_ptr<EntityManager> mEntityManager;
    std::unique_ptr<ComponentManager> mComponentManager;
    std::unique_ptr<SystemManager> mSystemManager;
};

template <typename T>
void Entity::add(T component) {
    ECS::getInstance().addComponent<T>(*this, component);
}

template <typename T>
void Entity::add() {
    ECS::getInstance().addComponent<T>(*this, T());
}

template <typename T>
void Entity::set(T component) {
    ECS::getInstance().setComponent<T>(*this, component);
}

template <typename T>
void Entity::remove() {
    ECS::getInstance().removeComponent<T>(*this);
}

template <typename T>
std::optional<T*> Entity::tryGet() const {
    return ECS::getInstance().tryGetComponent<T>(*this);
}

template <typename T>
T& Entity::get() const {
    return ECS::getInstance().getComponent<T>(*this);
}

template <typename T>
bool Entity::has() const {
    return ECS::getInstance().hasComponent<T>(*this);
}

}  // namespace whal::ecs
