//
//  Time.hpp
//  game_engine
//
//  Provides frame-independent timing for game logic.
//

#ifndef Time_hpp
#define Time_hpp

#include <chrono>

/**
 * @class Time
 * @brief Static time management class for frame-independent game logic.
 *
 * Provides delta time, time scale, and frame counting functionality.
 * All methods are static following the engine's singleton pattern.
 */
class Time {
public:
    /**
     * @brief Initialize the time system.
     */
    static void Init();

    /**
     * @brief Update time values. Call once per frame at the start of the frame.
     */
    static void Update();

    /**
     * @brief Get the time elapsed since the last frame (affected by time scale).
     * @return Delta time in seconds.
     */
    static float GetDeltaTime() { return delta_time * time_scale; }

    /**
     * @brief Get the unscaled time elapsed since the last frame.
     * @return Unscaled delta time in seconds.
     */
    static float GetUnscaledDeltaTime() { return delta_time; }

    /**
     * @brief Get the current time scale.
     * @return Time scale (1.0 = normal, 0.0 = paused).
     */
    static float GetTimeScale() { return time_scale; }

    /**
     * @brief Set the time scale for slow motion or pause.
     * @param scale Time scale value (1.0 = normal, 0.0 = paused).
     */
    static void SetTimeScale(float scale) { time_scale = scale; }

    /**
     * @brief Get the total time since game start (affected by time scale).
     * @return Total scaled time in seconds.
     */
    static float GetTotalTime() { return total_time; }

    /**
     * @brief Get the total unscaled time since game start.
     * @return Total unscaled time in seconds.
     */
    static float GetUnscaledTotalTime() { return unscaled_total_time; }

    /**
     * @brief Get the fixed timestep used for physics.
     * @return Fixed delta time in seconds.
     */
    static float GetFixedDeltaTime() { return fixed_delta_time; }

    /**
     * @brief Get the current frame number.
     * @return Frame count since game start.
     */
    static int GetFrameCount() { return frame_count; }

private:
    inline static float delta_time = 1.0f / 60.0f;
    inline static float time_scale = 1.0f;
    inline static float fixed_delta_time = 1.0f / 60.0f;
    inline static float total_time = 0.0f;
    inline static float unscaled_total_time = 0.0f;
    inline static int frame_count = 0;

    inline static std::chrono::high_resolution_clock::time_point last_frame_time;
    inline static bool initialized = false;
};

#endif /* Time_hpp */
