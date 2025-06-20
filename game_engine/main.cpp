//
//  main.cpp
//  game_engine
//
//  Created by Jack Gaffney on 1/21/25.
//

#include <iostream>
#include <filesystem>
#include <cstdlib>
#include "Engine.hpp"
#include "SDL2/SDL.h"
#include "Helper.h"
#include "Renderer.hpp"
#include "ConfigManager.hpp"


int main(int argc, char* argv[]) {
    std::string gpath = "resources/game.config";
    std::string rpath = "resources/rendering.config";
    ConfigManager config(gpath, rpath);
    ConfigManager::load();
    
    std::string gameTitle = ConfigManager::getGameTitle();
    glm::ivec2 resolution = ConfigManager::getResolution();
    glm::ivec3 clearColor = ConfigManager::getClearColor();
    
    Renderer renderer(gameTitle, clearColor, resolution);
    
    Engine engine;
    
    Engine::GameLoop();
    return 0;
}
