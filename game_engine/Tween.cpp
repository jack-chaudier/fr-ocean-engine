//
//  Tween.cpp
//  game_engine
//
//  Created for FR-Ocean Engine.
//

#include "Tween.hpp"
#include "Logger.hpp"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void Tween::Init() {
    tweens.clear();
    next_tween_id = 1;
}

void Tween::Update(float delta_time) {
    std::vector<int> to_remove;

    for (auto& tween : tweens) {
        if (tween.cancelled) {
            to_remove.push_back(tween.id);
            continue;
        }

        tween.elapsed += delta_time;
        float t = std::min(tween.elapsed / tween.duration, 1.0f);
        float eased_t = ApplyEasing(t, tween.ease_type);

        float current_value = tween.start_value + (tween.end_value - tween.start_value) * eased_t;

        // Call on_update callback
        try {
            if (tween.on_update && tween.on_update->isFunction()) {
                (*tween.on_update)(current_value);
            }
        }
        catch (luabridge::LuaException& e) {
            LOG_ERROR("Tween on_update error: " + std::string(e.what()));
        }

        // Check if complete
        if (t >= 1.0f) {
            try {
                if (tween.on_complete && tween.on_complete->isFunction()) {
                    (*tween.on_complete)();
                }
            }
            catch (luabridge::LuaException& e) {
                LOG_ERROR("Tween on_complete error: " + std::string(e.what()));
            }
            to_remove.push_back(tween.id);
        }
    }

    // Remove completed/cancelled tweens
    tweens.erase(
        std::remove_if(tweens.begin(), tweens.end(),
            [&to_remove](const TweenInstance& tw) {
                return std::find(to_remove.begin(), to_remove.end(), tw.id) != to_remove.end();
            }),
        tweens.end()
    );
}

int Tween::To(float from, float to, float duration,
              const std::string& ease_type,
              luabridge::LuaRef on_update,
              luabridge::LuaRef on_complete) {

    int id = next_tween_id++;

    TweenInstance tween;
    tween.id = id;
    tween.start_value = from;
    tween.end_value = to;
    tween.duration = duration;
    tween.elapsed = 0.0f;
    tween.ease_type = ParseEaseType(ease_type);
    tween.cancelled = false;

    if (on_update.isFunction()) {
        tween.on_update = std::make_shared<luabridge::LuaRef>(on_update);
    }

    if (on_complete.isFunction()) {
        tween.on_complete = std::make_shared<luabridge::LuaRef>(on_complete);
    }

    tweens.push_back(tween);

    return id;
}

void Tween::Cancel(int tween_id) {
    for (auto& tween : tweens) {
        if (tween.id == tween_id) {
            tween.cancelled = true;
            return;
        }
    }
}

void Tween::CancelAll() {
    for (auto& tween : tweens) {
        tween.cancelled = true;
    }
}

void Tween::Clear() {
    tweens.clear();
}

EaseType Tween::ParseEaseType(const std::string& name) {
    if (name == "Linear") return EaseType::Linear;
    if (name == "EaseInQuad") return EaseType::EaseInQuad;
    if (name == "EaseOutQuad") return EaseType::EaseOutQuad;
    if (name == "EaseInOutQuad") return EaseType::EaseInOutQuad;
    if (name == "EaseInCubic") return EaseType::EaseInCubic;
    if (name == "EaseOutCubic") return EaseType::EaseOutCubic;
    if (name == "EaseInOutCubic") return EaseType::EaseInOutCubic;
    if (name == "EaseInSine") return EaseType::EaseInSine;
    if (name == "EaseOutSine") return EaseType::EaseOutSine;
    if (name == "EaseInOutSine") return EaseType::EaseInOutSine;
    return EaseType::Linear;  // Default
}

float Tween::ApplyEasing(float t, EaseType type) {
    switch (type) {
        case EaseType::Linear:
            return t;

        case EaseType::EaseInQuad:
            return t * t;

        case EaseType::EaseOutQuad:
            return t * (2 - t);

        case EaseType::EaseInOutQuad:
            return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;

        case EaseType::EaseInCubic:
            return t * t * t;

        case EaseType::EaseOutCubic: {
            float f = t - 1;
            return f * f * f + 1;
        }

        case EaseType::EaseInOutCubic:
            return t < 0.5f
                ? 4 * t * t * t
                : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;

        case EaseType::EaseInSine:
            return 1 - std::cos(t * M_PI / 2);

        case EaseType::EaseOutSine:
            return std::sin(t * M_PI / 2);

        case EaseType::EaseInOutSine:
            return -(std::cos(M_PI * t) - 1) / 2;

        default:
            return t;
    }
}
