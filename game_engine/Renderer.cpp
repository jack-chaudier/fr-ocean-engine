//
//  Renderer.cpp
//  game_engine
//
//  Created by Jack Gaffney on 2/7/25.
//

#include "Renderer.hpp"
#include "Helper.h"
#include "Logger.hpp"
#include "EngineException.hpp"
#include "Actor.hpp"
#include "Rigidbody.hpp"
#include <iostream>
#include <cstdlib>

Renderer::Renderer(const std::string& title, glm::ivec3& clearColor, const glm::ivec2& resolution)
    : title(title), clearColor(clearColor), resolution(resolution) {
    window = Helper::SDL_CreateWindow(title.c_str(), 100, 100, resolution.x, resolution.y, SDL_WINDOW_SHOWN);
    if (!window) {
        std::string error = SDL_GetError();
        LOG_FATAL("Failed to create SDL window: " + error);
        throw RenderException("Failed to create SDL window: " + error);
    }

    renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::string error = SDL_GetError();
        LOG_FATAL("Failed to create SDL renderer: " + error);
        throw RenderException("Failed to create SDL renderer: " + error);
    }

    // Initialize camera dimensions with the window resolution
    camera_dimensions = resolution;
}

void Renderer::clear(glm::ivec3 color) {
    SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 0);
    SDL_RenderClear(renderer);
}

void Renderer::present() {
    Helper::SDL_RenderPresent(renderer);
}

Renderer::~Renderer() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
}

void Renderer::Follow(Actor* target, float lerp_speed) {
    follow_target = target;
    follow_lerp_speed = lerp_speed;
}

void Renderer::StopFollow() {
    follow_target = nullptr;
}

void Renderer::Shake(float intensity, float duration) {
    shake_intensity = intensity;
    shake_duration = duration;
    shake_elapsed = 0.0f;
}

void Renderer::SetBounds(float min_x, float min_y, float max_x, float max_y) {
    has_bounds = true;
    bounds_min = {min_x, min_y};
    bounds_max = {max_x, max_y};
}

void Renderer::ClearBounds() {
    has_bounds = false;
}

void Renderer::UpdateCamera(float dt) {
    // Camera follow
    if (follow_target && !follow_target->destroyed) {
        glm::vec2 target_pos;

        // Try to get position from Rigidbody if available
        luabridge::LuaRef rb_ref = follow_target->GetComponent("Rigidbody");
        if (rb_ref.isUserdata()) {
            Rigidbody* rb = rb_ref.cast<Rigidbody*>();
            if (rb) {
                b2Vec2 pos = rb->GetPosition();
                target_pos = {pos.x, pos.y};
            }
        } else {
            target_pos = camera_pos;
        }

        float t = 1.0f - std::exp(-follow_lerp_speed * dt);
        camera_pos.x += (target_pos.x - camera_pos.x) * t;
        camera_pos.y += (target_pos.y - camera_pos.y) * t;
    }

    // Bounds clamping
    if (has_bounds) {
        if (camera_pos.x < bounds_min.x) camera_pos.x = bounds_min.x;
        if (camera_pos.x > bounds_max.x) camera_pos.x = bounds_max.x;
        if (camera_pos.y < bounds_min.y) camera_pos.y = bounds_min.y;
        if (camera_pos.y > bounds_max.y) camera_pos.y = bounds_max.y;
    }

    // Screen shake
    if (shake_elapsed < shake_duration) {
        shake_elapsed += dt;
        float progress = shake_elapsed / shake_duration;
        float current_intensity = shake_intensity * (1.0f - progress);
        shake_offset.x = (static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f) * current_intensity;
        shake_offset.y = (static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f) * current_intensity;
    } else {
        shake_offset = {0.0f, 0.0f};
    }
}

glm::vec2 Renderer::GetEffectiveCameraPosition() {
    return camera_pos + shake_offset;
}
