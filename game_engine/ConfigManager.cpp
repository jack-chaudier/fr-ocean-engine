//
//  ConfigManager.cpp
//  game_engine
//
//  Created by Jack Gaffney on 2/8/25.
//

#include <filesystem>
#include "ConfigManager.hpp"
#include "Actor.hpp"


ConfigManager::ConfigManager(const std::string &gameConfigPath, const std::string &renderConfigPath) {
    ConfigManager::gameConfigPath = gameConfigPath;
    ConfigManager::renderConfigPath = renderConfigPath;
}

void ConfigManager::load() {
    if (!std::filesystem::exists("resources")) {
        std::cout << "error: resources/ missing";
        exit(0);
    }
    
    loadGame();
    loadRender();
}

void ConfigManager::loadRender() {
    if (std::filesystem::exists(renderConfigPath)) {
        EngineUtils::ReadJsonFile(renderConfigPath, renderDoc);
        
        if (renderDoc.HasMember("x_resolution")) {
            resolution.x = renderDoc["x_resolution"].GetInt();
        }
        if (renderDoc.HasMember("y_resolution")) {
            resolution.y = renderDoc["y_resolution"].GetInt();
        }
        if (renderDoc.HasMember("clear_color_r")) {
            color.x = renderDoc["clear_color_r"].GetInt();
        }
        if (renderDoc.HasMember("clear_color_g")) {
            color.y = renderDoc["clear_color_g"].GetInt();
        }
        if (renderDoc.HasMember("clear_color_b")) {
            color.z = renderDoc["clear_color_b"].GetInt();
        }
    }
}

void ConfigManager::loadGame() {
    if (!std::filesystem::exists("resources/game.config")) {
        std::cout << "error: resources/game.config missing";
        exit(0);
    }
    
    EngineUtils::ReadJsonFile(gameConfigPath, gameDoc);
    if (gameDoc.HasMember("game_title")) {
        gameTitle = gameDoc["game_title"].GetString();
    }
    if (gameDoc.HasMember("initial_scene")) {
        initialScene = gameDoc["initial_scene"].GetString();
    } else {
        std::cout << "error: initial_scene unspecified";
        exit(0);
    }
}


std::string ConfigManager::getGameTitle() {
    return gameTitle;
}

glm::ivec2 ConfigManager::getResolution() {
    return resolution;
}

glm::ivec3 ConfigManager::getClearColor() {
    return color;
}

std::string ConfigManager::getInitialScene() {
    return initialScene;
}
