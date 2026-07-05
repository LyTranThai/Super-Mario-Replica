#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <vector>
#include <map>
#include <algorithm>

enum class EventType {
    PlayerJump,
    PlayerHurt,
    PlayerDied,
    CoinCollected,
    EnemyStomped,
    BrickBroken,
    LevelCompleted,
    FireballShot,
    ItemSpawned
};

class EventListener {
public:
    virtual ~EventListener() = default;
    virtual void onEvent(EventType type, void* data = nullptr) = 0;
};

class EventManager {
private:
    std::map<EventType, std::vector<EventListener*>> listeners;

    EventManager() = default;

public:
    static EventManager& getInstance() {
        static EventManager instance;
        return instance;
    }

    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    void subscribe(EventType type, EventListener* listener) {
        auto& list = listeners[type];
        if (std::find(list.begin(), list.end(), listener) == list.end()) {
            list.push_back(listener);
        }
    }

    void unsubscribe(EventType type, EventListener* listener) {
        auto& list = listeners[type];
        auto it = std::find(list.begin(), list.end(), listener);
        if (it != list.end()) {
            list.erase(it);
        }
    }

    void broadcast(EventType type, void* data = nullptr) {
        auto it = listeners.find(type);
        if (it != listeners.end()) {
            // Copy list to prevent issues if a listener unsubscribes during callback
            auto listCopy = it->second;
            for (auto* listener : listCopy) {
                if (listener) {
                    listener->onEvent(type, data);
                }
            }
        }
    }
};

#endif // EVENT_SYSTEM_H
