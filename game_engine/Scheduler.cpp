//
//  Scheduler.cpp
//  game_engine
//
//  Created for FR-Ocean Engine.
//

#include "Scheduler.hpp"
#include "Logger.hpp"
#include <algorithm>

void Scheduler::Init() {
    tasks.clear();
    next_task_id = 1;
}

void Scheduler::Update(float delta_time) {
    std::vector<int> to_remove;

    for (auto& task : tasks) {
        if (task.cancelled) {
            to_remove.push_back(task.id);
            continue;
        }

        task.delay -= delta_time;

        if (task.delay <= 0) {
            // Execute callback
            try {
                if (task.callback && task.callback->isFunction()) {
                    (*task.callback)();
                }
            }
            catch (luabridge::LuaException& e) {
                LOG_ERROR("Timer callback error: " + std::string(e.what()));
            }

            if (task.interval > 0 && task.repeat_count != 0) {
                // Reset for next execution
                task.delay = task.interval;
                if (task.repeat_count > 0) {
                    task.repeat_count--;
                }
            } else {
                // One-shot task or finished repeating
                to_remove.push_back(task.id);
            }
        }
    }

    // Remove completed/cancelled tasks
    tasks.erase(
        std::remove_if(tasks.begin(), tasks.end(),
            [&to_remove](const ScheduledTask& task) {
                return std::find(to_remove.begin(), to_remove.end(), task.id) != to_remove.end();
            }),
        tasks.end()
    );
}

int Scheduler::After(float delay, luabridge::LuaRef callback) {
    if (!callback.isFunction()) {
        LOG_WARNING("Timer.After: callback is not a function");
        return 0;
    }

    int id = next_task_id++;

    ScheduledTask task;
    task.id = id;
    task.delay = delay;
    task.interval = 0;
    task.repeat_count = 0;
    task.callback = std::make_shared<luabridge::LuaRef>(callback);
    task.cancelled = false;

    tasks.push_back(task);

    return id;
}

int Scheduler::Every(float delay, float interval, luabridge::LuaRef callback) {
    if (!callback.isFunction()) {
        LOG_WARNING("Timer.Every: callback is not a function");
        return 0;
    }

    int id = next_task_id++;

    ScheduledTask task;
    task.id = id;
    task.delay = delay;
    task.interval = interval;
    task.repeat_count = -1;  // Infinite repeats
    task.callback = std::make_shared<luabridge::LuaRef>(callback);
    task.cancelled = false;

    tasks.push_back(task);

    return id;
}

void Scheduler::Cancel(int task_id) {
    for (auto& task : tasks) {
        if (task.id == task_id) {
            task.cancelled = true;
            return;
        }
    }
}

void Scheduler::CancelAll() {
    for (auto& task : tasks) {
        task.cancelled = true;
    }
}

void Scheduler::Clear() {
    tasks.clear();
}
