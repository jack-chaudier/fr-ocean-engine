//
//  ConfigManager.cpp
//  game_engine
//
//  Created by Jack Gaffney on 2/8/25.
//

#include <filesystem>
#include "ConfigManager.hpp"
#include "Actor.hpp"
#include "Logger.hpp"
#include "EngineException.hpp"


ConfigManager::ConfigManager(const std::string &gameConfigPath, const std::string &renderConfigPath) {
    ConfigManager::gameConfigPath = gameConfigPath;
    ConfigManager::renderConfigPath = renderConfigPath;
}

void ConfigManager::Load() {
    if (!std::filesystem::exists("resources")) {
        LOG_FATAL("resources/ directory missing");
        throw ConfigurationException("resources/ directory missing");
    }

    LoadGame();
    LoadRender();
}

void ConfigManager::LoadRender() {
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

void ConfigManager::LoadGame() {
    if (!std::filesystem::exists("resources/game.config")) {
        LOG_FATAL("resources/game.config missing");
        throw ConfigurationException("resources/game.config missing");
    }

    EngineUtils::ReadJsonFile(gameConfigPath, gameDoc);
    if (gameDoc.HasMember("game_title")) {
        gameTitle = gameDoc["game_title"].GetString();
    }
    if (gameDoc.HasMember("initial_scene")) {
        initialScene = gameDoc["initial_scene"].GetString();
    } else {
        LOG_FATAL("initial_scene not specified in game.config");
        throw ConfigurationException("initial_scene not specified in game.config");
    }
}


std::string ConfigManager::GetGameTitle() {
    return gameTitle;
}

glm::ivec2 ConfigManager::GetResolution() {
    return resolution;
}

glm::ivec3 ConfigManager::GetClearColor() {
    return color;
}

std::string ConfigManager::GetInitialScene() {
    return initialScene;
}
