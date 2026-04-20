//
//  SceneTransition.cpp
//  game_engine
//
//  Scene transition effects (fade to black, instant cut).
//

#include "SceneTransition.hpp"
#include "Renderer.hpp"
#include "SDL2/SDL.h"
#include <algorithm>

void SceneTransition::Init() {
    state = State::NONE;
    target_scene.clear();
    transition_type = "none";
    duration = 0.0f;
    elapsed = 0.0f;
    scene_load_pending = false;
}

void SceneTransition::StartTransition(const std::string& scene_name, const std::string& type, float dur) {
    target_scene = scene_name;
    transition_type = type;
    scene_load_pending = false;

    if (type == "fade" && dur > 0.0f) {
        state = State::FADE_OUT;
        duration = dur;
        elapsed = 0.0f;
    } else {
        // Instant transition
        state = State::NONE;
        duration = 0.0f;
        elapsed = 0.0f;
        scene_load_pending = true;
    }
}

void SceneTransition::Update(float dt) {
    if (state == State::NONE) {
        return;
    }

    elapsed += dt;

    if (state == State::FADE_OUT) {
        float half = duration * 0.5f;
        if (elapsed >= half) {
            scene_load_pending = true;
            state = State::FADE_IN;
            // Don't reset elapsed; it continues accumulating through full duration
        }
    } else if (state == State::FADE_IN) {
        if (elapsed >= duration) {
            state = State::NONE;
            elapsed = 0.0f;
        }
    }
}

void SceneTransition::Render() {
    if (state == State::NONE) {
        return;
    }

    SDL_Renderer* sdl_renderer = Renderer::getSDLRenderer();

    // Temporarily reset render scale for full-screen overlay
    float zoom = Renderer::GetCameraZoomFactor();
    SDL_RenderSetScale(sdl_renderer, 1.0f, 1.0f);

    float half = duration * 0.5f;
    Uint8 alpha = 0;

    if (state == State::FADE_OUT) {
        // Alpha goes 0 -> 255 during first half
        float progress = (half > 0.0f) ? std::min(elapsed / half, 1.0f) : 1.0f;
        alpha = (Uint8)(progress * 255.0f);
    } else if (state == State::FADE_IN) {
        // Alpha goes 255 -> 0 during second half
        float progress = (half > 0.0f) ? std::min((elapsed - half) / half, 1.0f) : 1.0f;
        alpha = (Uint8)((1.0f - progress) * 255.0f);
    }

    SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, alpha);

    glm::ivec2 dims = Renderer::GetCameraDimensions();
    SDL_Rect fullscreen = {0, 0, dims.x, dims.y};
    SDL_RenderFillRect(sdl_renderer, &fullscreen);

    // Restore render scale
    SDL_RenderSetScale(sdl_renderer, zoom, zoom);
}

bool SceneTransition::IsTransitioning() {
    return state != State::NONE;
}

bool SceneTransition::ShouldLoadScene() {
    return scene_load_pending;
}

std::string SceneTransition::GetTargetScene() {
    return target_scene;
}

void SceneTransition::ClearTargetScene() {
    scene_load_pending = false;
}
