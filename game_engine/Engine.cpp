//
//  Engine.cpp
//  game_engine
//
//  Created by Jack Gaffney on 1/22/25.
//

#include "Engine.hpp"
#include <string>
#include <iostream>
#include <memory>
#include <filesystem>
#include <set>
#include "EngineUtils.hpp"
#include "SceneDB.hpp"
#include "rapidjson/document.h"
#include "Renderer.hpp"
#include "ConfigManager.hpp"
#include "ComponentDB.hpp"
#include "Input.hpp"
#include "TextDB.hpp"
#include "AudioDB.hpp"
#include "ImageDB.hpp"
#include "Time.hpp"
#include "EventSystem.hpp"
#include "Scheduler.hpp"
#include "Tween.hpp"
#include "CollisionLayers.hpp"
#include "AnimationDB.hpp"
#include "ParticleSystem.hpp"
#include "DebugDraw.hpp"
#include "SceneTransition.hpp"


Engine::Engine() {
    cleanColor = ConfigManager::GetClearColor();
    TextDB::Init();
    Input::Init();
    AudioDB::Init();
    ComponentDB::Init();
    Time::Init();
    EventSystem::Init();
    Scheduler::Init();
    Tween::Init();
    CollisionLayers::Init();
    AnimationDB::Init();
    ParticleSystem::Init();
    DebugDraw::Init();
    SceneTransition::Init();
    ImageDB::CreateDefaultParticleTextureWithName("__default_particle");

    scene.loadScene();
}

Engine::~Engine() {
    // Every system that caches Lua references must release them before
    // ComponentDB::Shutdown() closes the Lua state.
    EventSystem::Clear();
    Scheduler::Clear();
    Tween::Clear();
    AnimationDB::Clear();
    ParticleSystem::Clear();
    scene.clearLuaRefs();
    ComponentDB::Shutdown();
    AudioDB::Shutdown();
    TextDB::Shutdown();
}

void Engine::GameLoop(int max_frames) {
    Renderer::clear(cleanColor);

    bool quit = false;
    int frames = 0;
    while (!quit) {
        Input::BeginFrame();

        SDL_Event e;
        while (Helper::SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;

            if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F1) {
                DebugDraw::ToggleEnabled();
            }

            Input::ProcessEvent(e);
        }

        Update();
        Render();

        Input::LateUpdate();

        if (max_frames >= 0 && ++frames >= max_frames) quit = true;
    }
}

void Engine::Update() {
    // Update time at the start of each frame
    Time::Update();

    // Handle scene loading (from direct load or transition)
    bool should_load_scene = !SceneDB::next_scene_to_load.empty();

    if (!should_load_scene && SceneTransition::ShouldLoadScene()) {
        SceneDB::next_scene_to_load = SceneTransition::GetTargetScene();
        SceneTransition::ClearTargetScene();
        should_load_scene = true;
    }

    if (should_load_scene) {
        EventSystem::Clear();
        Scheduler::Clear();
        Tween::Clear();
        AnimationDB::Clear();
        ParticleSystem::Clear();
        scene.loadScene();
    }

    // Update timer and tween systems
    float dt = Time::GetDeltaTime();
    Scheduler::Update(dt);
    Tween::Update(dt);
    AnimationDB::Update(dt);
    ParticleSystem::Update(dt);
    SceneTransition::Update(dt);
    Renderer::UpdateCamera(dt);

    scene.UpdateScene();
}

void Engine::Render() {
    Renderer::clear(cleanColor);

    ImageDB::RenderAndClearAllImages();

    // Render particles (world-space, after images, before text/UI)
    const auto& particle_data = ParticleSystem::GetDrawData();
    if (!particle_data.empty()) {
        float zoom = Renderer::GetCameraZoomFactor();
        glm::ivec2 cam_dims = Renderer::GetCameraDimensions();
        glm::vec2 cam_pos = Renderer::GetEffectiveCameraPosition();
        SDL_RenderSetScale(Renderer::getSDLRenderer(), zoom, zoom);

        for (const auto& p : particle_data) {
            const std::string& tex_name = p.image_name.empty() ? std::string("__default_particle") : p.image_name;
            SDL_Texture* tex = ImageDB::GetTexture(tex_name);
            SDL_SetTextureColorMod(tex, p.r, p.g, p.b);
            SDL_SetTextureAlphaMod(tex, p.a);

            float screen_x = (p.x - cam_pos.x) * 100.0f +
                cam_dims.x * 0.5f * (1.0f / zoom) - p.size * 0.5f;
            float screen_y = (p.y - cam_pos.y) * 100.0f +
                cam_dims.y * 0.5f * (1.0f / zoom) - p.size * 0.5f;

            SDL_FRect rect = { screen_x, screen_y, p.size, p.size };
            SDL_RenderCopyF(Renderer::getSDLRenderer(), tex, NULL, &rect);

            SDL_SetTextureColorMod(tex, 255, 255, 255);
            SDL_SetTextureAlphaMod(tex, 255);
        }
        SDL_RenderSetScale(Renderer::getSDLRenderer(), 1.0f, 1.0f);
    }

    // Rects draw before text so they work as HUD backdrops.
    ImageDB::RenderAndClearAllRects();
    TextDB::RenderQueuedTexts();
    // Pixels draw on top of everything for debug overlays.
    ImageDB::RenderAndClearAllPixels();

    DebugDraw::Render();
    SceneTransition::Render();

    Renderer::present();
}
