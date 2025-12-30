//
//  EventSystem.hpp
//  game_engine
//
//  Provides decoupled event-based communication between components.
//

#ifndef EventSystem_hpp
#define EventSystem_hpp

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

/**
 * @struct EventSubscription
 * @brief Represents a single event subscription.
 */
struct EventSubscription {
    int id;
    std::shared_ptr<luabridge::LuaRef> callback;
    bool once;  // If true, auto-unsubscribe after first call
};

/**
 * @class EventSystem
 * @brief Static event system for decoupled inter-component communication.
 *
 * Allows components to emit and subscribe to named events without
 * direct references to each other.
 */
class EventSystem {
public:
    /**
     * @brief Initialize the event system.
     */
    static void Init();

    /**
     * @brief Emit an event to all subscribers.
     * @param event_name Name of the event.
     * @param data Lua table containing event data (can be nil).
     */
    static void Emit(const std::string& event_name, luabridge::LuaRef data);

    /**
     * @brief Subscribe to an event.
     * @param event_name Name of the event to subscribe to.
     * @param callback Lua function to call when event is emitted.
     * @return Subscription ID for later unsubscription.
     */
    static int Subscribe(const std::string& event_name, luabridge::LuaRef callback);

    /**
     * @brief Subscribe to an event, auto-unsubscribing after first trigger.
     * @param event_name Name of the event to subscribe to.
     * @param callback Lua function to call when event is emitted.
     * @return Subscription ID for later unsubscription.
     */
    static int SubscribeOnce(const std::string& event_name, luabridge::LuaRef callback);

    /**
     * @brief Unsubscribe from an event by subscription ID.
     * @param subscription_id ID returned from Subscribe/SubscribeOnce.
     */
    static void Unsubscribe(int subscription_id);

    /**
     * @brief Unsubscribe all callbacks for a specific event.
     * @param event_name Name of the event.
     */
    static void UnsubscribeAll(const std::string& event_name);

    /**
     * @brief Clear all subscriptions. Called on scene change.
     */
    static void Clear();

private:
    inline static std::unordered_map<std::string, std::vector<EventSubscription>> subscriptions;
    inline static std::unordered_map<int, std::string> subscription_to_event;
    inline static int next_subscription_id = 1;
};

#endif /* EventSystem_hpp */
