//
//  Time.cpp
//  game_engine
//
//  Created for FR-Ocean Engine.
//

#include "Time.hpp"
#include <algorithm>

void Time::Init() {
    last_frame_time = std::chrono::high_resolution_clock::now();
    delta_time = 1.0f / 60.0f;
    time_scale = 1.0f;
    total_time = 0.0f;
    unscaled_total_time = 0.0f;
    frame_count = 0;
    initialized = true;
}

void Time::Update() {
    if (!initialized) {
        Init();
    }

    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = current_time - last_frame_time;
    last_frame_time = current_time;

    // Clamp delta time to prevent large jumps (e.g., after debugging pause)
    delta_time = std::clamp(elapsed.count(), 0.0001f, 0.25f);

    // Update total times
    unscaled_total_time += delta_time;
    total_time += delta_time * time_scale;

    frame_count++;
}
