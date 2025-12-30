//
//  Tween.hpp
//  game_engine
//
//  Provides value interpolation with easing functions.
//

#ifndef Tween_hpp
#define Tween_hpp

#include <string>
#include <vector>
#include <memory>
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

/**
 * @enum EaseType
 * @brief Available easing functions for tweens.
 */
enum class EaseType {
    Linear,
    EaseInQuad,
    EaseOutQuad,
    EaseInOutQuad,
    EaseInCubic,
    EaseOutCubic,
    EaseInOutCubic,
    EaseInSine,
    EaseOutSine,
    EaseInOutSine
};

/**
 * @struct TweenInstance
 * @brief Represents an active tween animation.
 */
struct TweenInstance {
    int id;
    float start_value;
    float end_value;
    float duration;
    float elapsed = 0.0f;
    EaseType ease_type;
    std::shared_ptr<luabridge::LuaRef> on_update;
    std::shared_ptr<luabridge::LuaRef> on_complete;
    bool cancelled = false;
};

/**
 * @class Tween
 * @brief Static tween system for smooth value interpolation.
 *
 * Provides Tween.To() functionality for Lua scripts with various easing functions.
 */
class Tween {
public:
    /**
     * @brief Initialize the tween system.
     */
    static void Init();

    /**
     * @brief Update all active tweens. Call once per frame.
     * @param delta_time Time elapsed since last frame.
     */
    static void Update(float delta_time);

    /**
     * @brief Create a new tween.
     * @param from Starting value.
     * @param to Ending value.
     * @param duration Duration in seconds.
     * @param ease_type Easing function name (e.g., "EaseOutQuad").
     * @param on_update Lua function called with current value each frame.
     * @param on_complete Lua function called when tween completes.
     * @return Tween ID for cancellation.
     */
    static int To(float from, float to, float duration,
                  const std::string& ease_type,
                  luabridge::LuaRef on_update,
                  luabridge::LuaRef on_complete);

    /**
     * @brief Cancel an active tween.
     * @param tween_id ID returned from To().
     */
    static void Cancel(int tween_id);

    /**
     * @brief Cancel all active tweens.
     */
    static void CancelAll();

    /**
     * @brief Clear all tweens. Called on scene change.
     */
    static void Clear();

private:
    inline static std::vector<TweenInstance> tweens;
    inline static int next_tween_id = 1;

    /**
     * @brief Parse easing type from string.
     */
    static EaseType ParseEaseType(const std::string& name);

    /**
     * @brief Apply easing function to normalized time.
     * @param t Normalized time (0-1).
     * @param type Easing type.
     * @return Eased value (0-1).
     */
    static float ApplyEasing(float t, EaseType type);
};

#endif /* Tween_hpp */
