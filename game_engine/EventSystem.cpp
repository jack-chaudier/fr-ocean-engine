//
//  EventSystem.cpp
//  game_engine
//
//  Created for FR-Ocean Engine.
//

#include "EventSystem.hpp"
#include "Logger.hpp"

void EventSystem::Init() {
    subscriptions.clear();
    subscription_to_event.clear();
    next_subscription_id = 1;
}

void EventSystem::Emit(const std::string& event_name, luabridge::LuaRef data) {
    auto it = subscriptions.find(event_name);
    if (it == subscriptions.end()) return;

    // Collect IDs to remove after iteration (for once-subscriptions)
    std::vector<int> to_remove;

    // Copy the vector to avoid modification during iteration
    auto subs_copy = it->second;

    for (const auto& sub : subs_copy) {
        try {
            if (sub.callback && sub.callback->isFunction()) {
                (*sub.callback)(data);
            }
        }
        catch (luabridge::LuaException& e) {
            LOG_ERROR("Event callback error: " + std::string(e.what()));
        }

        if (sub.once) {
            to_remove.push_back(sub.id);
        }
    }

    // Remove once-subscriptions
    for (int id : to_remove) {
        Unsubscribe(id);
    }
}

int EventSystem::Subscribe(const std::string& event_name, luabridge::LuaRef callback) {
    if (!callback.isFunction()) {
        LOG_WARNING("Event.Subscribe: callback is not a function");
        return 0;
    }

    int id = next_subscription_id++;

    EventSubscription sub;
    sub.id = id;
    sub.callback = std::make_shared<luabridge::LuaRef>(callback);
    sub.once = false;

    subscriptions[event_name].push_back(sub);
    subscription_to_event[id] = event_name;

    return id;
}

int EventSystem::SubscribeOnce(const std::string& event_name, luabridge::LuaRef callback) {
    if (!callback.isFunction()) {
        LOG_WARNING("Event.SubscribeOnce: callback is not a function");
        return 0;
    }

    int id = next_subscription_id++;

    EventSubscription sub;
    sub.id = id;
    sub.callback = std::make_shared<luabridge::LuaRef>(callback);
    sub.once = true;

    subscriptions[event_name].push_back(sub);
    subscription_to_event[id] = event_name;

    return id;
}

void EventSystem::Unsubscribe(int subscription_id) {
    auto event_it = subscription_to_event.find(subscription_id);
    if (event_it == subscription_to_event.end()) return;

    const std::string& event_name = event_it->second;

    auto subs_it = subscriptions.find(event_name);
    if (subs_it != subscriptions.end()) {
        auto& subs = subs_it->second;
        subs.erase(
            std::remove_if(subs.begin(), subs.end(),
                [subscription_id](const EventSubscription& sub) {
                    return sub.id == subscription_id;
                }),
            subs.end()
        );
    }

    subscription_to_event.erase(event_it);
}

void EventSystem::UnsubscribeAll(const std::string& event_name) {
    auto it = subscriptions.find(event_name);
    if (it == subscriptions.end()) return;

    // Remove from lookup map
    for (const auto& sub : it->second) {
        subscription_to_event.erase(sub.id);
    }

    subscriptions.erase(it);
}

void EventSystem::Clear() {
    subscriptions.clear();
    subscription_to_event.clear();
}
