#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
// #include <type_traits>
#include <vector>
#include "Util/Traits.h"

#include "ECS/Lib/Component.h"
#include "ECS/Lib/Entity.h"
#include "ECS/Lib/System.h"

namespace whal::ecs {

class Entity;

using SystemId = u16;

class SystemManager {
public:
    template <class T>
    std::shared_ptr<T> registerSystem() {
        static_assert(is_base_of_template<ISystem, T>::value, "Cannot register class which doesn't inherit ISystem");
        const SystemId id = getSystemID<T>();
        auto it = std::find(mSystemIDs.begin(), mSystemIDs.end(), id);
        assert(it == mSystemIDs.end());

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
        assert(it != mSystemIDs.end());
        int ix = std::distance(mSystemIDs.begin(), it);
        mPatterns[ix] = pattern;
    }

    void entityDestroyed(const Entity entity) const {
        // TODO make thread safe
        for (const auto& system : mSystems) {
            system->mEntities.erase(entity.mId);
        }
    }

    void entityPatternChanged(const Entity entity, const Pattern newEntityPattern) const {
        // TODO make thread safe
        for (size_t i = 0; i < mSystemIDs.size(); i++) {
            auto const& systemPattern = mPatterns[i];
            if ((newEntityPattern & systemPattern) == systemPattern) {
                mSystems[i]->mEntities.insert({entity.id(), entity});
            } else {
                mSystems[i]->mEntities.erase(entity.id());
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

}  // namespace whal::ecs
