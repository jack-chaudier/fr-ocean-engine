//
//  main.cpp
//  FR-Ocean Engine entry point.
//

#include <iostream>
#include <string>
#include "Engine.hpp"
#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"
#include "Renderer.hpp"
#include "ConfigManager.hpp"
#include "Logger.hpp"
#include "EngineException.hpp"

namespace {
    constexpr const char* ENGINE_VERSION = "1.1.0";
    constexpr const char* ENGINE_NAME = "FR-Ocean Engine";

    void PrintUsage(const char* program_name) {
        std::cout
            << "Usage: " << program_name << " [options]\n\n"
            << "Options:\n"
            << "  --resources <path>  Override resources directory (default: resources/)\n"
            << "  --debug             Enable debug logging\n"
            << "  --self-check [N]    Run N frames (default 60) then exit 0. For CI / smoke test.\n"
            << "  --version           Print version and exit\n"
            << "  --help              Print this help message\n";
    }

    void PrintVersion() { std::cout << ENGINE_NAME << " v" << ENGINE_VERSION << "\n"; }

    // SDL lifecycle RAII wrapper so every exit path cleans up.
    struct SdlLifecycle {
        bool sdl_ok = false;
        bool img_ok = false;
        ~SdlLifecycle() {
            if (img_ok) IMG_Quit();
            if (sdl_ok) SDL_Quit();
        }
    };
}

int main(int argc, char* argv[]) {
    std::string resources_path = "resources/";
    bool debug_mode = false;
    int max_frames = -1;  // -1 = no limit

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") { PrintUsage(argv[0]); return 0; }
        if (arg == "--version" || arg == "-v") { PrintVersion(); return 0; }
        if (arg == "--debug") { debug_mode = true; continue; }
        if (arg == "--self-check") {
            max_frames = 60;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                try { max_frames = std::stoi(argv[++i]); } catch (...) {}
            }
            continue;
        }
        if (arg == "--resources" && i + 1 < argc) {
            resources_path = argv[++i];
            if (!resources_path.empty() && resources_path.back() != '/') resources_path += '/';
            continue;
        }
        std::cerr << "Unknown option: " << arg << "\n";
        PrintUsage(argv[0]);
        return 1;
    }

    SdlLifecycle sdl;

    try {
        Logger::Init();
        if (debug_mode) Logger::SetMinLevel(LogLevel::DEBUG);
        LOG_INFO(std::string(ENGINE_NAME) + " v" + ENGINE_VERSION + " starting...");

        ConfigManager::SetResourcesPath(resources_path);
        ConfigManager config(resources_path + "game.config", resources_path + "rendering.config");
        ConfigManager::Load();

        const std::string game_title = ConfigManager::GetGameTitle();
        const glm::ivec2 resolution = ConfigManager::GetResolution();
        glm::ivec3 clear_color = ConfigManager::GetClearColor();

        LOG_INFO("Game: " + game_title);
        LOG_DEBUG("Resolution: " + std::to_string(resolution.x) + "x" + std::to_string(resolution.y));

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
            throw RenderException(std::string("SDL initialization failed: ") + SDL_GetError());
        }
        sdl.sdl_ok = true;

        if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
            throw RenderException(std::string("SDL_image initialization failed: ") + IMG_GetError());
        }
        sdl.img_ok = true;

        {
            Renderer renderer(game_title, clear_color, resolution);
            Engine engine;
            Engine::GameLoop(max_frames);
        }

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
