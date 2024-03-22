#pragma once

#include <bitset>
#include <optional>
#include <unordered_map>

#include "ECS/Lib/Entity.h"
#include "Util/Types.h"

namespace whal::ecs {

using ComponentType = u16;
const ComponentType MAX_COMPONENTS = 64;

using Pattern = std::bitset<MAX_COMPONENTS>;

// TODO compile-time inheritance?
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
            mComponentTable[mEntityToIndex[entity.id()]] = component;
            return;
        }
        const u32 ix = mSize++;
        mEntityToIndex[entity.id()] = ix;
        mIndexToEntity[ix] = entity.id();
        mComponentTable[ix] = component;
    }

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

    std::optional<T> getData(const Entity entity) const {
        if (mEntityToIndex.find(entity.id()) == mEntityToIndex.end()) {
            return std::nullopt;
        }
        const u32 ix = mEntityToIndex.at(entity.id());
        return mComponentTable.at(ix);
    }

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

}  // namespace whal::ecs
