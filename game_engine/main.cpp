//
//  main.cpp
//  FR-Ocean Engine
//
//  Entry point for the FR-Ocean game engine.
//  Initializes all subsystems and runs the main game loop.
//
//  Command-line options:
//    --resources <path>  Override resources directory (default: resources/)
//    --debug             Enable debug logging
//    --version           Print version and exit
//    --help              Print usage and exit
//
//  Created by Jack Gaffney on 1/21/25.
//

#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <string>
#include "Engine.hpp"
#include "SDL2/SDL.h"
#include "Helper.h"
#include "Renderer.hpp"
#include "ConfigManager.hpp"
#include "Logger.hpp"
#include "EngineException.hpp"

namespace {
    constexpr const char* ENGINE_VERSION = "1.0.0";
    constexpr const char* ENGINE_NAME = "FR-Ocean Engine";

    void PrintUsage(const char* programName) {
        std::cout << "Usage: " << programName << " [options]\n"
                  << "\nOptions:\n"
                  << "  --resources <path>  Override resources directory\n"
                  << "  --debug             Enable debug logging\n"
                  << "  --version           Print version and exit\n"
                  << "  --help              Print this help message\n";
    }

    void PrintVersion() {
        std::cout << ENGINE_NAME << " v" << ENGINE_VERSION << "\n";
    }
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    std::string resourcesPath = "resources/";
    bool debugMode = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            PrintUsage(argv[0]);
            return 0;
        }
        else if (arg == "--version" || arg == "-v") {
            PrintVersion();
            return 0;
        }
        else if (arg == "--debug") {
            debugMode = true;
        }
        else if (arg == "--resources" && i + 1 < argc) {
            resourcesPath = argv[++i];
            // Ensure path ends with separator
            if (!resourcesPath.empty() && resourcesPath.back() != '/') {
                resourcesPath += '/';
            }
        }
        else {
            std::cerr << "Unknown option: " << arg << "\n";
            PrintUsage(argv[0]);
            return 1;
        }
    }

    try {
        // Initialize logging
        Logger::Init();
        if (debugMode) {
            Logger::SetMinLevel(LogLevel::DEBUG);
        }
        LOG_INFO(std::string(ENGINE_NAME) + " v" + ENGINE_VERSION + " starting...");

        // Load configuration
        std::string gameConfigPath = resourcesPath + "game.config";
        std::string renderConfigPath = resourcesPath + "rendering.config";

        ConfigManager::SetResourcesPath(resourcesPath);
        ConfigManager config(gameConfigPath, renderConfigPath);
        ConfigManager::Load();

        // Get configuration values
        std::string gameTitle = ConfigManager::GetGameTitle();
        glm::ivec2 resolution = ConfigManager::GetResolution();
        glm::ivec3 clearColor = ConfigManager::GetClearColor();

        LOG_INFO("Game: " + gameTitle);
        LOG_DEBUG("Resolution: " + std::to_string(resolution.x) + "x" + std::to_string(resolution.y));

        // Initialize renderer
        Renderer renderer(gameTitle, clearColor, resolution);

        // Initialize engine and run game loop
        Engine engine;
        Engine::GameLoop();

        LOG_INFO("Engine shutting down...");
        Logger::Shutdown();
        return 0;
    }
    catch (const EngineException& e) {
        LOG_FATAL(std::string("Engine error: ") + e.what());
        Logger::Shutdown();
        return 1;
    }
    catch (const std::exception& e) {
        LOG_FATAL(std::string("Unexpected error: ") + e.what());
        Logger::Shutdown();
        return 1;
    }
}
