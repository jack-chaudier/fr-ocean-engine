//
//  Scheduler.hpp
//  game_engine
//
//  Provides delayed and repeating task execution.
//

#ifndef Scheduler_hpp
#define Scheduler_hpp

#include <string>
#include <vector>
#include <memory>
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

/**
 * @struct ScheduledTask
 * @brief Represents a scheduled task for delayed execution.
 */
struct ScheduledTask {
    int id;
    float delay;           // Time remaining until first execution
    float interval;        // Time between repeats (0 = one-shot)
    int repeat_count;      // -1 = infinite, 0 = done, >0 = remaining repeats
    std::shared_ptr<luabridge::LuaRef> callback;
    bool cancelled = false;
};

/**
 * @class Scheduler
 * @brief Static scheduler for delayed and repeating task execution.
 *
 * Provides Timer.After() and Timer.Every() functionality for Lua scripts.
 */
class Scheduler {
public:
    /**
     * @brief Initialize the scheduler.
     */
    static void Init();

    /**
     * @brief Update the scheduler. Call once per frame.
     * @param delta_time Time elapsed since last frame.
     */
    static void Update(float delta_time);

    /**
     * @brief Schedule a one-shot delayed callback.
     * @param delay Delay in seconds before execution.
     * @param callback Lua function to call.
     * @return Task ID for cancellation.
     */
    static int After(float delay, luabridge::LuaRef callback);

    /**
     * @brief Schedule a repeating callback.
     * @param delay Initial delay before first execution.
     * @param interval Time between subsequent executions.
     * @param callback Lua function to call.
     * @return Task ID for cancellation.
     */
    static int Every(float delay, float interval, luabridge::LuaRef callback);

    /**
     * @brief Cancel a scheduled task.
     * @param task_id ID returned from After/Every.
     */
    static void Cancel(int task_id);

    /**
     * @brief Cancel all scheduled tasks.
     */
    static void CancelAll();

    /**
     * @brief Clear all tasks. Called on scene change.
     */
    static void Clear();

private:
    inline static std::vector<ScheduledTask> tasks;
    inline static int next_task_id = 1;
};

#endif /* Scheduler_hpp */
