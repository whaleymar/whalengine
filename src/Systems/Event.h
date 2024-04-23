#pragma once

#include <vector>

#include "Util/Types.h"

namespace whal {

using EventId = s32;
using ListenerId = s32;
class EventManager;

class EventBase {
public:
    EventBase() { mId = S_EVENT_ID++; }
    EventId id() const { return mId; }

private:
    inline static EventId S_EVENT_ID = 0;
    EventId mId;
};

template <typename... T>
class Event : public EventBase {
public:
    Event() : EventBase() {}
};

template <typename... T>
class EventListener {
public:
    friend EventManager;
    using callbackFunc = void (*)(T... args);
    EventListener(callbackFunc func) : mCallback(func) {}

    void callback(T... args) { mCallback(args...); }
    ListenerId id() const { return mId; }

private:
    callbackFunc mCallback;
    ListenerId mId = 0;
};

class EventManager {
public:
    EventManager() = default;

    template <typename... T>
    void registerListener(Event<T...> event, EventListener<T...>& listener) {
        listener.mId = S_LISTENER_ID++;
        mEvents.push_back({event, &listener});
    }

    template <typename... T>
    void stopListening(Event<T...> event, EventListener<T...>& listener) {
        for (size_t i = 0; i < mEvents.size(); i++) {
            EventListener<T...>* eventListener = static_cast<EventListener<T...>*>(mEvents[i].second);
            if (mEvents[i].first.id() == event.id() && eventListener->id() == listener.id()) {
                removeListenerAt(i);
            }
        }
    }

    template <typename... T>
    void triggerEvent(Event<T...> newEvent, T... args) {
        for (auto& [event, listener] : mEvents) {
            if (event.id() != newEvent.id()) {
                continue;
            }

            EventListener<T...>* eventListener = static_cast<EventListener<T...>*>(listener);
            eventListener->callback(args...);
        }
    }

private:
    void removeListenerAt(int ix) {
        std::pair<EventBase, void*> last = mEvents.back();
        mEvents[ix] = last;
        mEvents.pop_back();
    }

    // RESEARCH can do a std::unordered_map<EventBase, std::vector<void*>> if this gets too slow
    std::vector<std::pair<EventBase, void*>> mEvents;
    inline static ListenerId S_LISTENER_ID = 1;
};

}  // namespace whal
