#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <functional>
#include <unordered_map>
#include <string>

class EventHandler {
public:
    using EventCallback = std::function<void()>;

    template <typename EventCallback, typename... Args>

    void registerEvent(const std::string& eventName, EventCallback callback, Args&&... args) {

        eventCallbacks[eventName] = [callback, ...args = std::forward<Args>(args)]() {

            callback(args.get()...);

        };

    }

    void triggerEvent(const std::string& eventName) {
        if (eventCallbacks.find(eventName) != eventCallbacks.end()) {
            eventCallbacks[eventName]();
        }
    }

private:
    std::unordered_map<std::string, EventCallback> eventCallbacks;
};

#endif