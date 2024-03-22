#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
#include <type_traits>
#include <vector>

#include "ECS/Lib/Component.h"
#include "ECS/Lib/Entity.h"
#include "ECS/Lib/System.h"

namespace whal::ecs {

class Entity;

using SystemId = u16;

// TODO assert T is base of system
class SystemManager {
public:
    SystemManager() {
        mPatterns = {};
        mSystems = {};
    }

    template <class T>
    std::shared_ptr<T> registerSystem() {
        static_assert(std::is_base_of_v<ISystem, T>, "Cannot register class which doesn't inherit ISystem");
        const SystemId id = getSystemID<T>();
        auto it = std::find(mSystemIDs.begin(), mSystemIDs.end(), id);
        assert(it == mSystemIDs.end());

        mSystemIDs.push_back(id);
        mPatterns.push_back(Pattern());
        auto system = std::make_shared<T>();
        mSystems.push_back(system);
    }

    template <class T>
    void setPattern(const Pattern pattern) {
        const SystemId id = getSystemID<T>();
        auto it = std::find(mSystemIDs.begin(), mSystemIDs.end(), id);
        assert(it != mSystemIDs.end());
        int ix = std::distance(mSystemIDs.begin(), it);
        mPatterns[ix] = pattern;
    }

    void entityDestroyed(const Entity entity) const {
        for (const auto& system : mSystems) {
            system->mEntities.erase(entity.mId);
        }
    }

    void entityPatternChanged(const Entity entity, const Pattern newEntityPattern) const {
        for (size_t i = 0; i < mSystemIDs.size(); i++) {
            auto const& systemPattern = mPatterns[i];
            if ((newEntityPattern & systemPattern) == systemPattern) {
                mSystems[i]->mEntities.insert(entity.mId);
            } else {
                mSystems[i]->mEntities.erase(entity.mId);
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
    std::vector<std::shared_ptr<ISystem>> mSystems;
};

}  // namespace whal::ecs
