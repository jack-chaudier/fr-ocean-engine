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
#include "EngineUtils.h"
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

    scene.loadScene();
}

Engine::~Engine() {
    scene.clearLuaRefs();
}

void Engine::GameLoop() {
    Renderer::clear(cleanColor);
    
    bool quit = false;
    while (!quit) {
        Input::BeginFrame();
        
        SDL_Event e;
        while (Helper::SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            
            Input::ProcessEvent(e);
        }

        Update();
        Render();
        
        Input::LateUpdate();
    }
}

void Engine::Update() {
    // Update time at the start of each frame
    Time::Update();

    // Handle scene loading and clear per-scene systems
    if (!SceneDB::next_scene_to_load.empty()) {
        EventSystem::Clear();
        Scheduler::Clear();
        Tween::Clear();
        scene.loadScene();
    }

    // Update timer and tween systems
    float dt = Time::GetDeltaTime();
    Scheduler::Update(dt);
    Tween::Update(dt);

    scene.UpdateScene();
}

void Engine::Render() {
    Renderer::clear(cleanColor);
    
    ImageDB::RenderAndClearAllImages();
    TextDB::RenderQueuedTexts();
    ImageDB::RenderAndClearAllPixels();
    
    Renderer::present();
}
